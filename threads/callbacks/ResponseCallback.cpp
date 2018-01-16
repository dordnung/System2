/**
 * -----------------------------------------------------
 * File        ResponseCallback.cpp
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

#include "ResponseCallback.h"
#include "RequestHandler.h"


ResponseCallback::ResponseCallback(Request *request, std::string error)
    : Callback(request->responseCallbackFunction), request(request), error(error), statusCode(0), totalTime(0.0f) {};

ResponseCallback::ResponseCallback(Request *request, CURL *curl, std::string content)
    : Callback(request->responseCallbackFunction), request(request), content(content) {
    // Get the response code
    long code;
    if (curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code) == CURLE_OK) {
        this->statusCode = static_cast<int>(code);
    } else {
        this->statusCode = 0;
    }

    // Get the last url
    char *url = NULL;
    if (curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &url) == CURLE_OK && url) {
        this->lastURL = url;
    }

    // Get the total execution time of the request
    double total;
    if (curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &total) == CURLE_OK) {
        this->totalTime = static_cast<float>(total);
    } else {
        this->totalTime = 0.0f;
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
    this->request->responseCallbackFunction->function->Execute(NULL);

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