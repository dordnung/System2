/**
 * -----------------------------------------------------
 * File        LegacyFTPThread.cpp
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

#include "LegacyFTPThread.h"
#include "LegacyDownloadCallback.h"
#include "LegacyDownloadThread.h"


// Only allow one FTP connection at the same time, because of RFC does not allow multiple connections
std::mutex legacyFTPMutex;

LegacyFTPThread::LegacyFTPThread(bool download, std::string remoteFile, std::string localFile, std::string url,
                                 std::string user, std::string pw, int port, int data, std::shared_ptr<CallbackFunction_t> callbackFunction)
    : Thread(), download(download), remoteFile(remoteFile), localFile(localFile), host(url),
    username(user), password(pw), port(port), data(data), callbackFunction(callbackFunction) {}


void LegacyFTPThread::Run() {
    // Get the full path to the local file
    char fullLocalFilePath[PLATFORM_MAX_PATH + 1];
    g_pSM->BuildPath(Path_Game, fullLocalFilePath, sizeof(fullLocalFilePath), this->localFile.c_str());

    // Open the local file
    FILE *localFile = nullptr;
    if (this->download) {
        // When downloading open writeable
        localFile = fopen(fullLocalFilePath, "wb");
    } else {
        // When uploading open readable
        localFile = fopen(fullLocalFilePath, "rb");
    }

    // Check if local file could be open
    if (!localFile) {
        system2Extension.AppendCallback(std::make_shared<LegacyDownloadCallback>(this->callbackFunction, "Couldn't open locale file", this->data));
        return;
    }

    // Only one process can be connect to FTP
    std::string error;
    {
        std::lock_guard<std::mutex> lock(this->mutex);

        // Init. Curl
        CURL* curl = curl_easy_init();
        if (curl) {
            char errorBuffer[CURL_ERROR_SIZE + 1];

            // Get progress info
            LegacyDownloadThread::ProgressInfo progress =
            {
                0,
                this->data,
                this->callbackFunction
            };

            // Get whole URL
            std::string fullURL = "ftp://" + this->host + "/" + this->remoteFile;

            // Set up curl
            curl_easy_setopt(curl, CURLOPT_URL, fullURL.c_str());
            curl_easy_setopt(curl, CURLOPT_PORT, this->port);
            curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);
            curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
            curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
            curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, LegacyDownloadThread::ProgressUpdated);
            curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, &progress);

#if defined unix || defined __unix__ || defined __linux__ || defined __unix || defined __APPLE__ || defined __darwin__
            // Use our own ca-bundle on unix like systems
            std::string caFile = system2Extension.GetCertificateFile();
            if (!caFile.empty()) {
                curl_easy_setopt(curl, CURLOPT_CAINFO, caFile.c_str());
            }
#endif

            // Login?
            if (!this->username.empty()) {
                std::string loginData = this->username + ":" + this->password;
                curl_easy_setopt(curl, CURLOPT_USERPWD, loginData.c_str());
            }

            // Upload stuff
            if (!download) {
                // Get the size of the file
                fseek(localFile, 0L, SEEK_END);
                curl_off_t fsize = (curl_off_t)ftell(localFile);
                fseek(localFile, 0L, SEEK_SET);

                curl_easy_setopt(curl, CURLOPT_READFUNCTION, UploadFTP);
                curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
                curl_easy_setopt(curl, CURLOPT_FTP_CREATE_MISSING_DIRS, CURLFTP_CREATE_DIR_RETRY);
                curl_easy_setopt(curl, CURLOPT_READDATA, localFile);
                curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, fsize);
            } else {
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, LegacyDownloadThread::WriteFile);
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, localFile);
            }

            // Perform and clean
            if (curl_easy_perform(curl) == CURLE_OK) {
                // Clean error buffer if there was no error
                error.clear();
            } else if (strlen(errorBuffer) < 2) {
                // Set readable error if there is no one
                error = "Couldn't execute FTP command";
            } else {
                error = errorBuffer;
            }

            curl_easy_cleanup(curl);
        }

        // Close file
        fclose(localFile);
    }

    // Add return status to queue
    system2Extension.AppendCallback(std::make_shared<LegacyDownloadCallback>(this->callbackFunction, error, this->data));
}


size_t LegacyFTPThread::UploadFTP(void *buffer, size_t size, size_t nmemb, void *userdata) {
    // Read file and return size
    return fread(buffer, size, nmemb, (FILE *)userdata);
}