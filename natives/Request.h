/**
 * -----------------------------------------------------
 * File        Request.h
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

#ifndef _SYSTEM2_REQUEST_H_
#define _SYSTEM2_REQUEST_H_

#include "extension.h"
#include "RequestHandler.h"


class Request {
public:
    std::string url;
    int port;
    std::string outputFile;
    bool verifySSL;
    int timeout;
    int data;

    std::shared_ptr<CallbackFunction_t> responseCallbackFunction;
    std::shared_ptr<CallbackFunction_t> progressCallbackFunction;

    Request(std::string url, std::shared_ptr<CallbackFunction_t> responseCallbackFunction);
    Request(const Request &request);
    virtual ~Request() = 0;

    virtual Request *Clone() const = 0;

    template<class RequestClass>
    static RequestClass *ConvertRequest(Handle_t hndl, IPluginContext *pContext) {
        HandleError err;

        RequestClass *request = NULL;
        if ((err = requestHandler.ReadHandle<RequestClass>(hndl, pContext->GetIdentity(), &request)) != HandleError_None) {
            pContext->ThrowNativeError("Invalid request handle %x (error %d)", hndl, err);
            return NULL;
        }

        return request;
    }
};


#endif