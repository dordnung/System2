/**
* -----------------------------------------------------
* File        ResponseCallback.h
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

#ifndef _SYSTEM2_RESPONSE_CALLBACK_H_
#define _SYSTEM2_RESPONSE_CALLBACK_H_

#include "Callback.h"
#include "Request.h"
#include "ResponseCallbackHandler.h"


class ResponseCallback : public Callback {
protected:
    Request * request;

public:
    std::string error;
    std::string content;
    std::string lastURL;
    int statusCode;
    float totalTime;

    ResponseCallback(Request *request, std::string error);
    ResponseCallback(Request *request, CURL *curl, std::string content);

    template<class ResponseCallbackClass>
    static ResponseCallbackClass *ConvertResponse(Handle_t hndl, IPluginContext *pContext) {
        HandleError err;

        ResponseCallbackClass *response;
        if ((err = responseCallbackHandler.ReadHandle<ResponseCallbackClass>(hndl, pContext->GetIdentity(), &response)) != HandleError_None) {
            pContext->ReportError("Invalid response handle %x (error %d)", hndl, err);
            return NULL;
        }

        return response;
    }

protected:
    virtual void Fire();
    virtual void PreFire() = 0;
};

#endif