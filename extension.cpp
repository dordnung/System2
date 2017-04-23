/**
 * -----------------------------------------------------
 * File        extension.cpp
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

#include "extension.h"
#include "natives.h"


bool System2Extension::SDK_OnLoad(char *error, size_t err_max, bool late) {
	this->frames = 0;

	// Add natives and register extension
	sharesys->AddNatives(myself, system2_natives);
	sharesys->RegisterLibrary(myself, "system2");

	// Add mutex and init curl
	mutex = threader->MakeMutex();
	curl_global_init(CURL_GLOBAL_ALL);

	smutils->AddGameFrameHook(&OnGameFrameHit);

	return true;
}

void System2Extension::SDK_OnUnload() {
	smutils->RemoveGameFrameHook(&OnGameFrameHit);
	mutex->DestroyThis();

	curl_global_cleanup();
}

void System2Extension::addToQueue(ThreadReturn *threadReturn) {
	// Lock mutex to gain thread safety
	while (!this->mutex->TryLock()) {
#ifdef _WIN32
		Sleep(50);
#else
		usleep(50000);
#endif
	}

	// Add the thread return to the front of the queue
	this->forwardQueue.push(threadReturn);
	this->mutex->Unlock();
}

void System2Extension::GameFrameHit() {
	// Increase frame number which will be needed for progress update
	this->frames++;

	// Lock the mutex to gain thread safety
	if (!this->mutex->TryLock()) {
		// Couldn't lock
		return;
	}

	// Are there outstandig forwards?
	if (!this->forwardQueue.empty()) {
		// Get the last forward if so
		ThreadReturn *threadReturn = this->forwardQueue.front();
		IPluginFunction *function = threadReturn->function;

		// Set function params
		if (threadReturn->mode == MODE_COMMAND || threadReturn->mode == MODE_GET) {
			// ... for a command callback
			function->PushString(threadReturn->resultString);
			function->PushCell(strlen(threadReturn->resultString) + 1);
			function->PushCell(threadReturn->result);
			function->PushCell(threadReturn->data);
			function->PushString(threadReturn->command);
		}
		else if (threadReturn->mode != MODE_COPY) {
			// ... for a progress callback
			function->PushCell(threadReturn->finished);
			function->PushString(threadReturn->curlError);

			function->PushFloat((float)threadReturn->dltotal);
			function->PushFloat((float)threadReturn->dlnow);
			function->PushFloat((float)threadReturn->ultotal);
			function->PushFloat((float)threadReturn->ulnow);
			function->PushCell(threadReturn->data);
		}
		else {
			// ... for a result for a copy
			function->PushCell((threadReturn->result == CMD_ERROR) ? 0 : 1);
			function->PushString(threadReturn->copyFrom);
			function->PushString(threadReturn->copyTo);
			function->PushCell(threadReturn->data);
		}

		// Finally execute the forward
		function->Execute(NULL);

		// Delete it and remove it from the queue
		delete threadReturn;
		forwardQueue.pop();
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
