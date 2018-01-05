/**
 * -----------------------------------------------------
 * File        RequestThread.cpp
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

#include "RequestThread.h"


RequestThread::RequestThread(Request *request, Handle_t requestHandle, IdentityToken_t *owner)
    : request(request), requestHandle(requestHandle), owner(owner) {};


void RequestThread::ApplyRequest(CURL *curl) {
    // Set URL and port
    curl_easy_setopt(curl, CURLOPT_URL, this->request->url.c_str());
    if (this->request->port > 0) {
        curl_easy_setopt(curl, CURLOPT_PORT, this->request->port);
    }

    // Set progress function
    if (this->request->progressCallback != NULL) {
        ProgressInfo progress = { 0, this };

        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
        curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, RequestThread::ProgressUpdated);
        curl_easy_setopt(curl, CURLOPT_XFERINFODATA, &progress);
    }

    // Set timeout
    if (this->request->timeout > 0) {
        curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, this->request->timeout);
    }

    // Set connect timeout to an better default value
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 30);

    // Prevent signals to interrupt our thread
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
}


void RequestThread::OnTerminate(IThreadHandle *pThread, bool cancel) {
    delete this;
}