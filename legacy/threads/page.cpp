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

typedef struct {
    std::string result;
    int data;
    IPluginFunction *callback;
} page_info;


LegacyPageThread::LegacyPageThread(std::string url, std::string post, std::string useragent, int data, IPluginFunction *callback) : IThread() {
    this->url = url;
    this->post = post;
    this->useragent = useragent;

    this->data = data;
    this->callback = callback;
}

void LegacyPageThread::RunThread(IThreadHandle *pHandle) {
    CommandState state = CMD_SUCCESS;

    // Create a page info for the write function
    page_info page =
    {
        std::string(),
        this->data,
        this->callback,
    };

    CURL *curl = curl_easy_init();
    if (curl) {
        char errorBuffer[CURL_ERROR_SIZE + 1];

        // Set up Curl
        curl_easy_setopt(curl, CURLOPT_URL, this->url.c_str());
        curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
        curl_easy_setopt(curl, CURLOPT_POSTREDIR, CURL_REDIR_POST_ALL);
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, page_get);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &page);
        curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
        curl_easy_setopt(curl, CURLOPT_BUFFERSIZE, MAX_RESULT_LENGTH);

        if (!this->post.empty()) {
            curl_easy_setopt(curl, CURLOPT_POST, 1L);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, this->post.c_str());
        }

        // Set the useragent
        if (!this->useragent.empty()) {
            curl_easy_setopt(curl, CURLOPT_USERAGENT, useragent.c_str());
        }

        // Perform
        if (curl_easy_perform(curl) != CURLE_OK) {
            page.result = errorBuffer;
            state = CMD_ERROR;
        }

        // Clean
        curl_easy_cleanup(curl);
    } else {
        // Error
        page.result = "ERROR: Couldn't init CURL!";
        state = CMD_ERROR;
    }

    // Add return status to queue
    system2Extension.AppendCallback(std::make_shared<LegacyCommandCallback>(page.result, std::string(), this->data, this->callback, state));
}


// Got something of the page
size_t page_get(void *buffer, size_t size, size_t nmemb, void *userdata) {
    size_t realsize = size * nmemb;

    // Get the page info
    page_info *page = (page_info *)userdata;

    // We only can push a string with a length of MAX_RESULT_LENGTH
    if (page->result.length() + realsize >= MAX_RESULT_LENGTH) {
        // Add return status to queue
        system2Extension.AppendCallback(std::make_shared<LegacyCommandCallback>(page->result, std::string(), page->data, page->callback, CMD_PROGRESS));

        // Clear result buffer
        page->result.clear();
    }

    // Add to result
    page->result.append((char *)buffer, realsize);

    return realsize;
}
