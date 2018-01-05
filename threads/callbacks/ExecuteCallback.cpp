/**
 * -----------------------------------------------------
 * File        ExecuteCallback.cpp
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

#include "ExecuteCallback.h"
#include "CommandOutputHandler.h"


ExecuteCallback::ExecuteCallback(bool success, std::string output, std::string command, int data, IPluginFunction *callback, IdentityToken_t *owner) {
    this->success = success;
    this->output = output;
    this->command = command;
    this->data = data;

    this->callback = callback;
    this->owner = owner;
}


std::string &ExecuteCallback::GetOutput() {
    return this->output;
}


void ExecuteCallback::Fire() {
    // Create the output handle
    Handle_t hndl = BAD_HANDLE;
    if (this->success) {
        hndl = handlesys->CreateHandle(commandOutputHandleType,
                                       this,
                                       this->owner,
                                       myself->GetIdentity(),
                                       NULL);
    }

    this->callback->PushCell(this->success);
    this->callback->PushCell(hndl);
    this->callback->PushString(this->command.c_str());
    this->callback->PushCell(this->data);
    this->callback->Execute(NULL);

    // Delete the output handle when finished
    if (hndl != BAD_HANDLE) {
        HandleSecurity sec = { this->owner, myself->GetIdentity() };
        handlesys->FreeHandle(hndl, &sec);
    }
}