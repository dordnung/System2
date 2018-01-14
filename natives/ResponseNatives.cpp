/**
 * -----------------------------------------------------
 * File        ResponseNatives.cpp
 * Authors     David Ordnung
 * License     GPLv3
 * Web         http://dordnung.de
 * -----------------------------------------------------
 *
 * Copyright (C) 2013-2018 David Ordnung
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


cell_t NativeResponse_GetLastUrl(IPluginContext *pContext, const cell_t *params) {
    ResponseCallback *response = ResponseCallback::ConvertResponse<ResponseCallback>(params[1], pContext);
    if (response == NULL) {
        return 0;
    }

    pContext->StringToLocalUTF8(params[2], params[3], response->lastURL.c_str(), NULL);
    return 1;
}

cell_t NativeResponse_GetContent(IPluginContext *pContext, const cell_t *params) {
    ResponseCallback *response = ResponseCallback::ConvertResponse<ResponseCallback>(params[1], pContext);
    if (response == NULL) {
        return 0;
    }

    // Get offset and check range
    int length = static_cast<int>(response->content.length());
    int offset = params[4];
    if (offset < 0) {
        offset = 0;
    }

    if (offset > length) {
        offset = length;
    }


    // Copy the content beginning from offset
    size_t bytes;
    pContext->StringToLocalUTF8(params[2], params[3], response->content.substr(offset).c_str(), &bytes);

    return length - bytes - offset;
}


cell_t NativeResponse_GetContentSize(IPluginContext *pContext, const cell_t *params) {
    ResponseCallback *response = ResponseCallback::ConvertResponse<ResponseCallback>(params[1], pContext);
    if (response == NULL) {
        return 0;
    }

    return response->content.length();
}

cell_t NativeResponse_GetStatusCode(IPluginContext *pContext, const cell_t *params) {
    ResponseCallback *response = ResponseCallback::ConvertResponse<ResponseCallback>(params[1], pContext);
    if (response == NULL) {
        return 0;
    }

    return response->statusCode;
}


cell_t NativeResponse_GetTotalTime(IPluginContext *pContext, const cell_t *params) {
    ResponseCallback *response = ResponseCallback::ConvertResponse<ResponseCallback>(params[1], pContext);
    if (response == NULL) {
        return 0;
    }

    return sp_ftoc(response->totalTime);
}



cell_t NativeHTTPResponse_GetHeader(IPluginContext *pContext, const cell_t *params) {
    HTTPResponseCallback *response = ResponseCallback::ConvertResponse<HTTPResponseCallback>(params[1], pContext);
    if (response == NULL) {
        return 0;
    }

    char *header;
    pContext->LocalToString(params[2], &header);

    if (response->headers.find(header) == response->headers.end()) {
        return 0;
    }

    pContext->StringToLocalUTF8(params[3], params[4], response->headers[header].c_str(), NULL);
    return 1;
}

cell_t NativeHTTPResponse_GetHeadersCount(IPluginContext *pContext, const cell_t *params) {
    HTTPResponseCallback *response = ResponseCallback::ConvertResponse<HTTPResponseCallback>(params[1], pContext);
    if (response == NULL) {
        return 0;
    }

    return response->headers.size();
}

cell_t NativeHTTPResponse_GetHeadersArray(IPluginContext *pContext, const cell_t *params) {
    HTTPResponseCallback *response = ResponseCallback::ConvertResponse<HTTPResponseCallback>(params[1], pContext);
    if (response == NULL) {
        return 0;
    }

    cell_t *array;
    pContext->LocalToPhysAddr(params[2], &array);

    cell_t index = 0;
    std::map<std::string, std::string>::iterator it;
    for (it = response->headers.begin(); it != response->headers.end() && index < params[3]; it++) {
        pContext->StringToLocalUTF8(array[index], params[4], it->first.c_str(), NULL);
        index++;
    }

    return 1;
}