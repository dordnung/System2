/**
 * -----------------------------------------------------
 * File        ResponseCallback.cpp
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

#include "ResponseCallback.h"
#include "ResponseHandler.h"


ResponseCallback::ResponseCallback(std::string error, Request *request, Handle_t requestHandle, IdentityToken_t *owner)
    : success(true), error(error), request(request), requestHandle(requestHandle),
    statusCode(-1), totalTime(0.0f), owner(owner) {};


ResponseCallback::ResponseCallback(CURL *curl, std::string content, Request *request, Handle_t requestHandle, IdentityToken_t *owner)
    : success(true), content(content), request(request), requestHandle(requestHandle), owner(owner) {
    // Get the response code
    long code;
    if (curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code) == CURLE_OK) {
        this->statusCode = (int)code;
    } else {
        this->statusCode = 0;
    }

    // Get the last url
    char *url = NULL;
    if (curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &url) == CURLE_OK && url) {
        this->lastURL = url;
    } else {
        this->lastURL = std::string();
    }

    double total;
    if (curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &total) == CURLE_OK) {
        this->totalTime = (float)total;
    } else {
        this->totalTime = 0.0f;
    }
}


void ResponseCallback::Fire() {
    // Create the response handle
    Handle_t hndl = BAD_HANDLE;
    if (this->success) {
        hndl = handlesys->CreateHandle(responseHandleType,
                                       this,
                                       this->owner,
                                       myself->GetIdentity(),
                                       NULL);
        this->request->responseCallback->PushCell(true);
        this->request->responseCallback->PushString("");
    } else {
        this->request->responseCallback->PushCell(false);
        this->request->responseCallback->PushString(this->error.c_str());
    }

    this->request->responseCallback->PushCell(this->requestHandle);
    this->request->responseCallback->PushCell(hndl);
    this->request->responseCallback->Execute(NULL);

    // Delete the response handle when finished
    if (hndl != BAD_HANDLE) {
        HandleSecurity sec = { this->owner, myself->GetIdentity() };
        handlesys->FreeHandle(hndl, &sec);
    }

    // Delete the request handle when finished and auto clean up is enabled
    if (this->request->autoClean) {
        HandleSecurity sec = { this->owner, myself->GetIdentity() };
        handlesys->FreeHandle(this->requestHandle, &sec);
    }
}