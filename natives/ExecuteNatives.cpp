/**
 * -----------------------------------------------------
 * File        ExecuteNatives.cpp
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

#include "Natives.h"
#include "ExecuteCallbackHandler.h"
#include "ExecuteThread.h"
#include "ExecuteCallback.h"
#include "CompressLevel.h"
#include "CompressArchive.h"

#if defined _WIN32 || defined _WIN64
#include <io.h>
#define access _access
#define X_OK 0x4
#else
#include <sys/utsname.h>
#include <unistd.h>
#endif

#define MAX_COMMAND_LENGTH 2048

bool Get7ZIPExecutable(bool force32Bit, std::string& binDir) {
    static std::string sBinDir64;
    static std::string sBinDir32;

    // 64-bit has a higher priority (when not forcing 32-bit)
    if (!force32Bit && !sBinDir64.empty()) {
        binDir = sBinDir64;
        return true;
    } else if (!sBinDir32.empty()) {
        binDir = sBinDir32;
        return true;
    }

#if defined _WIN32 || defined _WIN64
    char binDir32[PLATFORM_MAX_PATH + 1];

    g_pSM->BuildPath(Path_SM, binDir32, sizeof(binDir32), "data/system2/win/7z.exe");
    binDir = binDir32;

    // Check if bin dir can be executed
    if (access(binDir32, X_OK) != -1) {
        sBinDir32 = binDir32;
        sBinDir64 = binDir32;

        return true;
    }

    return false;
#else
    struct utsname unameData;
    uname(&unameData);

    char binDir32[PLATFORM_MAX_PATH + 1];
    char binDir64[PLATFORM_MAX_PATH + 1];

    bool is64BitMachine = !strcmp(unameData.machine, "x86_64") || !strcmp(unameData.machine, "amd64");
    if (is64BitMachine) {
        g_pSM->BuildPath(Path_SM, binDir64, sizeof(binDir64), "data/system2/linux/amd64/7z");
        if (access(binDir64, X_OK) != -1) {
            sBinDir64 = binDir64;
        }
    }

    g_pSM->BuildPath(Path_SM, binDir32, sizeof(binDir32), "data/system2/linux/i386/7z");
    if (access(binDir32, X_OK) != -1) {
        sBinDir32 = binDir32;
    }

    if (!is64BitMachine || force32Bit) {
        binDir = binDir32;
        return !sBinDir32.empty();
    }

    binDir = binDir64;
    return !sBinDir64.empty();
#endif
}

cell_t NativeCheck7ZIP(IPluginContext* pContext, const cell_t* params) {
    std::string binDir;
    bool valid = Get7ZIPExecutable(params[3], binDir);

    pContext->StringToLocalUTF8(params[1], params[2], binDir.c_str(), nullptr);

    return valid;
}

cell_t NativeCompress(IPluginContext* pContext, const cell_t* params) {
    char* path;
    char* archive;
    char fullPath[PLATFORM_MAX_PATH + 1];
    char fullArchivePath[PLATFORM_MAX_PATH + 1];

    std::string binDir;
    if (!Get7ZIPExecutable(params[7], binDir)) {
        return 0;
    }

    pContext->LocalToString(params[2], &path);
    pContext->LocalToString(params[3], &archive);

    auto callback = system2Extension.CreateCallbackFunction(pContext->GetFunctionById(params[1]));
    if (!callback) {
        pContext->ThrowNativeError("Callback ID %x is invalid", params[1]);
        return 0;
    }

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

    // Create the compress command
    std::string command;
#if defined _WIN32 || defined _WIN64
    command = "\"\"" + binDir + "\" a " + archiveType + " \"" + std::string(fullArchivePath) + "\" \"" + std::string(fullPath) + "\" -mmt " + level + "\"";
#else
    command = "\"" + binDir + "\" a " + archiveType + " \"" + std::string(fullArchivePath) + "\" \"" + std::string(fullPath) + "\" -mmt " + level + " 2>&1";
#endif

    // Start the thread that executes the command
    ExecuteThread* commandThread = new ExecuteThread(command, params[6], callback);
    commandThread->RunThread();

    return 1;
}

cell_t NativeExtract(IPluginContext* pContext, const cell_t* params) {
    char* path;
    char* archive;
    char fullArchivePath[PLATFORM_MAX_PATH + 1];
    char fullPath[PLATFORM_MAX_PATH + 1];

    std::string binDir;
    if (!Get7ZIPExecutable(params[5], binDir)) {
        return 0;
    }

    pContext->LocalToString(params[2], &path);
    pContext->LocalToString(params[3], &archive);

    auto callback = system2Extension.CreateCallbackFunction(pContext->GetFunctionById(params[1]));
    if (!callback) {
        pContext->ThrowNativeError("Callback ID %x is invalid", params[1]);
        return 0;
    }

    g_pSM->BuildPath(Path_Game, fullArchivePath, sizeof(fullArchivePath), path);
    g_pSM->BuildPath(Path_Game, fullPath, sizeof(fullPath), archive);

    // Create the extract command
    std::string command;
#if defined _WIN32
    command = "\"\"" + binDir + "\" x \"" + std::string(fullArchivePath) + "\" -o\"" + std::string(fullPath) + "\" -mmt -aoa\"";
#else
    command = "\"" + binDir + "\" x \"" + std::string(fullArchivePath) + "\" -o\"" + std::string(fullPath) + "\" -mmt -aoa 2>&1";
#endif

    // Start the thread that executes the command
    ExecuteThread* commandThread = new ExecuteThread(command, params[4], callback);
    commandThread->RunThread();

    return 1;
}

cell_t NativeExecuteThreaded(IPluginContext* pContext, const cell_t* params) {
    char* command;
    pContext->LocalToString(params[2], &command);

    auto callback = system2Extension.CreateCallbackFunction(pContext->GetFunctionById(params[1]));
    if (!callback) {
        pContext->ThrowNativeError("Callback ID %x is invalid", params[1]);
        return 0;
    }

    // Start the thread that executes the command
    ExecuteThread* commandThread = new ExecuteThread(command, params[3], callback);
    commandThread->RunThread();

    return 1;
}

cell_t NativeExecuteFormattedThreaded(IPluginContext* pContext, const cell_t* params) {
    char command[MAX_COMMAND_LENGTH + 1];
    smutils->FormatString(command, sizeof(command), pContext, params, 3);

    auto callback = system2Extension.CreateCallbackFunction(pContext->GetFunctionById(params[1]));
    if (!callback) {
        pContext->ThrowNativeError("Callback ID %x is invalid", params[1]);
        return 0;
    }

    // Start the thread that executes the command - with data
    ExecuteThread* commandThread = new ExecuteThread(command, params[2], callback);
    commandThread->RunThread();

    return 1;
}

cell_t NativeExecuteOutput_GetOutput(IPluginContext* pContext, const cell_t* params) {
    // Get the handle to the execute callback
    Handle_t hndl = static_cast<Handle_t>(params[1]);

    ExecuteCallback* callback = ExecuteCallback::ConvertExecuteCallback(hndl, pContext);
    if (!callback) {
        return 0;
    }

    // Get offset and check range
    std::string output = callback->GetOutput();

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

cell_t NativeExecuteOutput_GetLength(IPluginContext* pContext, const cell_t* params) {
    // Get the handle to the execute callback
    Handle_t hndl = static_cast<Handle_t>(params[1]);

    ExecuteCallback* callback = ExecuteCallback::ConvertExecuteCallback(hndl, pContext);
    if (!callback) {
        return 0;
    }

    // Just return the length
    return callback->GetOutput().length();
}

cell_t NativeExecuteOutput_GetExitStatus(IPluginContext* pContext, const cell_t* params) {
    // Get the handle to the execute callback
    Handle_t hndl = static_cast<Handle_t>(params[1]);

    ExecuteCallback* callback = ExecuteCallback::ConvertExecuteCallback(hndl, pContext);
    if (!callback) {
        return 0;
    }

    // Just return the exit status
    return callback->GetExitStatus();
}

cell_t NativeExecute(IPluginContext* pContext, const cell_t* params) {
    char* command;
    pContext->LocalToString(params[3], &command);

    return NativeExecuteCommand(command, pContext, params);
}

cell_t NativeExecuteFormatted(IPluginContext* pContext, const cell_t* params) {
    // Format the command string
    char command[MAX_COMMAND_LENGTH + 1];
    smutils->FormatString(command, sizeof(command), pContext, params, 3);

    return NativeExecuteCommand(command, pContext, params);
}

cell_t NativeExecuteCommand(std::string command, IPluginContext* pContext, const cell_t* params) {
    // Execute the command
    FILE* commandFile = PosixOpen(command.c_str(), "r");

    // Was there an error?
    if (!commandFile) {
        pContext->StringToLocal(params[1], params[2], "");
        return 0;
    }

    // Read the result
    std::string output;

    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), commandFile)) {
        // Add buffer to the output
        output += buffer;
    }

    // Close Posix
    PosixClose(commandFile);

    // Set the result output
    pContext->StringToLocalUTF8(params[1], params[2], output.c_str(), nullptr);
    return 1;
}