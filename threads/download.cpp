/**
 * -----------------------------------------------------
 * File        download.cpp
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

#include "download.h"


typedef struct {
	const char *filename;
	FILE *stream;
} file_download_info;


DownloadThread::DownloadThread(char *host, char *file, IPluginFunction *callback, int any) : IThread() {
	strcpy(this->url, host);
	strcpy(this->localFile, file);

	this->function = callback;
	this->data = any;
}

void DownloadThread::RunThread(IThreadHandle *pHandle) {
	char fullLocalPath[PLATFORM_MAX_PATH + 1];

	// Create a thread return struct
	ThreadReturn *threadReturn = new ThreadReturn;

	threadReturn->function = function;
	threadReturn->mode = MODE_DOWNLOAD;
	threadReturn->finished = CMD_EMPTY;
	threadReturn->data = data;

	strcpy(threadReturn->curlError, "");

	// Get full path to the local file
	g_pSM->BuildPath(Path_Game, fullLocalPath, sizeof(fullLocalPath), "%s", localFile);

	file_download_info download_info = {
		fullLocalPath,
		NULL
	};

	// Get progress info
	progress_info progress =
	{
		0,
		data,
		function,
		MODE_DOWNLOAD,
	};

	// Init. Curl
	CURL *curl = curl_easy_init();

	if (curl) {
		// Set up Curl
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, threadReturn->curlError);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, file_write);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &download_info);
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
		curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, progress_updated);
		curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, &progress);

		// Perform and clean
		curl_easy_perform(curl);
		curl_easy_cleanup(curl);
	}


	// Close file if opened
	if (download_info.stream != NULL) {
		fclose(download_info.stream);
	}

	// Add return status to queue
	system2Extension.addToQueue(threadReturn);
}


size_t file_write(void *buffer, size_t size, size_t nmemb, void *stream) {
	file_download_info *download_info = (file_download_info *)stream;

	// Is file not open -> open it
	if (download_info && !download_info->stream) {
		download_info->stream = fopen(download_info->filename, "wb");

		// Could we open the file?
		if (!download_info->stream) {
			return -1;
		}
	}

	// Write to the file
	return fwrite(buffer, size, nmemb, download_info->stream);
}


int progress_updated(void *data, double dltotal, double dlnow, double ultotal, double ulnow) {
	progress_info *progress = (progress_info *)data;

	if ((dlnow > 0 || dltotal > 0 || ultotal > 0 || ulnow > 0) && (system2Extension.GetFrames() != progress->lastFrame)) {
		// Create a thread return struct
		ThreadReturn *threadReturn = new ThreadReturn;

		// Save to func
		threadReturn->function = progress->function;
		threadReturn->mode = progress->mode;
		threadReturn->finished = 0;
		threadReturn->data = progress->data;

		// Update data
		threadReturn->dlnow = dlnow;
		threadReturn->dltotal = dltotal;
		threadReturn->ultotal = ultotal;
		threadReturn->ulnow = ulnow;

		// Add return status to queue
		system2Extension.addToQueue(threadReturn);
	}

	// Save current frame
	progress->lastFrame = system2Extension.GetFrames();

	return 0;
}