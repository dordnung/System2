/**
 * -----------------------------------------------------
 * File        ResponseNatives.cpp
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

#include "ResponseCallback.h"
#include "HTTPResponseCallback.h"
#include "HTTPRequestThread.h"

cell_t NativeResponse_GetLastURL(IPluginContext* pContext, const cell_t* params) {
    ResponseCallback* response = ResponseCallback::ConvertResponse<ResponseCallback>(params[1], pContext);
    if (!response) {
        return 0;
    }

    pContext->StringToLocalUTF8(params[2], params[3], response->lastURL.c_str(), nullptr);
    return 1;
}

cell_t NativeResponse_GetContent(IPluginContext* pContext, const cell_t* params) {
    ResponseCallback* response = ResponseCallback::ConvertResponse<ResponseCallback>(params[1], pContext);
    if (!response) {
        return 0;
    }

    // Get offset and check range
    std::string output = response->content;

    int offset = params[4];
    int length = static_cast<int>(output.length());
    if (offset < 0) {
        offset = 0;
    }

    if (offset > length) {
        offset = length;
    }

    char* delimiter;
    pContext->LocalToString(params[5], &delimiter);

    if (strlen(delimiter) > 0) {
        // Find the delimiter
        size_t delimiterPos = output.find(delimiter, offset);
        if (delimiterPos != std::string::npos) {
            bool includeDelimiter = params[6];
            if (includeDelimiter) {
                // Include the delimiter in the response
                delimiterPos += strlen(delimiter);
            }

            output = output.substr(offset, delimiterPos - offset);
        } else {
            output = output.substr(offset);
        }
    } else {
        output = output.substr(offset);
    }

    size_t bytes;
    pContext->StringToLocalUTF8(params[2], params[3], output.c_str(), &bytes);

    return bytes;
}

cell_t NativeResponse_GetContentLength(IPluginContext* pContext, const cell_t* params) {
    ResponseCallback* response = ResponseCallback::ConvertResponse<ResponseCallback>(params[1], pContext);
    if (!response) {
        return 0;
    }

    return response->content.length();
}

cell_t NativeResponse_GetStatusCode(IPluginContext* pContext, const cell_t* params) {
    ResponseCallback* response = ResponseCallback::ConvertResponse<ResponseCallback>(params[1], pContext);
    if (!response) {
        return 0;
    }

    return response->statusCode;
}

cell_t NativeResponse_GetTotalTime(IPluginContext* pContext, const cell_t* params) {
    ResponseCallback* response = ResponseCallback::ConvertResponse<ResponseCallback>(params[1], pContext);
    if (!response) {
        return 0;
    }

    return sp_ftoc(response->totalTime);
}

cell_t NativeResponse_GetDownloadSize(IPluginContext* pContext, const cell_t* params) {
    ResponseCallback* response = ResponseCallback::ConvertResponse<ResponseCallback>(params[1], pContext);
    if (!response) {
        return 0;
    }

    return response->downloadSize;
}

cell_t NativeResponse_GetUploadSize(IPluginContext* pContext, const cell_t* params) {
    ResponseCallback* response = ResponseCallback::ConvertResponse<ResponseCallback>(params[1], pContext);
    if (!response) {
        return 0;
    }

    return response->uploadSize;
}

cell_t NativeResponse_GetDownloadSpeed(IPluginContext* pContext, const cell_t* params) {
    ResponseCallback* response = ResponseCallback::ConvertResponse<ResponseCallback>(params[1], pContext);
    if (!response) {
        return 0;
    }

    return response->downloadSpeed;
}

cell_t NativeResponse_GetUploadSpeed(IPluginContext* pContext, const cell_t* params) {
    ResponseCallback* response = ResponseCallback::ConvertResponse<ResponseCallback>(params[1], pContext);
    if (!response) {
        return 0;
    }

    return response->uploadSpeed;
}

cell_t NativeHTTPResponse_GetContentType(IPluginContext* pContext, const cell_t* params) {
    HTTPResponseCallback* response = ResponseCallback::ConvertResponse<HTTPResponseCallback>(params[1], pContext);
    if (!response) {
        return 0;
    }

    pContext->StringToLocalUTF8(params[2], params[3], response->contentType.c_str(), nullptr);
    return 1;
}

cell_t NativeHTTPResponse_GetHeader(IPluginContext* pContext, const cell_t* params) {
    HTTPResponseCallback* response = ResponseCallback::ConvertResponse<HTTPResponseCallback>(params[1], pContext);
    if (!response) {
        return 0;
    }

    char* header;
    pContext->LocalToString(params[2], &header);

    for (auto it = response->headers.begin(); it != response->headers.end(); ++it) {
        if (HTTPRequestThread::EqualsIgnoreCase(it->first, header)) {
            pContext->StringToLocalUTF8(params[3], params[4], response->headers[header].c_str(), nullptr);
            return 1;
        }
    }

    return 0;
}

cell_t NativeHTTPResponse_GetHeaderName(IPluginContext* pContext, const cell_t* params) {
    HTTPResponseCallback* response = ResponseCallback::ConvertResponse<HTTPResponseCallback>(params[1], pContext);
    if (!response) {
        return 0;
    }

    if (params[2] >= static_cast<int>(response->headers.size())) {
        return 0;
    }

    // Map can't be accessed by index
    auto it = response->headers.begin();
    for (int i = 0; i < params[2]; i++) {
        ++it;
    }

    pContext->StringToLocalUTF8(params[3], params[4], it->first.c_str(), nullptr);
    return 1;
}

cell_t NativeHTTPResponse_GetHeaders(IPluginContext* pContext, const cell_t* params) {
    HTTPResponseCallback* response = ResponseCallback::ConvertResponse<HTTPResponseCallback>(params[1], pContext);
    if (!response) {
        return 0;
    }

    return response->headers.size();
}

cell_t NativeHTTPResponse_GetHTTPVersion(IPluginContext* pContext, const cell_t* params) {
    HTTPResponseCallback* response = ResponseCallback::ConvertResponse<HTTPResponseCallback>(params[1], pContext);
    if (!response) {
        return 0;
    }

    return response->httpVersion;
}