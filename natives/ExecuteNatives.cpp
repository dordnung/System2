/**
 * -----------------------------------------------------
 * File        ExecuteNatives.cpp
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
#include "ExecuteCallbackHandler.h"
#include "ExecuteThread.h"
#include "ExecuteCallback.h"
#include "CompressLevel.h"
#include "CompressArchive.h"

#if defined __unix__ || defined __linux__ || defined __unix
#include <sys/utsname.h>
#endif

#define MAX_COMMAND_LENGTH 2048


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

        // Start the thread that executes the command
        ExecuteThread *commandThread = new ExecuteThread(command, params[6], pContext->GetFunctionById(params[1]));
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

        // Start the thread that executes the command
        ExecuteThread *commandThread = new ExecuteThread(command, params[4], pContext->GetFunctionById(params[1]));
        threader->MakeThread(commandThread);
    } else {
        g_pSM->LogError(myself, "ERROR: Coulnd't find 7-ZIP executable at %s to extract %s", binDir, fullArchivePath);
    }

    return 1;
}


cell_t NativeExecuteThreaded(IPluginContext *pContext, const cell_t *params) {
    char *command;
    pContext->LocalToString(params[2], &command);

    // Start the thread that executes the command
    ExecuteThread *commandThread = new ExecuteThread(command, params[3], pContext->GetFunctionById(params[1]));
    threader->MakeThread(commandThread);

    return 1;
}


cell_t NativeExecuteFormattedThreaded(IPluginContext *pContext, const cell_t *params) {
    char command[MAX_COMMAND_LENGTH + 1];
    smutils->FormatString(command, sizeof(command), pContext, params, 3);

    // Start the thread that executes the command - with data
    ExecuteThread *commandThread = new ExecuteThread(command, params[2], pContext->GetFunctionById(params[1]));
    threader->MakeThread(commandThread);

    return 1;
}


cell_t NativeExecuteOutput_GetOutput(IPluginContext *pContext, const cell_t *params) {
    // Get the handle to the command callback
    Handle_t hndl = static_cast<Handle_t>(params[1]);
    HandleError err;

    ExecuteCallback *callback;
    if ((err = executeCallbackHandler.ReadHandle(hndl, pContext->GetIdentity(), &callback)) != HandleError_None) {
        pContext->ReportError("Invalid execute output handle %x (error %d)", hndl, err);
        return 0;
    }

    // Get offset and check range
    int offset = params[4];
    if (offset < 0) {
        offset = 0;
    }
    if (offset > (int)callback->GetOutput().length()) {
        offset = callback->GetOutput().length();
    }

    // Copy the output beginning from offset
    pContext->StringToLocalUTF8(params[2], params[3], callback->GetOutput().substr(offset).c_str(), NULL);
    return 1;
}


cell_t NativeExecuteOutput_GetSize(IPluginContext *pContext, const cell_t *params) {
    // Get the handle to the command callback
    Handle_t hndl = static_cast<Handle_t>(params[1]);
    HandleError err;

    ExecuteCallback *callback;
    if ((err = executeCallbackHandler.ReadHandle(hndl, pContext->GetIdentity(), &callback)) != HandleError_None) {
        pContext->ReportError("Invalid execute output handle %x (error %d)", hndl, err);
        return 0;
    }

    // Just return the length
    return callback->GetOutput().length();
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

    // Close Posix
    PosixClose(commandFile);

    // Set the result output
    pContext->StringToLocalUTF8(params[1], params[2], output.c_str(), NULL);
    return true;
}