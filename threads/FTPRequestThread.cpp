/**
 * -----------------------------------------------------
 * File        FTPRequestThread.cpp
 * Authors     David Ordnung
 * License     GPLv3
 * Web         http://dordnung.de
 * -----------------------------------------------------
 *
 * Copyright (C) 2013-2020 David Ordnung
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>
 */

#include "FTPRequestThread.h"
#include "FTPResponseCallback.h"


FTPRequestThread::FTPRequestThread(FTPRequest *ftpRequest) : RequestThread(ftpRequest), ftpRequest(ftpRequest) {};

void FTPRequestThread::Run() {
    // Create a curl object
    CURL *curl = curl_easy_init();

    if (curl) {
        // Apply general request stuff
        WriteDataInfo writeData = { std::string(), nullptr };
        if (!this->ApplyRequest(curl, writeData)) {
            system2Extension.AppendCallback(std::make_shared<FTPResponseCallback>(this->ftpRequest, "Can not open output file"));
            curl_easy_cleanup(curl);

            return;
        }

        // Collect error information
        char errorBuffer[CURL_ERROR_SIZE + 1];
        curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);

        // Use FTP if no scheme is given
        curl_easy_setopt(curl, CURLOPT_DEFAULT_PROTOCOL, "ftp");

        // Set the ftp username
        if (!this->ftpRequest->username.empty()) {
            curl_easy_setopt(curl, CURLOPT_USERNAME, this->ftpRequest->username.c_str());
        }

        // Set the ftp password
        if (!this->ftpRequest->password.empty()) {
            curl_easy_setopt(curl, CURLOPT_PASSWORD, this->ftpRequest->password.c_str());
        }

        FILE *inputFile = nullptr;
        if (!this->ftpRequest->inputFile.empty()) {
            // Get the full path to the file
            char filePath[PLATFORM_MAX_PATH + 1];
            smutils->BuildPath(Path_Game, filePath, sizeof(filePath), this->ftpRequest->inputFile.c_str());

            // Open the file readable
            inputFile = fopen(filePath, "rb");
            if (!inputFile) {
                // Create error callback and clean up curl
                system2Extension.AppendCallback(std::make_shared<FTPResponseCallback>(this->ftpRequest, "Can not open file to upload"));
                curl_easy_cleanup(curl);

                // Close output file if opened
                if (writeData.file) {
                    fclose(writeData.file);
                }

                return;
            }

            // Get the size of the file
            fseek(inputFile, 0L, SEEK_END);
            curl_off_t fsize = (curl_off_t)ftell(inputFile);
            fseek(inputFile, 0L, SEEK_SET);

            // Set CURL to upload a file
            curl_easy_setopt(curl, CURLOPT_READFUNCTION, RequestThread::ReadFile);
            curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
            curl_easy_setopt(curl, CURLOPT_FTP_CREATE_MISSING_DIRS, this->ftpRequest->createMissingDirs ? CURLFTP_CREATE_DIR : CURLFTP_CREATE_DIR_NONE);
            curl_easy_setopt(curl, CURLOPT_APPEND, this->ftpRequest->appendToFile ? 1L : 0L);
            curl_easy_setopt(curl, CURLOPT_READDATA, inputFile);
            curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, fsize);
        } else {
            if (this->ftpRequest->listFilenamesOnly) {
                curl_easy_setopt(curl, CURLOPT_DIRLISTONLY, 1L);
            }
        }

        // Only one process can be connect to FTP
        std::shared_ptr<FTPResponseCallback> callback;
        {
            std::lock_guard<std::mutex> lock(this->mutex);

            // Perform curl operation and create the callback
            if (curl_easy_perform(curl) == CURLE_OK) {
                callback = std::make_shared<FTPResponseCallback>(this->ftpRequest, curl, writeData.content);
            } else {
                if (!strlen(errorBuffer)) {
                    // Set readable error if there is no one
                    callback = std::make_shared<FTPResponseCallback>(this->ftpRequest, "Couldn't execute FTP request");
                } else {
                    callback = std::make_shared<FTPResponseCallback>(this->ftpRequest, errorBuffer);
                }
            }
        }

        // Clean up curl
        curl_easy_cleanup(curl);

        // Close input file if opened
        if (inputFile) {
            fclose(inputFile);
        }

        // Also close output file if opened
        if (writeData.file) {
            fclose(writeData.file);
        }

        // Append callback so it can be fired
        system2Extension.AppendCallback(callback);
    } else {
        system2Extension.AppendCallback(std::make_shared<FTPResponseCallback>(this->ftpRequest, "Couldn't initialize CURL"));
    }
}