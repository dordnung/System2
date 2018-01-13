/**
 * -----------------------------------------------------
 * File        RequestThread.h
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

#ifndef _SYSTEM2_REQUEST_THREAD_H_
#define _SYSTEM2_REQUEST_THREAD_H_

#include "extension.h"
#include "Request.h"
#include <map>


class RequestThread : public IThread {
private:
    static uint32_t lastProgressFrame;
    Request *request;

public:
    typedef struct {
        std::string content;
        FILE *file;
    } WriteDataInfo;

    RequestThread(Request *request);

    bool ApplyRequest(CURL *curl, WriteDataInfo &writeData);

    virtual void RunThread(IThreadHandle *pThread) = 0;
    virtual void OnTerminate(IThreadHandle *pThread, bool cancel);

    static size_t WriteData(char *ptr, size_t size, size_t nmemb, void *userdata);
    static size_t ReadFile(char *buffer, size_t size, size_t nitems, void *instream);
    static size_t ProgressUpdated(void *clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow);
};

#endif