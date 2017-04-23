/**
 * -----------------------------------------------------
 * File        page.cpp
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

#include "page.h"


PageThread::PageThread(char *link, char *postmethod, char *agent, IPluginFunction *callback, int any) : IThread() {
	strcpy(this->url, link);
	strcpy(this->post, postmethod);
	strcpy(this->useragent, agent);

	this->function = callback;
	this->data = any;
}

void PageThread::RunThread(IThreadHandle *pHandle) {
	// Create a thread return struct
	ThreadReturn *threadReturn = new ThreadReturn;

	threadReturn->function = function;
	threadReturn->mode = MODE_GET;
	threadReturn->result = CMD_SUCCESS;
	threadReturn->data = data;

	strcpy(threadReturn->command, "");
	strcpy(threadReturn->curlError, "");
	strcpy(threadReturn->resultString, "");

	// Init. Curl
	CURL *curl = curl_easy_init();

	if (curl) {
		// Set up Curl
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, threadReturn->curlError);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, page_get);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, threadReturn);
		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
		curl_easy_setopt(curl, CURLOPT_BUFFERSIZE, MAX_RESULT_LENGTH - 1);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post);

		// Set the useragent
		if (strcmp(useragent, "") != 0) {
			curl_easy_setopt(curl, CURLOPT_USERAGENT, useragent);
		}

		// Perform
		if (curl_easy_perform(curl) != CURLE_OK) {
			strcpy(threadReturn->resultString, threadReturn->curlError);
			threadReturn->result = CMD_ERROR;
		}

		// Clean
		curl_easy_cleanup(curl);
	}

	// Add return status to queue
	system2Extension.addToQueue(threadReturn);
}


// Got something of the page
size_t page_get(void *buffer, size_t size, size_t nmemb, void *stream) {
	// Buffer
	ThreadReturn *threadReturn = (ThreadReturn *)stream;

	// real size
	size_t realsize = size * nmemb;

	// More than MAX_RESULT_LENGTH?
	if (strlen(threadReturn->resultString) + realsize >= MAX_RESULT_LENGTH - 1) {
		// We only can push a string with a length of MAX_RESULT_LENGTH
		ThreadReturn *threadReturn2 = new ThreadReturn;

		threadReturn2->function = threadReturn->function;
		threadReturn2->data = threadReturn->data;
		threadReturn2->mode = MODE_GET;
		threadReturn2->result = CMD_PROGRESS;

		strcpy(threadReturn2->command, "");
		strcpy(threadReturn2->resultString, threadReturn->resultString);

		// Add return status to queue
		system2Extension.addToQueue(threadReturn2);

		// Empty buffer
		strcpy(threadReturn->resultString, "");
	}

	// Add to buffer
	strncat(threadReturn->resultString, (char *)buffer, realsize);
	return realsize;
}
