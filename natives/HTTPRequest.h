/**
 * -----------------------------------------------------
 * File        HTTPRequest.h
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

#ifndef _SYSTEM2_HTTP_REQUEST_H_
#define _SYSTEM2_HTTP_REQUEST_H_

#include "Request.h"
#include "HTTPRequestMethod.h"

#include <map>
#include <queue>

struct forms {
    uint8_t nameType;
    std::string nameContent;
    uint8_t dataType;
    std::string dataContent;
};

class HTTPRequest : public Request {
public:
    std::string bodyData;
    std::map<std::string, std::string> headers;
    std::string userAgent;
    std::string username;
    std::string password;
    bool followRedirects;
    std::queue<forms>form;

    HTTPRequest(std::string url, std::shared_ptr<CallbackFunction_t> responseCallbackFunction);
    HTTPRequest(const HTTPRequest &request);

    virtual HTTPRequest *Clone() const;

    bool Get();
    bool Post();
    bool Put();
    bool Patch();
    bool Delete();
    bool Head();

private:
    bool MakeThread(HTTPRequestMethod method);
};


#endif