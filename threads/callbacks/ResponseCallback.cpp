/**
 * -----------------------------------------------------
 * File        ResponseCallback.cpp
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

#include "ResponseCallback.h"
#include "RequestHandler.h"


ResponseCallback::ResponseCallback(Request *request, std::string error)
    : Callback(request->responseCallbackFunction), request(request), error(error),
    statusCode(0), totalTime(0.0f), downloadSize(0), uploadSize(0), downloadSpeed(0), uploadSpeed(0) {};

ResponseCallback::ResponseCallback(Request *request, CURL *curl, std::string content)
    : Callback(request->responseCallbackFunction), request(request), content(content),
    statusCode(0), totalTime(0.0f), downloadSize(0), uploadSize(0), downloadSpeed(0), uploadSpeed(0) {
    // Get the response code
    long code;
    if (curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code) == CURLE_OK) {
        this->statusCode = static_cast<int>(code);
    }

    // Get the last url
    char *url = nullptr;
    if (curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &url) == CURLE_OK && url) {
        this->lastURL = url;
    }

    // Get the total execution time of the request
    double total;
    if (curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &total) == CURLE_OK) {
        this->totalTime = static_cast<float>(total);
    }

    // Get the download size
    curl_off_t downloadSize;
    if (curl_easy_getinfo(curl, CURLINFO_SIZE_DOWNLOAD_T, &downloadSize) == CURLE_OK) {
        this->downloadSize = static_cast<int>(downloadSize);
    }

    // Get the upload size
    curl_off_t uploadSize;
    if (curl_easy_getinfo(curl, CURLINFO_SIZE_UPLOAD_T, &uploadSize) == CURLE_OK) {
        this->uploadSize = static_cast<int>(uploadSize);
    }

    // Get the download speed
    curl_off_t downloadSpeed;
    if (curl_easy_getinfo(curl, CURLINFO_SPEED_DOWNLOAD_T, &downloadSpeed) == CURLE_OK) {
        this->downloadSpeed = static_cast<int>(downloadSpeed);
    }

    // Get the upload speed
    curl_off_t uploadSpeed;
    if (curl_easy_getinfo(curl, CURLINFO_SPEED_UPLOAD_T, &uploadSpeed) == CURLE_OK) {
        this->uploadSpeed = static_cast<int>(uploadSpeed);
    }
}


void ResponseCallback::Fire() {
    IdentityToken_t *owner = this->request->responseCallbackFunction->plugin->GetIdentity();
    Handle_t responseHandle = BAD_HANDLE;

    if (this->error.empty()) {
        // Create a response handle to this callback on success
        responseHandle = responseCallbackHandler.CreateHandle(this, owner);
        this->request->responseCallbackFunction->function->PushCell(true);
        this->request->responseCallbackFunction->function->PushString("");
    } else {
        this->request->responseCallbackFunction->function->PushCell(false);
        this->request->responseCallbackFunction->function->PushString(this->error.c_str());
    }

    // Create a temporary request handle, so in the callback the correct request will be used
    Handle_t requestHandle = requestHandler.CreateLocaleHandle(this->request, owner);
    this->request->responseCallbackFunction->function->PushCell(requestHandle);

    this->request->responseCallbackFunction->function->PushCell(responseHandle);

    // Fire the PreFire event for subclasses
    this->PreFire();

    // Finally execute the callback
    this->request->responseCallbackFunction->function->Execute(nullptr);

    // Delete the request handle when finished
    if (requestHandle != BAD_HANDLE) {
        requestHandler.FreeHandle(requestHandle, owner);
    }

    // Delete the response handle when finished
    if (responseHandle != BAD_HANDLE) {
        responseCallbackHandler.FreeHandle(responseHandle, owner);
    }
}

void ResponseCallback::Abort() {
    // The request will only be deleted by the handle, but as it will not be invoked we have to delete it manually
    delete this->request;
}