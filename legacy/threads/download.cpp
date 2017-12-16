/**
 * -----------------------------------------------------
 * File        download.cpp
 * Authors     David Ordnung
 * License     GPLv3
 * Web         http://dordnung.de
 * -----------------------------------------------------
 *
 * Copyright (C) 2013-2017 David Ordnung
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

#include "download.h"


LegacyDownloadCallback::LegacyDownloadCallback(std::string curlError, int data, IPluginFunction *callback) {
    this->finished = true;
    this->curlError = curlError;
    this->dlTotal = 0.0f;
    this->dlNow = 0.0f;
    this->ulTotal = 0.0f;
    this->ulNow = 0.0f;
    this->data = data;
    this->callback = callback;
}

LegacyDownloadCallback::LegacyDownloadCallback(bool finished, std::string curlError, float dlTotal, float dlNow, float ulTotal, float ulNow, int data, IPluginFunction *callback) {
    this->finished = finished;
    this->curlError = curlError;
    this->dlTotal = dlTotal;
    this->dlNow = dlNow;
    this->ulTotal = ulTotal;
    this->ulNow = ulNow;
    this->data = data;
    this->callback = callback;
}

void LegacyDownloadCallback::Fire() {
    this->callback->PushCell(this->finished);
    this->callback->PushString(this->curlError.c_str());
    this->callback->PushFloat(this->dlTotal);
    this->callback->PushFloat(this->dlNow);
    this->callback->PushFloat(this->ulTotal);
    this->callback->PushFloat(this->ulNow);
    this->callback->PushCell(this->data);
    this->callback->Execute(NULL);
}



LegacyDownloadThread::LegacyDownloadThread(std::string url, std::string localFile, int data, IPluginFunction *callback) : IThread() {
    this->url = url;
    this->localFile = localFile;
    this->data = data;
    this->callback = callback;
}

void LegacyDownloadThread::RunThread(IThreadHandle *pHandle) {
    // Get full path to the local file and open it
    char fullLocalPath[PLATFORM_MAX_PATH + 1];
    g_pSM->BuildPath(Path_Game, fullLocalPath, sizeof(fullLocalPath), this->localFile.c_str());

    // Could we open the file?
    FILE *stream = fopen(fullLocalPath, "wb");
    if (!stream) {
        system2Extension.AppendCallback(std::make_shared<LegacyDownloadCallback>("Couldn't create file", this->data, this->callback));
        return;
    }

    // Init. Curl
    std::string error;
    CURL *curl = curl_easy_init();
    if (curl) {
        char errorBuffer[CURL_ERROR_SIZE + 1];

        // Get progress info
        progress_info progress =
        {
            0,
            this->data,
            this->callback,
        };

        // Set up Curl
        curl_easy_setopt(curl, CURLOPT_URL, this->url.c_str());
        curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 20);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, file_write);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, stream);
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
        curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
        curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, progress_updated);
        curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, &progress);

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
    system2Extension.AppendCallback(std::make_shared<LegacyDownloadCallback>(error, this->data, this->callback));
}


size_t file_write(void *buffer, size_t size, size_t nmemb, void *userdata) {
    // Write to the file
    return fwrite(buffer, size, nmemb, (FILE *)userdata);
}


int progress_updated(void *data, double dltotal, double dlnow, double ultotal, double ulnow) {
    progress_info *progress = (progress_info *)data;

    if ((dlnow > 0.0 || dltotal > 0.0 || ultotal > 0.0 || ulnow > 0.0) && (system2Extension.GetFrames() != progress->lastFrame)) {
        // Add return status to queue
        system2Extension.AppendCallback(std::make_shared<LegacyDownloadCallback>(false, "", (float)dltotal, (float)dlnow, (float)ultotal, (float)ulnow, progress->data, progress->callback));
    }

    // Save current frame
    progress->lastFrame = system2Extension.GetFrames();

    return 0;
}