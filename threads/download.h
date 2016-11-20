/**
 * -----------------------------------------------------
 * File        download.h
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

#ifndef _DOWNLOAD_H_
#define _DOWNLOAD_H_

#include "extension.h"


typedef struct {
	int lastFrame;
	int data;
	IPluginFunction *function;
	Mode mode;
} progress_info;


class DownloadThread : public IThread {
private:
	char url[PLATFORM_MAX_PATH + 1];
	char localFile[PLATFORM_MAX_PATH + 1];

	IPluginFunction *function;
	int data;

public:
	DownloadThread(char *host, char *file, IPluginFunction *callback, int any);

	void RunThread(IThreadHandle *pThread);
	void OnTerminate(IThreadHandle *pThread, bool cancel) {}
};

size_t file_write(void *buffer, size_t size, size_t nmemb, void *stream);
int progress_updated(void *data, double dltotal, double dlnow, double ultotal, double ulnow);

#endif