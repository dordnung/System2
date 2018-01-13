/**
 * -----------------------------------------------------
 * File        FTPRequestThread.cpp
 * Authors     David Ordnung
 * License     GPLv3
 * Web         http://dordnung.de
 * -----------------------------------------------------
 *
 * Copyright (C) 2013-2018 David Ordnung
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


 // Only allow one FTP connection at the same time, because of RFC does not allow multiple connections
IMutex *ftpMutex;

FTPRequestThread::FTPRequestThread(FTPRequest *ftpRequest, bool isDownload)
    : RequestThread(ftpRequest), ftpRequest(ftpRequest), isDownload(isDownload) {};


void FTPRequestThread::RunThread(IThreadHandle *pHandle) {
    // Create a curl object
    CURL *curl = curl_easy_init();

    if (curl) {
        // Apply general request stuff
        this->ApplyRequest(curl);

        // Collect error information
        char errorBuffer[CURL_ERROR_SIZE + 1];
        curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);

        // Get the full path to the file
        char filePath[PLATFORM_MAX_PATH + 1];
        smutils->BuildPath(Path_Game, filePath, sizeof(filePath), this->ftpRequest->file.c_str());

        FILE *file = NULL;
        if (this->isDownload) {
            // Open the file writeable
            file = fopen(filePath, "wb");
        } else {
            // Open the file readable
            file = fopen(filePath, "rb");
        }

        // Check if file could be opened
        if (!file) {
            // Create error callback and clean up curl
            system2Extension.AppendCallback(std::make_shared<FTPResponseCallback>(this->ftpRequest, "Can not open local file", this->isDownload));
            curl_easy_cleanup(curl);

            return;
        }

        WriteDataInfo writeData = { std::string(), file };
        if (this->isDownload) {
            // Set the write function and data
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, RequestThread::WriteData);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &writeData);
        } else {
            // Get the size of the file
            fseek(file, 0L, SEEK_END);
            curl_off_t fsize = (curl_off_t)ftell(file);
            fseek(file, 0L, SEEK_SET);

            curl_easy_setopt(curl, CURLOPT_READFUNCTION, RequestThread::ReadFile);
            curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
            curl_easy_setopt(curl, CURLOPT_FTP_CREATE_MISSING_DIRS, this->ftpRequest->createMissingDirs ? CURLFTP_CREATE_DIR : CURLFTP_CREATE_DIR_NONE);
            curl_easy_setopt(curl, CURLOPT_APPEND, this->ftpRequest->appendToFile ? 1L : 0L);
            curl_easy_setopt(curl, CURLOPT_READDATA, file);
            curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, fsize);
        }

        // Only one process can be connect to FTP
        ftpMutex->Lock();

        // Perform curl operation and create the callback
        std::shared_ptr<FTPResponseCallback> callback;
        if (curl_easy_perform(curl) == CURLE_OK) {
            callback = std::make_shared<FTPResponseCallback>(this->ftpRequest, curl, writeData.content, this->isDownload);
        } else {
            if (!strlen(errorBuffer)) {
                // Set readable error if there is no one
                callback = std::make_shared<FTPResponseCallback>(this->ftpRequest, "Couldn't execute FTP request", this->isDownload);
            } else {
                callback = std::make_shared<FTPResponseCallback>(this->ftpRequest, errorBuffer, this->isDownload);
            }
        }

        // Only one process can be connect to FTP
        ftpMutex->Unlock();

        // Clean up curl
        curl_easy_cleanup(curl);

        // Also close locale file if opened
        if (file) {
            fclose(file);
        }

        // Append callback so it can be fired
        system2Extension.AppendCallback(callback);
    } else {
        system2Extension.AppendCallback(std::make_shared<FTPResponseCallback>(this->ftpRequest, "Couldn't initialize CURL", this->isDownload));
    }
}