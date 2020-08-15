/**
 * -----------------------------------------------------
 * File        RequestThread.cpp
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

#include "RequestThread.h"
#include "ProgressCallback.h"

// Set initial last progress frame
uint32_t RequestThread::lastProgressFrame = 0;


RequestThread::RequestThread(Request *request) : Thread(), request(request) {};

bool RequestThread::ApplyRequest(CURL *curl, WriteDataInfo &writeData) {
    // Set URL and port
    curl_easy_setopt(curl, CURLOPT_URL, this->request->url.c_str());
    if (this->request->port >= 0) {
        curl_easy_setopt(curl, CURLOPT_PORT, this->request->port);
    }

    // Disable SSL verifying if wanted
    if (!this->request->verifySSL) {
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    } else {
#if defined unix || defined __unix__ || defined __linux__ || defined __unix || defined __APPLE__ || defined __darwin__
        // Use our own ca-bundle on unix like systems
        std::string caFile = system2Extension.GetCertificateFile();
        if (!caFile.empty()) {
            curl_easy_setopt(curl, CURLOPT_CAINFO, caFile.c_str());
        }
#endif
    }

    // Set proxy with username and password
    if (!this->request->proxy.empty()) {
        curl_easy_setopt(curl, CURLOPT_PROXY, this->request->proxy.c_str());

        // Set the username for the proxy
        if (!this->request->proxyUsername.empty()) {
            curl_easy_setopt(curl, CURLOPT_PROXYUSERNAME, this->request->proxyUsername.c_str());
        }

        // Set the password for the proxy
        if (!this->request->proxyPassword.empty()) {
            curl_easy_setopt(curl, CURLOPT_PROXYPASSWORD, this->request->proxyPassword.c_str());
        }

        // Set http tunneling
        if (this->request->proxyHttpTunnel) {
            curl_easy_setopt(curl, CURLOPT_HTTPPROXYTUNNEL, 1L);
            curl_easy_setopt(curl, CURLOPT_SUPPRESS_CONNECT_HEADERS, 1L);
        }
    }

    // Check if also write to an output file
    if (!this->request->outputFile.empty()) {
        // Get the full path to the file
        char filePath[PLATFORM_MAX_PATH + 1];
        smutils->BuildPath(Path_Game, filePath, sizeof(filePath), this->request->outputFile.c_str());

        // Open the file writeable
        writeData.file = fopen(filePath, "wb");
        if (!writeData.file) {
            return false;
        }
    }

    // Set the write function and data
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, RequestThread::WriteData);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &writeData);

    // Set progress function
    if (this->request->progressCallbackFunction) {
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
        curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, RequestThread::ProgressUpdated);
        curl_easy_setopt(curl, CURLOPT_XFERINFODATA, this);
    }

    // Set timeout
    if (this->request->timeout >= 0) {
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, this->request->timeout);
    } else {
        // Set connect timeout to a better default value
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 60);
    }

    // Prevent signals to interrupt our thread
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);

    return true;
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
    RequestThread *requestThread = static_cast<RequestThread *>(clientp);

    if ((dlnow > 0.0 || dltotal > 0.0 || ultotal > 0.0 || ulnow > 0.0) && (system2Extension.GetFrames() != requestThread->lastProgressFrame)) {
        // Append progress callback
        system2Extension.AppendCallback(std::make_shared<ProgressCallback>(requestThread->request->Clone(),
                                                                           static_cast<int>(dltotal),
                                                                           static_cast<int>(dlnow),
                                                                           static_cast<int>(ultotal),
                                                                           static_cast<int>(ulnow),
                                                                           requestThread->request->data));
    }

    // Save current frame
    requestThread->lastProgressFrame = system2Extension.GetFrames();

    return 0;
}