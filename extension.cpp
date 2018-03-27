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

#include <algorithm>

#if defined _WIN32 || defined _WIN64
#define sleep_ms(x) Sleep(x);
#else
#define sleep_ms(x) usleep(x * 1000);
#endif


System2Extension::System2Extension() : threadMutex(NULL), frames(0), isRunning(false) {};

bool System2Extension::SDK_OnLoad(char *error, size_t err_max, bool late) {
    this->frames = 0;
    this->isRunning = true;

    // Add natives and register extension
    sharesys->AddNatives(myself, system2_natives);
    sharesys->AddNatives(myself, system2_legacy_natives);
    sharesys->RegisterLibrary(myself, "system2");

    // Creates needed mutexes
    threadMutex = threader->MakeMutex();
    ftpMutex = threader->MakeMutex();
    legacyFTPMutex = threader->MakeMutex();

    // Create handles
    executeCallbackHandler.Initialize();
    requestHandler.Initialize();
    responseCallbackHandler.Initialize();

    // Add game frame hook
    smutils->AddGameFrameHook(&OnGameFrameHit);

    // Add this plugin listener
    plsys->AddPluginsListener(this);

    // Init CURL
    curl_global_init(CURL_GLOBAL_ALL);

    return true;
}

void System2Extension::SDK_OnUnload() {
    this->threadMutex->Lock();

    // Mark that we are not running anymore
    this->isRunning = false;

    // Remove game frame hook so no callback will run anymore
    smutils->RemoveGameFrameHook(&OnGameFrameHit);

    this->threadMutex->Unlock();

    // Wait until all threads are finished
    if (runningThreads.size() > 0) {
        rootconsole->ConsolePrint("[System2] Please wait until %d thread(s) finished...", runningThreads.size());
        for (auto it = this->runningThreads.begin(); it != runningThreads.end(); ++it) {
            (*it)->WaitForThread();
        }
        rootconsole->ConsolePrint("[System2] All threads finished");
    }

    // Abort callbacks
    for (auto it = this->callbackQueue.begin(); it != callbackQueue.end(); ++it) {
        (*it)->Abort();
    }

    // Remove handles
    executeCallbackHandler.Shutdown();
    requestHandler.Shutdown();
    responseCallbackHandler.Shutdown();

    // Remove plugin listener
    plsys->RemovePluginsListener(this);

    // Clear STL stuff
    this->callbackQueue.clear();
    this->callbackFunctions.clear();
    this->runningThreads.clear();

    // Remove created mutexes
    this->threadMutex->DestroyThis();
    ftpMutex->DestroyThis();
    legacyFTPMutex->DestroyThis();

    // Finally clean up CURL
    curl_global_cleanup();
}


void System2Extension::OnPluginUnloaded(IPlugin *plugin) {
    // Search if the plugin has any pending callback functions and invalidate them
    for (auto it = this->callbackFunctions.begin(); it != callbackFunctions.end();) {
        if ((*it)->plugin == plugin) {
            // Mark it as invalid and remove it from the list
            (*it)->isValid = false;
            it = this->callbackFunctions.erase(it);
        } else {
            ++it;
        }
    }
}


void System2Extension::AppendCallback(std::shared_ptr<Callback> callback) {
    // Lock mutex to gain thread safety
    while (!this->threadMutex->TryLock()) {
        sleep_ms(1);
    }

    if (this->isRunning) {
        // Add the callback to the queue and unlock mutex again
        this->callbackQueue.push_back(callback);
    } else {
        // Abort the callback if we not running anymore
        callback->Abort();
    }

    this->threadMutex->Unlock();
}


bool System2Extension::RegisterAndStartThread(IThread *thread) {
    // Create the thread suspended, add it to the list and then start it
    IThreadHandle *handle = threader->MakeThread(thread, Thread_CreateSuspended);
    if (!handle) {
        return false;
    }

    this->threadMutex->Lock();
    this->runningThreads.push_back(handle);
    this->threadMutex->Unlock();

    handle->Unpause();
    return true;
}

void System2Extension::UnregisterAndDeleteThreadHandle(IThreadHandle *threadHandle) {
    while (!this->threadMutex->TryLock()) {
        sleep_ms(1);
    }

    // Just remove from the list of running threads
    if (this->isRunning) {
        this->runningThreads.erase(std::remove(this->runningThreads.begin(), this->runningThreads.end(), threadHandle), this->runningThreads.end());
    }

    // Destroy the thread handle to free resources
    threadHandle->DestroyThis();

    this->threadMutex->Unlock();
}


std::shared_ptr<CallbackFunction_t> System2Extension::CreateCallbackFunction(IPluginFunction *function) {
    if (!function || !function->IsRunnable()) {
        // Function is not valid
        return NULL;
    }

    auto plugin = plsys->FindPluginByContext(function->GetParentRuntime()->GetDefaultContext()->GetContext());
    if (!plugin) {
        // Plugin is not valid
        return NULL;
    }

    // Check if we already have the callback function
    for (auto it = this->callbackFunctions.begin(); it != callbackFunctions.end(); ++it) {
        if ((*it)->function == function) {
            auto callbackFunction = (*it);
            callbackFunction->plugin = plugin;
            callbackFunction->isValid = true;

            // Reuse the callback function
            return callbackFunction;
        }
    }

    auto callbackFunction = std::make_shared<CallbackFunction_t>();
    callbackFunction->plugin = plugin;
    callbackFunction->function = function;
    callbackFunction->isValid = true;

    // Add to the internal list of callback functions
    this->callbackFunctions.push_back(callbackFunction);
    return callbackFunction;
}


void System2Extension::GameFrameHit() {
    // Increase number of frames
    this->frames++;

    // Lock the mutex to gain thread safety
    if (!this->threadMutex->TryLock()) {
        // Couldn't lock -> do not wait
        return;
    }

    // Are there outstandig callbacks?
    std::shared_ptr<Callback> callback = NULL;
    if (this->isRunning && !this->callbackQueue.empty()) {
        callback = this->callbackQueue.front();

        // Remove the callback from the queue
        // No deleting needed, as callbacks are shared pointers
        callbackQueue.pop_front();
    }

    // Unlock mutex
    this->threadMutex->Unlock();

    // Proccess callback outside mutex lock to avoid infinite loop
    if (callback) {
        if (callback->callbackFunction->isValid && callback->callbackFunction->function->IsRunnable()) {
            // Fire the callback if the callback function is valid
            callback->Fire();
        } else {
            callback->Abort();
        }
    }
}

uint32_t System2Extension::GetFrames() {
    return this->frames;
}

void OnGameFrameHit(bool simulating) {
    system2Extension.GameFrameHit();
}


// Create and link the extension
System2Extension system2Extension;
SMEXT_LINK(&system2Extension);