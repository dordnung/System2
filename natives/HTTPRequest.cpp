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


HTTPRequest::HTTPRequest(std::string url, IPluginFunction *responseCallback) : Request(url, responseCallback), followRedirects(true), autoReferer(false) {};

HTTPRequest::HTTPRequest(const HTTPRequest &request) :
    Request(request), data(request.data), outputFile(request.outputFile), headers(request.headers), userAgent(request.userAgent),
    username(request.username), password(request.password), followRedirects(request.followRedirects), autoReferer(request.autoReferer) {};


void HTTPRequest::Get() {
    this->makeThread(METHOD_GET);
}

void HTTPRequest::Post() {
    this->makeThread(METHOD_POST);
}

void HTTPRequest::Put() {
    this->makeThread(METHOD_PUT);
}

void HTTPRequest::Patch() {
    this->makeThread(METHOD_PATCH);
}

void HTTPRequest::Delete() {
    this->makeThread(METHOD_DELETE);
}

void HTTPRequest::Head() {
    this->makeThread(METHOD_HEAD);
}


void HTTPRequest::makeThread(HTTPRequestMethod method) {
    // Make a copy for the thread, so it works independent
    HTTPRequestThread *requestThread = new HTTPRequestThread(new HTTPRequest(*this), method);
    threader->MakeThread(requestThread);
}