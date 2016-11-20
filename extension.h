/**
 * -----------------------------------------------------
 * File        extension.h
 * Authors     Popoklopsi, Sourcemod
 * License     GPLv3
 * Web         http://popoklopsi.de
 * -----------------------------------------------------
 *
 * Copyright (C) 2013-2016 Popoklopsi, Sourcemod
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

#ifndef _EXTENSION_H_
#define _EXTENSION_H_

#include "smsdk_ext.h"

#include <stdio.h>
#include <string.h>
#include <fstream>

#include <queue>
#include <curl/curl.h>

#define MAX_RESULT_LENGTH 4096


enum OS {
	OS_UNKOWN,
	OS_WIN,
	OS_UNIX,
	OS_MAC
};

enum Mode {
	MODE_COMMAND,
	MODE_DOWNLOAD,
	MODE_UPLOAD,
	MODE_COPY,
	MODE_GET
};

enum ReturnState {
	CMD_SUCCESS,
	CMD_EMPTY,
	CMD_ERROR,
	CMD_PROGRESS
};


typedef struct {
	char resultString[MAX_RESULT_LENGTH];
	char curlError[CURL_ERROR_SIZE + 1];

	int finished;
	int data;

	double dltotal;
	double dlnow;
	double ultotal;
	double ulnow;

	Mode mode;

	IPluginFunction *function;
	cell_t result;
} ThreadReturn;


class System2Extension : public SDKExtension {
private:
	IMutex *mutex;
	std::queue<ThreadReturn *> forwardQueue;
	uint32_t frames;

public:
	void addToQueue(ThreadReturn *threadReturn);

	void GameFrameHit();
	uint32_t GetFrames() {
		return this->frames;
	}

	virtual bool SDK_OnLoad(char *error, size_t maxlength, bool late);
	virtual void SDK_OnUnload();
};


void OnGameFrameHit(bool simulating);

extern System2Extension system2Extension;

#endif