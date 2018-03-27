/**
 * -----------------------------------------------------
 * File        HTTPRequestThread.h
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

#ifndef _SYSTEM2_HTTP_REQUEST_THREAD_H_
#define _SYSTEM2_HTTP_REQUEST_THREAD_H_

#include "RequestThread.h"
#include "HTTPRequest.h"


class HTTPRequestThread : public RequestThread {
private:
    HTTPRequestMethod requestMethod;

public:
    HTTPRequest * httpRequest;

    typedef struct {
        CURL *curl;
        std::map<std::string, std::string> headers;
        long lastResponseCode;
    } HeaderInfo;

    HTTPRequestThread(HTTPRequest *httpRequest, HTTPRequestMethod requestMethod);

    virtual void RunThread(IThreadHandle *pThread);

    static size_t ReadHeader(char *buffer, size_t size, size_t nitems, void *userdata);
    static bool EqualsIgnoreCase(const std::string &str1, const std::string &str2);

private:
    static inline std::string& LeftTrim(std::string &str);
    static inline std::string& RightTrim(std::string &str);
    static inline std::string& Trim(std::string &str);
};


#endif