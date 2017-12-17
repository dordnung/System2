/**
 * -----------------------------------------------------
 * File        natives.cpp
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

#include "natives.h"
#include "command.h"
#include "copy.h"
#include "md5.h"
#include "crc.h"

#include <fstream>
#include <sstream> 
#if defined __unix__ || defined __linux__ || defined __unix
#include <sys/utsname.h>
#endif

#define MAX_COMMAND_LENGTH 2048

enum OS {
    OS_UNKNOWN,
    OS_WIN,
    OS_UNIX,
    OS_MAC
};

enum CompressArchive {
    ARCHIVE_ZIP,
    ARCHIVE_7Z,
    ARCHIVE_GZIP,
    ARCHIVE_BZIP2,
    ARCHIVE_TAR
};

enum CompressLevel {
    LEVEL_1,
    LEVEL_3,
    LEVEL_5,
    LEVEL_7,
    LEVEL_9
};


cell_t NativeCopyFile(IPluginContext *pContext, const cell_t *params) {
    char *from;
    char *to;

    pContext->LocalToString(params[2], &from);
    pContext->LocalToString(params[3], &to);

    // Start the thread that copys a file
    CopyThread *copyThread = new CopyThread(from, to, pContext->GetFunctionById(params[1]), params[4]);
    threader->MakeThread(copyThread);

    return 1;
}


cell_t NativeCompress(IPluginContext *pContext, const cell_t *params) {
    char *path;
    char *archive;

    char binDir[PLATFORM_MAX_PATH + 1];
    char fullPath[PLATFORM_MAX_PATH + 1];
    char fullArchivePath[PLATFORM_MAX_PATH + 1];

    pContext->LocalToString(params[2], &path);
    pContext->LocalToString(params[3], &archive);

    // Build the path to the executable, to the path to compress and the archive
#if defined _WIN32 || defined _WIN64
    g_pSM->BuildPath(Path_SM, binDir, sizeof(binDir), "data/system2/win/7z.exe");
#else
    struct utsname unameData;
    uname(&unameData);

    if (strcmp(unameData.machine, "x86_64") == 0 || strcmp(unameData.machine, "amd64") == 0) {
        g_pSM->BuildPath(Path_SM, binDir, sizeof(binDir), "data/system2/linux/amd64/7z");
    } else {
        g_pSM->BuildPath(Path_SM, binDir, sizeof(binDir), "data/system2/linux/i386/7z");
    }
#endif
    g_pSM->BuildPath(Path_Game, fullPath, sizeof(fullPath), path);
    g_pSM->BuildPath(Path_Game, fullArchivePath, sizeof(fullArchivePath), archive);

    // Get the compress level
    std::string level;
    switch (params[5]) {
        case LEVEL_1:
        {
            level = "-mx1";
            break;
        }
        case LEVEL_3:
        {
            level = "-mx3";
            break;
        }
        case LEVEL_5:
        {
            level = "-mx5";
            break;
        }
        case LEVEL_7:
        {
            level = "-mx7";
            break;
        }
        default:
        {
            level = "-mx9";
            break;
        }
    }

    // Get the archive to use
    std::string archiveType;
    switch (params[4]) {
        case ARCHIVE_ZIP:
        {
            archiveType = "-tzip";
            break;
        }
        case ARCHIVE_7Z:
        {
            archiveType = "-t7z";
            break;
        }
        case ARCHIVE_GZIP:
        {
            archiveType = "-tgzip";
            break;
        }
        case ARCHIVE_BZIP2:
        {
            archiveType = "-tbzip2";
            break;
        }
        default:
        {
            archiveType = "-ttar";
            break;
        }
    }

    // 7z exists?
    FILE *testExist;
    if ((testExist = fopen(binDir, "rb")) != NULL) {
        fclose(testExist);

        // Create the compress command
        std::string command;
#if defined _WIN32 || defined _WIN64
        command = "\"\"" + std::string(binDir) + "\" a " + archiveType + " \"" + std::string(fullArchivePath) + "\" \"" + std::string(fullPath) + "\" -mmt " + level + "\"";
#else
        command = "\"" + std::string(binDir) + "\" a " + archiveType + " \"" + std::string(fullArchivePath) + "\" \"" + std::string(fullPath) + "\" -mmt " + level + " 2>&1";
#endif
        // Logging command
        g_pSM->LogMessage(myself, "Extracting archive: %s", command.c_str());

        // Start the thread that executes the command
        CommandThread *commandThread = new CommandThread(command, pContext->GetFunctionById(params[1]), params[6]);
        threader->MakeThread(commandThread);
    } else {
        g_pSM->LogError(myself, "ERROR: Coulnd't find 7-ZIP executable at %s to compress %s", binDir, fullPath);
    }

    return 1;
}


cell_t NativeExtract(IPluginContext *pContext, const cell_t *params) {
    char *path;
    char *archive;

    char binDir[PLATFORM_MAX_PATH + 1];
    char fullArchivePath[PLATFORM_MAX_PATH + 1];
    char fullPath[PLATFORM_MAX_PATH + 1];

    pContext->LocalToString(params[2], &path);
    pContext->LocalToString(params[3], &archive);

    // Build the path to the executable, to the path to extract to and the archive
#if defined _WIN32 || defined _WIN64
    g_pSM->BuildPath(Path_SM, binDir, sizeof(binDir), "data/system2/win/7z.exe");
#else
    struct utsname unameData;
    uname(&unameData);

    if (strcmp(unameData.machine, "x86_64") == 0 || strcmp(unameData.machine, "amd64") == 0) {
        g_pSM->BuildPath(Path_SM, binDir, sizeof(binDir), "data/system2/linux/amd64/7z");
    } else {
        g_pSM->BuildPath(Path_SM, binDir, sizeof(binDir), "data/system2/linux/i386/7z");
    }
#endif
    g_pSM->BuildPath(Path_Game, fullArchivePath, sizeof(fullArchivePath), path);
    g_pSM->BuildPath(Path_Game, fullPath, sizeof(fullPath), archive);

    // Test if the local file exists
    FILE *testExist;
    if ((testExist = fopen(binDir, "rb")) != NULL) {
        fclose(testExist);

        // Create the extract command
        std::string command;
#if defined _WIN32
        command = "\"\"" + std::string(binDir) + "\" x \"" + std::string(fullArchivePath) + "\" -o\"" + std::string(fullPath) + "\" -mmt -aoa\"";
#else
        command = "\"" + std::string(binDir) + "\" x \"" + std::string(fullArchivePath) + "\" -o\"" + std::string(fullPath) + "\" -mmt -aoa 2>&1";
#endif
        // Logging command
        g_pSM->LogMessage(myself, "Extracting archive: %s", command.c_str());

        // Start the thread that executes the command
        CommandThread *commandThread = new CommandThread(command, pContext->GetFunctionById(params[1]), params[4]);
        threader->MakeThread(commandThread);
    } else {
        g_pSM->LogError(myself, "ERROR: Coulnd't find 7-ZIP executable at %s to extract %s", binDir, fullArchivePath);
    }

    return 1;
}


cell_t NativeExecuteThreaded(IPluginContext *pContext, const cell_t *params) {
    char *command;
    pContext->LocalToString(params[3], &command);

    // Start the thread that executes the command
    CommandThread *commandThread = new CommandThread(command, pContext->GetFunctionById(params[1]), params[2]);
    threader->MakeThread(commandThread);

    return 1;
}


cell_t NativeExecuteFormattedThreaded(IPluginContext *pContext, const cell_t *params) {
    char command[MAX_COMMAND_LENGTH + 1];
    smutils->FormatString(command, sizeof(command), pContext, params, 3);

    // Start the thread that executes the command - with data
    CommandThread *commandThread = new CommandThread(command, pContext->GetFunctionById(params[1]), params[2]);
    threader->MakeThread(commandThread);

    return 1;
}


cell_t NativeExecute(IPluginContext *pContext, const cell_t *params) {
    char *command;
    pContext->LocalToString(params[3], &command);

    return NativeExecuteCommand(command, pContext, params);
}


cell_t NativeExecuteFormatted(IPluginContext *pContext, const cell_t *params) {
    // Format the command string
    char command[MAX_COMMAND_LENGTH + 1];
    smutils->FormatString(command, sizeof(command), pContext, params, 3);

    return NativeExecuteCommand(command, pContext, params);
}


cell_t NativeExecuteCommand(std::string command, IPluginContext *pContext, const cell_t *params) {
    // Execute the command
    FILE *commandFile = PosixOpen(command.c_str(), "r");

    // Was there an error?
    if (!commandFile) {
        pContext->StringToLocal(params[1], params[2], "");
        return false;
    }

    // Read the result
    std::string output;

    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), commandFile) != NULL) {
        // Add buffer to the output
        output += buffer;
    }

    pContext->StringToLocalUTF8(params[1], params[2], output.c_str(), NULL);

    // Close Posix and return the result
    PosixClose(commandFile);
    return true;
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
