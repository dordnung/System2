/**
 * -----------------------------------------------------
 * File        HTTPRequestThread.cpp
 * Authors     David Ordnung
 * License     GPLv3
 * Web         http://dordnung.de
 * -----------------------------------------------------
 *
 * Copyright (C) 2013-2020 David Ordnung
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

#include "HTTPRequestThread.h"
#include "HTTPResponseCallback.h"
#include "HTTPRequestMethod.h"

HTTPRequestThread::HTTPRequestThread(HTTPRequest* httpRequest, HTTPRequestMethod requestMethod)
    : RequestThread(httpRequest), requestMethod(requestMethod), httpRequest(httpRequest) {};

void HTTPRequestThread::Run() {
    // Create a curl object
    CURL* curl = curl_easy_init();

    if (curl) {
        // Apply general request stuff
        WriteDataInfo writeData = { std::string(), 0, nullptr };
        if (!this->ApplyRequest(curl, writeData)) {
            // Create error callback and clean up curl
            system2Extension.AppendCallback(std::make_shared<HTTPResponseCallback>(this->httpRequest, "Can not open output file", this->requestMethod));
            curl_easy_cleanup(curl);

            return;
        }

        // Collect error information
        char errorBuffer[CURL_ERROR_SIZE + 1];
        curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);

        // Use HTTP if no scheme is given
        curl_easy_setopt(curl, CURLOPT_DEFAULT_PROTOCOL, "http");

        // Set the http user agent
        if (!this->httpRequest->userAgent.empty()) {
            curl_easy_setopt(curl, CURLOPT_USERAGENT, this->httpRequest->userAgent.c_str());
        }

        // Set the http username
        if (!this->httpRequest->username.empty()) {
            curl_easy_setopt(curl, CURLOPT_USERNAME, this->httpRequest->username.c_str());
        }

        // Set the http password
        if (!this->httpRequest->password.empty()) {
            curl_easy_setopt(curl, CURLOPT_PASSWORD, this->httpRequest->password.c_str());
        }

        // Set the follow redirect property
        if (this->httpRequest->followRedirects) {
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        }

        // Set data to send
        if (!this->httpRequest->bodyData.empty()) {
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, this->httpRequest->bodyData.c_str());
        }

        // Set headers
        struct curl_slist* headers = nullptr;
        if (!this->httpRequest->headers.empty()) {
            std::string header;
            for (auto it = this->httpRequest->headers.begin(); it != this->httpRequest->headers.end(); ++it) {
                if (!it->first.empty()) {
                    header = it->first + ":";
                }
                header = header + it->second;
                headers = curl_slist_append(headers, header.c_str());

                // Also use accept encoding of CURL
                if (this->EqualsIgnoreCase(it->first, "Accept-Encoding")) {
                    curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, it->second.c_str());
                }
            }

            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        }

        // Get response headers
        HeaderInfo headerData = { curl, std::map<std::string, std::string>(), -1L };
        curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, HTTPRequestThread::ReadHeader);
        curl_easy_setopt(curl, CURLOPT_HEADERDATA, &headerData);

        // Set http method
        switch (this->requestMethod) {
            case METHOD_GET:
                curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
                break;
            case METHOD_POST:
                curl_easy_setopt(curl, CURLOPT_POST, 1L);
                if (this->httpRequest->bodyData.empty()) {
                    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "");
                    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, 0L);
                }

                break;
            case METHOD_PUT:
                curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
                break;
            case METHOD_PATCH:
                curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");
                break;
            case METHOD_DELETE:
                curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
                break;
            case METHOD_HEAD:
                curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
                break;
        }

        // Perform curl operation and create the callback
        std::shared_ptr<HTTPResponseCallback> callback;
        if (curl_easy_perform(curl) == CURLE_OK) {
            callback = std::make_shared<HTTPResponseCallback>(this->httpRequest, curl, writeData.content, writeData.contentLength, this->requestMethod, headerData.headers);
        } else {
            if (!strlen(errorBuffer)) {
                // Set readable error if there is no one
                callback = std::make_shared<HTTPResponseCallback>(this->httpRequest, "Couldn't execute HTTP request", this->requestMethod);
            } else {
                callback = std::make_shared<HTTPResponseCallback>(this->httpRequest, errorBuffer, this->requestMethod);
            }
        }

        // Clean up curl
        curl_easy_cleanup(curl);
        if (headers) {
            curl_slist_free_all(headers);
        }

        // Also close output file if opened
        if (writeData.file) {
            fclose(writeData.file);
        }

        // Append callback so it can be fired
        system2Extension.AppendCallback(callback);
    } else {
        system2Extension.AppendCallback(std::make_shared<HTTPResponseCallback>(this->httpRequest, "Couldn't initialize CURL", this->requestMethod));
    }
}

size_t HTTPRequestThread::ReadHeader(char* buffer, size_t size, size_t nitems, void* userdata) {
    // Get the header info
    HeaderInfo* headerInfo = (HeaderInfo*)userdata;

    long responseCode;
    curl_easy_getinfo(headerInfo->curl, CURLINFO_RESPONSE_CODE, &responseCode);

    // CURL will give not only the latest headers, so check if the response code changed
    if (headerInfo->lastResponseCode != responseCode) {
        headerInfo->lastResponseCode = responseCode;
        headerInfo->headers.clear();
    }

    size_t realsize = size * nitems;
    if (realsize > 0) {
        std::string name;
        std::string value;

        // Get the header as string
        std::string header = std::string(buffer, realsize);

        // Get the name and the value of the header
        size_t semi = header.find(':');
        if (semi == std::string::npos) {
            name = header;
        } else {
            name = header.substr(0, semi);
            value = header.substr(semi + 1);
        }

        Trim(name);
        Trim(value);

        // Only append if one of the two values is set
        if (name.length() > 0 || value.length() > 0) {
            headerInfo->headers[name] = value;
        }
    }

    return realsize;
}

bool HTTPRequestThread::EqualsIgnoreCase(const std::string& str1, const std::string& str2) {
    size_t str1Len = str1.size();
    if (str2.size() != str1Len) {
        return false;
    }

    for (size_t i = 0; i < str1Len; ++i) {
        if (tolower(str1[i]) != tolower(str2[i])) {
            return false;
        }
    }

    return true;
}

inline std::string& HTTPRequestThread::LeftTrim(std::string& str) {
    std::size_t found = str.find_first_not_of(" \t\f\v\n\r");
    if (found != std::string::npos) {
        str.erase(0, found);
    } else {
        str.clear();
    }

    return str;
}

inline std::string& HTTPRequestThread::RightTrim(std::string& str) {
    std::size_t found = str.find_last_not_of(" \t\f\v\n\r");
    if (found != std::string::npos) {
        str.erase(found + 1);
    } else {
        str.clear();
    }

    return str;
}

inline std::string& HTTPRequestThread::Trim(std::string& str) {
    return LeftTrim(RightTrim(str));
}