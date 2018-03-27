/**
 * -----------------------------------------------------
 * File        ExecuteCallback.cpp
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

#include "ExecuteCallback.h"
#include "ExecuteCallbackHandler.h"


ExecuteCallback::ExecuteCallback(std::shared_ptr<CallbackFunction_t> callbackFunction, bool success, int exitStatus, std::string output, std::string command, int data)
    : Callback(callbackFunction), success(success), exitStatus(exitStatus), output(output), command(command), data(data) {}


const std::string &ExecuteCallback::GetOutput() const {
    return this->output;
}

int ExecuteCallback::GetExitStatus() const {
    return this->exitStatus;
}

void ExecuteCallback::Fire() {
    IdentityToken_t *owner = this->callbackFunction->plugin->GetIdentity();
    Handle_t outputHandle = BAD_HANDLE;

    if (this->success) {
        // Create the output handle
        outputHandle = executeCallbackHandler.CreateHandle(this, owner);
    }

    // Push every argument to the callback and execute it
    this->callbackFunction->function->PushCell(this->success);
    this->callbackFunction->function->PushString(this->command.c_str());
    this->callbackFunction->function->PushCell(outputHandle);
    this->callbackFunction->function->PushCell(this->data);
    this->callbackFunction->function->Execute(NULL);

    // Delete the output handle when finished
    if (outputHandle != BAD_HANDLE) {
        executeCallbackHandler.FreeHandle(outputHandle, owner);
    }
}

ExecuteCallback *ExecuteCallback::ConvertExecuteCallback(Handle_t hndl, IPluginContext *pContext) {
    HandleError err;

    ExecuteCallback *executeCallback = NULL;
    if ((err = executeCallbackHandler.ReadHandle(hndl, pContext->GetIdentity(), &executeCallback)) != HandleError_None) {
        pContext->ThrowNativeError("Invalid execute output handle %x (error %d)", hndl, err);
        return NULL;
    }

    return executeCallback;
}