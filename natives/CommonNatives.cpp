/**
 * -----------------------------------------------------
 * File        CommonNatives.cpp
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

#include "Natives.h"
#include "CopyThread.h"
#include "OS.h"

#include "md5/md5.h"
#include "crc/crc.h"

#include <fstream>


cell_t NativeCopyFile(IPluginContext *pContext, const cell_t *params) {
    char *from;
    char *to;

    pContext->LocalToString(params[2], &from);
    pContext->LocalToString(params[3], &to);

    auto callback = system2Extension.CreateCallbackFunction(pContext->GetFunctionById(params[1]));
    if (!callback) {
        pContext->ReportError("Callback ID %x is invalid", params[1]);
        return 0;
    }

    // Start the thread that copys a file
    CopyThread *copyThread = new CopyThread(from, to, params[4], callback);
    system2Extension.RegisterThread(threader->MakeThread(copyThread, Thread_Default));

    return 1;
}


cell_t NativeGetGameDir(IPluginContext *pContext, const cell_t *params) {
    pContext->StringToLocalUTF8(params[1], params[2], smutils->GetGamePath(), NULL);
    return 1;
}


cell_t NativeGetOS(IPluginContext *pContext, const cell_t *params) {
#if defined __WIN32__ || defined _MSC_VER || defined __CYGWIN32__ || defined _Windows || defined __MSDOS__ || defined _WIN64 || defined _WIN32
    return OS_WIN;
#elif defined __unix__ || defined __linux__ || defined __unix
    return OS_UNIX;
#elif defined __APPLE__ || defined __darwin__
    return OS_MAC;
#else
    return OS_UNKNOWN;
#endif
}


cell_t NativeGetStringMD5(IPluginContext *pContext, const cell_t *params) {
    char *str;

    // Get the string to get MD5 of
    pContext->LocalToString(params[1], &str);
    if (!strlen(str)) {
        return 1;
    }

    // Calculate the MD5 hash
    MD5 md5 = MD5();
    md5.update(str, strlen(str));
    md5.finalize();

    // Save the MD5 hash to the plugins buffer
    pContext->StringToLocalUTF8(params[2], params[3], md5.hexdigest().c_str(), NULL);
    return 1;
}


cell_t NativeGetFileMD5(IPluginContext *pContext, const cell_t *params) {
    char *memblock;
    char *filePath;
    char fullFilePath[PLATFORM_MAX_PATH + 1];

    // Get the full paths to the file
    pContext->LocalToString(params[1], &filePath);
    g_pSM->BuildPath(Path_Game, fullFilePath, sizeof(fullFilePath), filePath);

    // Open the file
    std::ifstream file(fullFilePath, std::ifstream::in | std::ifstream::binary | std::ifstream::ate);
    if (file.bad() || !file.is_open()) {
        pContext->StringToLocalUTF8(params[2], params[3], "", NULL);
        return 0;
    }

    // Get the size of the file and save the content to a var
    int size = (int)file.tellg();
    if (size < 1) {
        pContext->StringToLocalUTF8(params[2], params[3], "", NULL);
        return 0;
    }

    memblock = new char[size];
    file.seekg(0, std::ios::beg);
    file.read(memblock, size);
    file.close();

    // Calculate the MD5 hash
    MD5 md5 = MD5();
    md5.update(memblock, size);
    md5.finalize();

    // Free the memory from reading again
    delete[] memblock;

    // Save the MD5 hash to the plugins buffer
    pContext->StringToLocalUTF8(params[2], params[3], md5.hexdigest().c_str(), NULL);

    return 1;
}


cell_t NativeGetStringCRC32(IPluginContext *pContext, const cell_t *params) {
    char *str;

    // Get the string
    pContext->LocalToString(params[1], &str);
    if (!strlen(str)) {
        return 1;
    }

    // Calculate the CRC32 hash
    char crc32[9];
    crc32ToHex(crc32buf(str, strlen(str)), crc32, sizeof(crc32));

    // Save the CRC32 hash to the plugins buffer
    pContext->StringToLocalUTF8(params[2], params[3], crc32, NULL);
    return 1;
}


cell_t NativeGetFileCRC32(IPluginContext *pContext, const cell_t *params) {
    char *memblock;
    char *filePath;
    char fullFilePath[PLATFORM_MAX_PATH + 1];

    // Get the full paths to the file
    pContext->LocalToString(params[1], &filePath);
    g_pSM->BuildPath(Path_Game, fullFilePath, sizeof(fullFilePath), filePath);

    // Open the file
    std::ifstream file(fullFilePath, std::ifstream::in | std::ifstream::binary | std::ifstream::ate);
    if (file.bad() || !file.is_open()) {
        pContext->StringToLocalUTF8(params[2], params[3], "", NULL);
        return 0;
    }

    // Get the size of the file and save the content to a var
    int size = (int)file.tellg();
    if (size < 1) {
        pContext->StringToLocalUTF8(params[2], params[3], "", NULL);
        return 0;
    }

    memblock = new char[size];
    file.seekg(0, std::ios::beg);
    file.read(memblock, size);
    file.close();

    // Calculate the CRC32 hash
    char crc32[9];
    crc32ToHex(crc32buf(memblock, size), crc32, sizeof(crc32));

    // Free the memory from reading again
    delete[] memblock;

    // Save the CRC32 hash to the plugins buffer
    pContext->StringToLocalUTF8(params[2], params[3], crc32, NULL);

    return 1;
}


cell_t NativeURLEncode(IPluginContext *pContext, const cell_t *params) {
    // Get the string to encode
    char *str;
    pContext->LocalToString(params[1], &str);

    // Use the curl escape method to encode it
    CURL *curl = curl_easy_init();
    if (curl) {
        char *output = curl_easy_escape(curl, str, 0);
        if (output) {
            pContext->StringToLocalUTF8(params[2], params[3], output, NULL);
            curl_free(output);

            curl_easy_cleanup(curl);
            return 1;
        }

        curl_easy_cleanup(curl);
        return 0;
    }

    return 0;
}


cell_t NativeURLDecode(IPluginContext *pContext, const cell_t *params) {
    // Get the string to decode
    char *str;
    pContext->LocalToString(params[1], &str);

    // Use the curl unescape method to decode it
    CURL *curl = curl_easy_init();
    if (curl) {
        char *output = curl_easy_unescape(curl, str, 0, NULL);
        if (output) {
            pContext->StringToLocalUTF8(params[2], params[3], output, NULL);
            curl_free(output);

            curl_easy_cleanup(curl);
            return 1;
        }

        curl_easy_cleanup(curl);
        return 0;
    }

    return 0;
}