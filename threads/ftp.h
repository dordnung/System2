/**
 * -----------------------------------------------------
 * File        ftp.h
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

#ifndef _FTP_H_
#define _FTP_H_

#include "extension.h"

 // Only allow one FTP connection at the same time, because of RFC does not allow multiple connections
extern IMutex *ftpMutex;

class FTPThread : public IThread {
private:
	char remoteFile[PLATFORM_MAX_PATH + 1];
	char localFile[PLATFORM_MAX_PATH + 1];
	char host[PLATFORM_MAX_PATH + 1];
	char username[128];
	char password[128];

	int port;
	Mode mode;
	int data;

	IPluginFunction *function;

public:
	FTPThread(char *remoteFile, char *localFile, char *url, char *user, char *pw, int port, IPluginFunction *callback, Mode mode, int any);

	void RunThread(IThreadHandle *pThread);
	void OnTerminate(IThreadHandle *pThread, bool cancel) {}
};

size_t ftp_upload(void *buffer, size_t size, size_t nmemb, void *stream);

#endif