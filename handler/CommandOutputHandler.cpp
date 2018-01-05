/**
* -----------------------------------------------------
* File        CommandOutputHandler.cpp
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

#include "CommandOutputHandler.h"


HandleType_t commandOutputHandleType = 0;
CommandOutputHandler commandOutputHandler;

void CommandOutputHandler::Initialize() {
    HandleAccess rules;
    handlesys->InitAccessDefaults(NULL, &rules);

    // Do not allowe deleting of the handle, as this will always deleted after the callback
    rules.access[HandleAccess_Delete] = HANDLE_RESTRICT_IDENTITY;

    commandOutputHandleType =
        handlesys->CreateType("System2CommandOutput",
                              this,
                              0,
                              NULL,
                              &rules,
                              myself->GetIdentity(),
                              NULL);
}

void CommandOutputHandler::Shutdown() {
    handlesys->RemoveType(commandOutputHandleType, myself->GetIdentity());
}

void CommandOutputHandler::OnHandleDestroy(HandleType_t type, void *object) {
    // Nothing to do, as handle is a shared pointer and will be deleted automatically
}