/**
 * -----------------------------------------------------
 * File        HTTPResponseCallback.cpp
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

#include "HTTPResponseCallback.h"


HTTPResponseCallback::HTTPResponseCallback(HTTPRequest *httpRequest, std::string error, HTTPRequestMethod requestMethod)
    : ResponseCallback(httpRequest, error), requestMethod(requestMethod) {}


HTTPResponseCallback::HTTPResponseCallback(HTTPRequest *httpRequest, CURL *curl, std::string content,
                                           HTTPRequestMethod requestMethod, std::map<std::string, std::string> headers)
    : ResponseCallback(httpRequest, curl, content), requestMethod(requestMethod), headers(headers) {}


void HTTPResponseCallback::PreFire() {
    // Push the request method for a HTTP request
    this->request->responseCallbackFunction->function->PushCell(this->requestMethod);
}