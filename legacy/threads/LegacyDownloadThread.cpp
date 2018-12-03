/**
 * -----------------------------------------------------
 * File        LegacyDownloadThread.cpp
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

#include "LegacyDownloadThread.h"
#include "LegacyDownloadCallback.h"


LegacyDownloadThread::LegacyDownloadThread(std::string url, std::string localFile, int data, std::shared_ptr<CallbackFunction_t> callbackFunction)
    : IThread(), url(url), localFile(localFile), data(data), callbackFunction(callbackFunction) {}


void LegacyDownloadThread::RunThread(IThreadHandle *pHandle) {
    // Get full path to the local file and open it
    char fullLocalPath[PLATFORM_MAX_PATH + 1];
    g_pSM->BuildPath(Path_Game, fullLocalPath, sizeof(fullLocalPath), this->localFile.c_str());

    // Could we open the file?
    FILE *stream = fopen(fullLocalPath, "wb");
    if (!stream) {
        system2Extension.AppendCallback(std::make_shared<LegacyDownloadCallback>(this->callbackFunction, "Couldn't create file", this->data));
        return;
    }

    // Init. Curl
    std::string error;
    CURL *curl = curl_easy_init();
    if (curl) {
        char errorBuffer[CURL_ERROR_SIZE + 1];

        // Get progress info
        ProgressInfo progress =
        {
            0,
            this->data,
            this->callbackFunction,
        };

        // Set up Curl
        curl_easy_setopt(curl, CURLOPT_URL, this->url.c_str());
        curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 20);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteFile);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, stream);
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
        curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
        curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, ProgressUpdated);
        curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, &progress);
        
#if defined unix || defined __unix__ || defined __linux__ || defined __unix || defined __APPLE__ || defined __darwin__
        // Use our own ca-bundle on unix like systems
        char caPath[PLATFORM_MAX_PATH + 1];
        smutils->BuildPath(Path_SM, caPath, sizeof(caPath), "data/system2/ca-bundle.crt");

        curl_easy_setopt(curl, CURLOPT_CAINFO, caPath);
#endif
        
        // Perform and clean
        if (curl_easy_perform(curl) == CURLE_OK) {
            // Clean error buffer if there was no error
            error.clear();
        } else if (strlen(errorBuffer) < 2) {
            // Set readable error if there is no one
            error = "Couldn't execute download command";
        } else {
            error = errorBuffer;
        }

        curl_easy_cleanup(curl);
    }


    // Close file
    fclose(stream);

    // Add return status to queue
    system2Extension.AppendCallback(std::make_shared<LegacyDownloadCallback>(this->callbackFunction, error, this->data));
}


void LegacyDownloadThread::OnTerminate(IThreadHandle *pThread, bool cancel) {
    system2Extension.UnregisterAndDeleteThreadHandle(pThread);
    delete this;
}


size_t LegacyDownloadThread::WriteFile(void *buffer, size_t size, size_t nmemb, void *userdata) {
    // Write to the file
    return fwrite(buffer, size, nmemb, (FILE *)userdata);
}


int LegacyDownloadThread::ProgressUpdated(void *data, double dltotal, double dlnow, double ultotal, double ulnow) {
    ProgressInfo *progress = (ProgressInfo *)data;

    if ((dlnow > 0.0 || dltotal > 0.0 || ultotal > 0.0 || ulnow > 0.0) && (system2Extension.GetFrames() != progress->lastFrame)) {
        // Add return status to queue
        system2Extension.AppendCallback(std::make_shared<LegacyDownloadCallback>(
            progress->callbackFunction, false, "", (float)dltotal, (float)dlnow, (float)ultotal, (float)ulnow, progress->data));
    }

    // Save current frame
    progress->lastFrame = system2Extension.GetFrames();

    return 0;
}