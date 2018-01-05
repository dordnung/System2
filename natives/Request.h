/**
 * -----------------------------------------------------
 * File        Request.h
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

#ifndef _SYSTEM2_REQUEST_H_
#define _SYSTEM2_REQUEST_H_

#include "extension.h"


class Request {
public:
    std::string url;
    int port;
    bool autoClean;
    int timeout;
    int any;

    IPluginFunction *responseCallback;
    IPluginFunction *progressCallback;

    Request(std::string url, IPluginFunction *responseCallback);
    virtual ~Request() = 0;

    template<class RequestClass>
    static RequestClass *convertRequest(Handle_t hndl, IPluginContext *pContext) {
        HandleError err;
        HandleSecurity sec = { pContext->GetIdentity(), myself->GetIdentity() };

        RequestClass *request;
        if ((err = handlesys->ReadHandle(hndl, requestHandleType, &sec, (void **)&request)) != HandleError_None) {
            pContext->ReportError("Invalid request handle %x (error %d)", hndl, err);
            return NULL;
        }

        return request;
    }
};


#endif