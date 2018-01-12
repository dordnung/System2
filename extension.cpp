/**
 * -----------------------------------------------------
 * File        extension.cpp
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

#include "extension.h"
#include "Natives.h"
#include "ExecuteCallbackHandler.h"
#include "RequestHandler.h"
#include "ResponseCallbackHandler.h"
#include "LegacyNatives.h"
#include "LegacyFTPThread.h"
#include "FTPRequestThread.h"


bool System2Extension::SDK_OnLoad(char *error, size_t err_max, bool late) {
    this->frames = 0;

    // Add natives and register extension
    sharesys->AddNatives(myself, system2_natives);
    sharesys->AddNatives(myself, system2_legacy_natives);
    sharesys->RegisterLibrary(myself, "system2");

    // Create needed mutex
    mutex = threader->MakeMutex();
    ftpMutex = threader->MakeMutex();
    legacyFTPMutex = threader->MakeMutex();

    // Create handles
    executeCallbackHandler.Initialize();
    requestHandler.Initialize();
    responseCallbackHandler.Initialize();

    smutils->AddGameFrameHook(&OnGameFrameHit);

    // CURL needs to be initialized
    curl_global_init(CURL_GLOBAL_ALL);

    return true;
}

void System2Extension::SDK_OnUnload() {
    // Remove created mutex
    mutex->DestroyThis();
    ftpMutex->DestroyThis();
    legacyFTPMutex->DestroyThis();

    // Remove handles
    executeCallbackHandler.Shutdown();
    requestHandler.Shutdown();
    responseCallbackHandler.Shutdown();

    smutils->RemoveGameFrameHook(&OnGameFrameHit);

    curl_global_cleanup();
}

void System2Extension::AppendCallback(std::shared_ptr<Callback> callback) {
    // Lock mutex to gain thread safety
    while (!this->mutex->TryLock()) {
#ifdef _WIN32
        Sleep(50);
#else
        usleep(50000);
#endif
    }

    // Add the callback to the queue and unlock mutex again
    this->callbackQueue.push(callback);
    this->mutex->Unlock();
}

void System2Extension::GameFrameHit() {
    // Increase number of frames
    this->frames++;

    // Lock the mutex to gain thread safety
    if (!this->mutex->TryLock()) {
        // Couldn't lock -> do not wait
        return;
    }

    // Are there outstandig callbacks?
    if (!this->callbackQueue.empty()) {
        // Fire the next callback
        this->callbackQueue.front()->Fire();

        // Remove the callback from the queue
        callbackQueue.pop();
    }

    // Unlock mutex
    this->mutex->Unlock();
}


void OnGameFrameHit(bool simulating) {
    system2Extension.GameFrameHit();
}


// Create and link the extension
System2Extension system2Extension;
SMEXT_LINK(&system2Extension);