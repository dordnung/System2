/**
* -----------------------------------------------------
* File        RequestHandler.h
* Authors     David Ordnung
* License     GPLv3
* Web         http://dordnung.de
* -----------------------------------------------------
*
* Copyright (C) 2013-2020 David Ordnung
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

#ifndef _SYSTEM2_REQUEST_HANDLER_H_
#define _SYSTEM2_REQUEST_HANDLER_H_

#include "Handler.h"


class RequestHandler : public Handler {
private:
    HandleType_t handleType;

public:
    RequestHandler();

    virtual void Initialize();
    virtual void Shutdown();

    template<class RequestClass>
    Handle_t CreateGlobalHandle(RequestClass* request, IdentityToken_t* owner) {
        return handlesys->CreateHandle(this->handleType,
                                       request,
                                       owner,
                                       myself->GetIdentity(),
                                       nullptr);
    }

    template<class RequestClass>
    Handle_t CreateLocaleHandle(RequestClass* request, IdentityToken_t* owner) {
        // Do not allow deleting or cloning for the plugin
        HandleAccess rules;
        g_pHandleSys->InitAccessDefaults(nullptr, &rules);
        rules.access[HandleAccess_Delete] = HANDLE_RESTRICT_OWNER | HANDLE_RESTRICT_IDENTITY;
        rules.access[HandleAccess_Clone] = HANDLE_RESTRICT_OWNER | HANDLE_RESTRICT_IDENTITY;

        HandleSecurity sec = { owner, myself->GetIdentity() };
        return handlesys->CreateHandleEx(this->handleType,
                                         request,
                                         &sec,
                                         &rules,
                                         nullptr);
    }

    template<class RequestClass>
    HandleError ReadHandle(Handle_t hndl, IdentityToken_t* owner, RequestClass** request) {
        HandleSecurity sec = { owner, myself->GetIdentity() };
        return handlesys->ReadHandle(hndl, this->handleType, &sec, (void**)request);
    }

    virtual void OnHandleDestroy(HandleType_t type, void* object);
};

extern RequestHandler requestHandler;

#endif