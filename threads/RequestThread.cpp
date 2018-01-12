/**
 * -----------------------------------------------------
 * File        RequestThread.cpp
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

#include "RequestThread.h"
#include "ProgressCallback.h"

// Set initial last progress frame
uint32_t RequestThread::lastProgressFrame = 0;


RequestThread::RequestThread(Request *request) : request(request) {};


void RequestThread::ApplyRequest(CURL *curl) {
    // Set URL and port
    curl_easy_setopt(curl, CURLOPT_URL, this->request->url.c_str());
    if (this->request->port > 0) {
        curl_easy_setopt(curl, CURLOPT_PORT, this->request->port);
    }

    // Set progress function
    if (this->request->progressCallback != NULL) {
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
        curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, RequestThread::ProgressUpdated);
        curl_easy_setopt(curl, CURLOPT_XFERINFODATA, this);
    }

    // Set timeout
    if (this->request->timeout > 0) {
        curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, this->request->timeout);
    }

    // Prevent signals to interrupt our thread
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
}


void RequestThread::OnTerminate(IThreadHandle *pThread, bool cancel) {
    delete this;
}


size_t RequestThread::WriteData(char *ptr, size_t size, size_t nmemb, void *userdata) {
    // Get the data info
    RequestThread::WriteDataInfo *dataInfo = (RequestThread::WriteDataInfo *)userdata;

    // Add to content
    size_t realsize = size * nmemb;
    dataInfo->content.append(ptr, realsize);

    // Write to the file if any file is opened
    if (dataInfo->file) {
        return fwrite(ptr, size, nmemb, dataInfo->file);
    }

    return realsize;
}


size_t RequestThread::ReadFile(char *buffer, size_t size, size_t nitems, void *instream) {
    // Just read the content from the file
    return fread(buffer, size, nitems, (FILE *)instream);
}


size_t RequestThread::ProgressUpdated(void *clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow) {
    RequestThread *requestThread = (RequestThread *)clientp;

    if ((dlnow > 0.0 || dltotal > 0.0 || ultotal > 0.0 || ulnow > 0.0) && (system2Extension.GetFrames() != requestThread->lastProgressFrame)) {
        // Append progress callback
        system2Extension.AppendCallback(std::make_shared<ProgressCallback>(requestThread->request, (int)dltotal, (int)dlnow, (int)ultotal, (int)ulnow, requestThread->request->data));
    }

    // Save current frame
    requestThread->lastProgressFrame = system2Extension.GetFrames();

    return 0;
}