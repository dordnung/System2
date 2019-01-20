/**
 * -----------------------------------------------------
 * File        HTTPRequest.cpp
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

#include "HTTPRequest.h"
#include "HTTPRequestThread.h"


HTTPRequest::HTTPRequest(std::string url, std::shared_ptr<CallbackFunction_t> responseCallbackFunction)
    : Request(url, responseCallbackFunction), followRedirects(true) {}

HTTPRequest::HTTPRequest(const HTTPRequest &request) :
    Request(request), bodyData(request.bodyData), headers(request.headers), userAgent(request.userAgent),
    username(request.username), password(request.password), followRedirects(request.followRedirects), form(request.form) {}


HTTPRequest *HTTPRequest::Clone() const {
    return new HTTPRequest(*this);
}


bool HTTPRequest::Get() {
    return this->MakeThread(METHOD_GET);
}

bool HTTPRequest::Post() {
    return this->MakeThread(METHOD_POST);
}

bool HTTPRequest::Put() {
    return this->MakeThread(METHOD_PUT);
}

bool HTTPRequest::Patch() {
    return this->MakeThread(METHOD_PATCH);
}

bool HTTPRequest::Delete() {
    return this->MakeThread(METHOD_DELETE);
}

bool HTTPRequest::Head() {
    return this->MakeThread(METHOD_HEAD);
}


bool HTTPRequest::MakeThread(HTTPRequestMethod method) {
    // Make a copy for the thread, so it works independent
    HTTPRequestThread *requestThread = new HTTPRequestThread(this->Clone(), method);
    if (!system2Extension.RegisterAndStartThread(requestThread)) {
        delete requestThread->httpRequest;
        delete requestThread;

        return false;
    }

    return true;
}