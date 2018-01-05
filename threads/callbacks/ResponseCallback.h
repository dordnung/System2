/**
* -----------------------------------------------------
* File        ResponseCallback.h
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

#ifndef _SYSTEM2_RESPONSE_CALLBACK_H_
#define _SYSTEM2_RESPONSE_CALLBACK_H_

#include "extension.h"
#include "Request.h"
#include "Callback.h"


class ResponseCallback : public Callback {
public:
    bool success;
    std::string error;
    Request *request;
    Handle_t requestHandle;
    std::string content;
    std::string lastURL;
    int statusCode;
    float totalTime;

    IdentityToken_t *owner;

    ResponseCallback(std::string error, Request *request, Handle_t requestHandle, IdentityToken_t *owner);
    ResponseCallback(CURL *curl, std::string content, Request *request, Handle_t requestHandle, IdentityToken_t *owner);

    virtual void Fire();

    template<class ResponseCallbackClass>
    static ResponseCallbackClass *convertResponse(Handle_t hndl, IPluginContext *pContext) {
        HandleError err;
        HandleSecurity sec = { pContext->GetIdentity(), myself->GetIdentity() };

        ResponseCallbackClass *response;
        if ((err = handlesys->ReadHandle(hndl, responseHandleType, &sec, (void **)&response)) != HandleError_None) {
            pContext->ReportError("Invalid response handle %x (error %d)", hndl, err);
            return NULL;
        }

        return response;
    }
};

#endif