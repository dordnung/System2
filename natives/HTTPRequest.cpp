/**
 * -----------------------------------------------------
 * File        HTTPRequest.cpp
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

#include "HTTPRequest.h"
#include "HTTPRequestThread.h"


HTTPRequest::HTTPRequest(std::string url, IPluginFunction *responseCallback) : Request(url, responseCallback), followRedirects(true), autoReferer(false) {};


void HTTPRequest::Get(Handle_t requestHandle, IdentityToken_t *owner) {
    this->makeThread(METHOD_GET, requestHandle, owner);
}

void HTTPRequest::Post(Handle_t requestHandle, IdentityToken_t *owner) {
    this->makeThread(METHOD_POST, requestHandle, owner);
}

void HTTPRequest::Put(Handle_t requestHandle, IdentityToken_t *owner) {
    this->makeThread(METHOD_PUT, requestHandle, owner);
}

void HTTPRequest::Patch(Handle_t requestHandle, IdentityToken_t *owner) {
    this->makeThread(METHOD_PATCH, requestHandle, owner);
}

void HTTPRequest::Delete(Handle_t requestHandle, IdentityToken_t *owner) {
    this->makeThread(METHOD_DELETE, requestHandle, owner);
}

void HTTPRequest::Head(Handle_t requestHandle, IdentityToken_t *owner) {
    this->makeThread(METHOD_HEAD, requestHandle, owner);
}


void HTTPRequest::makeThread(HTTPRequestMethod method, Handle_t requestHandle, IdentityToken_t *owner) {
    HTTPRequestThread *requestThread = new HTTPRequestThread(this, requestHandle, owner, method);
    threader->MakeThread(requestThread);
}