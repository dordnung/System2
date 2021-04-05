/**
 * -----------------------------------------------------
 * File        HTTPResponseCallback.cpp
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

#include "HTTPResponseCallback.h"

HTTPResponseCallback::HTTPResponseCallback(HTTPRequest* httpRequest, std::string error, HTTPRequestMethod requestMethod)
    : ResponseCallback(httpRequest, error), requestMethod(requestMethod), httpVersion(CURL_HTTP_VERSION_NONE) {}

HTTPResponseCallback::HTTPResponseCallback(HTTPRequest* httpRequest, CURL* curl, std::string content, size_t contentLength,
                                           HTTPRequestMethod requestMethod, std::map<std::string, std::string> headers)
    : ResponseCallback(httpRequest, curl, content, contentLength), requestMethod(requestMethod), headers(headers), httpVersion(CURL_HTTP_VERSION_NONE) {
    // Get the http version
    long version;
    if (curl_easy_getinfo(curl, CURLINFO_HTTP_VERSION, &version) == CURLE_OK) {
        this->httpVersion = static_cast<int>(version);
    }

    // Get the content type
    char* contentType = nullptr;
    if (curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &contentType) == CURLE_OK && contentType) {
        this->contentType = contentType;
    }
}

void HTTPResponseCallback::PreFire() {
    // Push the request method for a HTTP request
    this->request->responseCallbackFunction->function->PushCell(this->requestMethod);
}