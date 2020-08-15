/**
 * -----------------------------------------------------
 * File        HTTPRequest.cpp
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

#include "HTTPRequest.h"
#include "HTTPRequestThread.h"

HTTPRequest::HTTPRequest(std::string url, std::shared_ptr<CallbackFunction_t> responseCallbackFunction)
    : Request(url, responseCallbackFunction), followRedirects(true) {}

HTTPRequest::HTTPRequest(const HTTPRequest& request) :
    Request(request), bodyData(request.bodyData), headers(request.headers), userAgent(request.userAgent),
    username(request.username), password(request.password), followRedirects(request.followRedirects) {}

HTTPRequest* HTTPRequest::Clone() const {
    return new HTTPRequest(*this);
}

void HTTPRequest::Get() {
    MakeThread(METHOD_GET);
}

void HTTPRequest::Post() {
    MakeThread(METHOD_POST);
}

void HTTPRequest::Put() {
    MakeThread(METHOD_PUT);
}

void HTTPRequest::Patch() {
    MakeThread(METHOD_PATCH);
}

void HTTPRequest::Delete() {
    MakeThread(METHOD_DELETE);
}

void HTTPRequest::Head() {
    MakeThread(METHOD_HEAD);
}

void HTTPRequest::MakeThread(HTTPRequestMethod method) {
    // Make a copy for the thread, so it works independent
    HTTPRequestThread* requestThread = new HTTPRequestThread(this->Clone(), method);
    requestThread->RunThread();
}