/**
* -----------------------------------------------------
* File        ResponseCallbackHandler.cpp
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

#include "ResponseCallbackHandler.h"


ResponseCallbackHandler::ResponseCallbackHandler() : handleType(0) {}

void ResponseCallbackHandler::Initialize() {
    HandleAccess rules;
    handlesys->InitAccessDefaults(nullptr, &rules);

    // Do not allowe deleting of the handle, as this will always deleted after the callback
    rules.access[HandleAccess_Delete] = HANDLE_RESTRICT_OWNER | HANDLE_RESTRICT_IDENTITY;
    rules.access[HandleAccess_Clone] = HANDLE_RESTRICT_OWNER | HANDLE_RESTRICT_IDENTITY;

    this->handleType =
        handlesys->CreateType("System2Response",
                              this,
                              0,
                              nullptr,
                              &rules,
                              myself->GetIdentity(),
                              nullptr);
}

void ResponseCallbackHandler::Shutdown() {
    handlesys->RemoveType(this->handleType, myself->GetIdentity());
}

void ResponseCallbackHandler::OnHandleDestroy(HandleType_t type, void *object) {
    // Nothing to do, as handle is a callback and will be deleted otherwise
}


// Create an instance of the response callback handler
ResponseCallbackHandler responseCallbackHandler;