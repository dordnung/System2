/**
 * -----------------------------------------------------
 * File        ftp.cpp
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

#include "ftp.h"
#include "download.h"


typedef struct {
	const char *filename;
	FILE *stream;
} ftp_info;


FTPThread::FTPThread(char *remoteFile, char *localFile, char *url, char *user, char *pw, int port, IPluginFunction *callback, Mode mode, int any) : IThread() {
	strcpy(this->remoteFile, remoteFile);
	strcpy(this->localFile, localFile);
	strcpy(this->host, url);
	strcpy(this->username, user);
	strcpy(this->password, pw);

	this->port = port;
	this->mode = mode;
	this->function = callback;
	this->data = any;
}

void FTPThread::RunThread(IThreadHandle *pHandle) {
	FILE *localReadFile = NULL;

	char fullLocalPath[PLATFORM_MAX_PATH + 1];
	char fullHost[PLATFORM_MAX_PATH + 20];

	// Create a thread return struct
	ThreadReturn *threadReturn = new ThreadReturn;

	threadReturn->function = function;
	threadReturn->mode = mode;
	threadReturn->finished = 1;
	threadReturn->data = data;

	strcpy(threadReturn->curlError, "");

	// Get the full path to the local file
	g_pSM->BuildPath(Path_Game, fullLocalPath, sizeof(fullLocalPath), "%s", localFile);

	ftp_info ftp_info =
	{
		fullLocalPath,
		NULL
	};

	progress_info progress =
	{
		0,
		data,
		function,
		mode
	};

	// Open the file if want to upload
	if (mode == MODE_UPLOAD) {
		localReadFile = fopen(fullLocalPath, "rb");
	}

	if (mode != MODE_UPLOAD || localReadFile != NULL) {
		// Init. Curl
		CURL *curl = curl_easy_init();

		if (curl) {
			// Get hole URL
			sprintf(fullHost, "ftp://%s/%s", host, remoteFile);

			// Set up curl
			curl_easy_setopt(curl, CURLOPT_URL, fullHost);
			curl_easy_setopt(curl, CURLOPT_PORT, port);
			curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, threadReturn->curlError);
			curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
			curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
			curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, progress_updated);
			curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, &progress);

			// Login?
			if (strlen(username) > 1) {
				char loginData[256];
				sprintf(loginData, "%s:%s", username, password);
				curl_easy_setopt(curl, CURLOPT_USERPWD, loginData);
			}

			// Upload stuff
			if (mode == MODE_UPLOAD) {
				// Get size
				fseek(localReadFile, 0L, SEEK_END);
				curl_off_t fsize = (curl_off_t)ftell(localReadFile);
				fseek(localReadFile, 0L, SEEK_SET);

				curl_easy_setopt(curl, CURLOPT_READFUNCTION, ftp_upload);
				curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
				curl_easy_setopt(curl, CURLOPT_FTP_CREATE_MISSING_DIRS, CURLFTP_CREATE_DIR_RETRY);
				curl_easy_setopt(curl, CURLOPT_READDATA, localReadFile);
				curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, fsize);

			}
			else {
				curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, file_write);
				curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ftp_info);
			}

			// Perform and clean
			curl_easy_perform(curl);
			curl_easy_cleanup(curl);
		}

		// Close file if opened
		if (ftp_info.stream != NULL) {
			fclose(ftp_info.stream);
		}

		// Close readed file
		if (localReadFile != NULL) {
			fclose(localReadFile);
		}
	}

	// Add return status to queue
	system2Extension.addToQueue(threadReturn);
}


size_t ftp_upload(void *buffer, size_t size, size_t nmemb, void *stream) {
	// Read file and return size
	return fread(buffer, size, nmemb, (FILE *)stream);
}