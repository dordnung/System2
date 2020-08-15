/**
 * -----------------------------------------------------
 * File        LegacyNatives.cpp
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

#include "LegacyNatives.h"
#include "LegacyCommandThread.h"
#include "LegacyDownloadThread.h"
#include "LegacyFTPThread.h"
#include "LegacyPageThread.h"
#include "LegacyCommandState.h"

#if defined __unix__ || defined __linux__ || defined __unix
#include <sys/utsname.h>
#endif

#define MAX_COMMAND_LENGTH 2048


cell_t NativeGetPage(IPluginContext *pContext, const cell_t *params) {
    char *url;
    char *post;
    char *agent;

    pContext->LocalToString(params[2], &url);
    pContext->LocalToString(params[3], &post);
    pContext->LocalToString(params[4], &agent);

    auto callback = system2Extension.CreateCallbackFunction(pContext->GetFunctionById(params[1]));
    if (!callback) {
        return 0;
    }

    // Start the thread that gets the content of the page
    LegacyPageThread *pageThread = new LegacyPageThread(url, post, agent, params[5], callback);
    pageThread->RunThread();

    return 1;
}


cell_t NativeDownloadFileUrl(IPluginContext *pContext, const cell_t *params) {
    char *url;
    char *localFile;

    pContext->LocalToString(params[2], &url);
    pContext->LocalToString(params[3], &localFile);

    auto callback = system2Extension.CreateCallbackFunction(pContext->GetFunctionById(params[1]));
    if (!callback) {
        return 0;
    }

    // Start the thread that download the content
    LegacyDownloadThread *downloadThread = new LegacyDownloadThread(url, localFile, params[4], callback);
    downloadThread->RunThread();

    return 1;
}


cell_t NativeDownloadFile(IPluginContext *pContext, const cell_t *params) {
    char *remoteFile;
    char *localFile;
    char *host;
    char *username;
    char *password;

    pContext->LocalToString(params[2], &remoteFile);
    pContext->LocalToString(params[3], &localFile);
    pContext->LocalToString(params[4], &host);
    pContext->LocalToString(params[5], &username);
    pContext->LocalToString(params[6], &password);

    auto callback = system2Extension.CreateCallbackFunction(pContext->GetFunctionById(params[1]));
    if (!callback) {
        return 0;
    }

    // Start the thread that downloads a file from FTP
    LegacyFTPThread *ftpThread = new LegacyFTPThread(true, remoteFile, localFile, host, username, password, params[7], params[8], callback);
    ftpThread->RunThread();

    return 1;
}


cell_t NativeUploadFile(IPluginContext *pContext, const cell_t *params) {
    char *localFile;
    char *remoteFile;
    char *host;
    char *username;
    char *password;

    pContext->LocalToString(params[2], &localFile);
    pContext->LocalToString(params[3], &remoteFile);
    pContext->LocalToString(params[4], &host);
    pContext->LocalToString(params[5], &username);
    pContext->LocalToString(params[6], &password);

    auto callback = system2Extension.CreateCallbackFunction(pContext->GetFunctionById(params[1]));
    if (!callback) {
        return 0;
    }

    // Start the thread that uploads a file to FTP
    LegacyFTPThread *ftpThread = new LegacyFTPThread(false, remoteFile, localFile, host, username, password, params[7], params[8], callback);
    ftpThread->RunThread();

    return 1;
}


cell_t NativeCompressFile(IPluginContext *pContext, const cell_t *params) {
    char *file;
    char *folder;

    char zdir[PLATFORM_MAX_PATH + 1];
    char ldir[PLATFORM_MAX_PATH + 1];
    char rdir[PLATFORM_MAX_PATH + 1];

    pContext->LocalToString(params[2], &file);
    pContext->LocalToString(params[3], &folder);

    // Build the path to the files and folders
#if defined _WIN32 || defined _WIN64
    g_pSM->BuildPath(Path_SM, zdir, sizeof(zdir), "data/system2/win/7z.exe");
#else
    struct utsname unameData;
    uname(&unameData);

    if (strcmp(unameData.machine, "x86_64") == 0 || strcmp(unameData.machine, "amd64") == 0) {
        g_pSM->BuildPath(Path_SM, zdir, sizeof(zdir), "data/system2/linux/amd64/7z");
    } else {
        g_pSM->BuildPath(Path_SM, zdir, sizeof(zdir), "data/system2/linux/i386/7z");
    }
#endif
    g_pSM->BuildPath(Path_Game, ldir, sizeof(ldir), file);
    g_pSM->BuildPath(Path_Game, rdir, sizeof(rdir), folder);

    // Get the compress level
    char level[6];
    switch (params[5]) {
        case 0:
        {
            strcpy(level, "-mx1");
            break;
        }
        case 1:
        {
            strcpy(level, "-mx3");
            break;
        }
        case 2:
        {
            strcpy(level, "-mx5");
            break;
        }
        case 3:
        {
            strcpy(level, "-mx7");
            break;
        }
        default:
        {
            strcpy(level, "-mx9");
            break;
        }
    }

    // Get the archive to use
    char archive[12];
    switch (params[4]) {
        case 0:
        {
            strcpy(archive, "-tzip");
            break;
        }
        case 1:
        {
            strcpy(archive, "-t7z");
            break;
        }
        case 2:
        {
            strcpy(archive, "-tgzip");
            break;
        }
        case 3:
        {
            strcpy(archive, "-tbzip2");
            break;
        }
        default:
        {
            strcpy(archive, "-ttar");
            break;
        }
    }

    // 7z exists?
    FILE *testExist;
    if ((testExist = fopen(zdir, "rb")) != NULL) {
        fclose(testExist);

        // Create the compress command
        char command[MAX_COMMAND_LENGTH + 1];
#if defined _WIN32 || defined _WIN64
        sprintf(command, "\"\"%s\" a %s \"%s\" \"%s\" -mmt %s\"", zdir, archive, rdir, ldir, level);
#else
        sprintf(command, "\"%s\" a %s \"%s\" \"%s\" -mmt %s", zdir, archive, rdir, ldir, level);
#endif

        auto callback = system2Extension.CreateCallbackFunction(pContext->GetFunctionById(params[1]));
        if (!callback) {
            return 0;
        }

        // Start the thread that executes the command
        LegacyCommandThread *commandThread = new LegacyCommandThread(command, params[6], callback);
        commandThread->RunThread();
    } else {
        g_pSM->LogError(myself, "ERROR: Coulnd't find 7-ZIP at %s to compress %s", zdir, ldir);
    }

    return 1;
}


cell_t NativeExtractArchive(IPluginContext *pContext, const cell_t *params) {
    char *file;
    char *folder;

    char zdir[PLATFORM_MAX_PATH + 1];
    char ldir[PLATFORM_MAX_PATH + 1];
    char rdir[PLATFORM_MAX_PATH + 1];

    pContext->LocalToString(params[2], &file);
    pContext->LocalToString(params[3], &folder);

    // Build the path to the files and folders
#if defined _WIN32 || defined _WIN64
    g_pSM->BuildPath(Path_SM, zdir, sizeof(zdir), "data/system2/win/7z.exe");
#else
    struct utsname unameData;
    uname(&unameData);

    if (strcmp(unameData.machine, "x86_64") == 0 || strcmp(unameData.machine, "amd64") == 0) {
        g_pSM->BuildPath(Path_SM, zdir, sizeof(zdir), "data/system2/linux/amd64/7z");
    } else {
        g_pSM->BuildPath(Path_SM, zdir, sizeof(zdir), "data/system2/linux/i386/7z");
    }
#endif
    g_pSM->BuildPath(Path_Game, ldir, sizeof(ldir), file);
    g_pSM->BuildPath(Path_Game, rdir, sizeof(rdir), folder);

    // Test if the local file exists
    FILE *testExist;
    if ((testExist = fopen(zdir, "rb")) != NULL) {
        fclose(testExist);

        // Create the extract command
        char command[MAX_COMMAND_LENGTH + 1];
#if defined _WIN32
        sprintf(command, "\"\"%s\" x \"%s\" -o\"%s\" -mmt -aoa\"", zdir, ldir, rdir);
#else
        sprintf(command, "\"%s\" x \"%s\" -o\"%s\" -mmt -aoa", zdir, ldir, rdir);
#endif

        auto callback = system2Extension.CreateCallbackFunction(pContext->GetFunctionById(params[1]));
        if (!callback) {
            return 0;
        }

        // Start the thread that executes the command
        LegacyCommandThread *commandThread = new LegacyCommandThread(command, params[4], callback);
        commandThread->RunThread();
    } else {
        g_pSM->LogError(myself, "ERROR: Coulnd't find 7-ZIP at %s to extract %s", zdir, ldir);
    }

    return 1;
}


cell_t NativeRunThreadCommand(IPluginContext *pContext, const cell_t *params) {
    char command[MAX_COMMAND_LENGTH + 1];

    smutils->FormatString(command, sizeof(command), pContext, params, 2);

    auto callback = system2Extension.CreateCallbackFunction(pContext->GetFunctionById(params[1]));
    if (!callback) {
        return 0;
    }

    // Start the thread that executes the command
    LegacyCommandThread *commandThread = new LegacyCommandThread(command, 0, callback);
    commandThread->RunThread();

    return 1;
}


cell_t NativeRunThreadCommandWithData(IPluginContext *pContext, const cell_t *params) {
    char command[MAX_COMMAND_LENGTH + 1];

    smutils->FormatString(command, sizeof(command), pContext, params, 3);

    auto callback = system2Extension.CreateCallbackFunction(pContext->GetFunctionById(params[1]));
    if (!callback) {
        return 0;
    }

    // Start the thread that executes the command
    LegacyCommandThread *commandThread = new LegacyCommandThread(command, params[2], callback);
    commandThread->RunThread();

    return 1;
}


cell_t NativeRunCommand(IPluginContext *pContext, const cell_t *params) {
    // Format the command string
    char cmdString[MAX_COMMAND_LENGTH + 1];
    smutils->FormatString(cmdString, sizeof(cmdString), pContext, params, 3);

    // Redirect everything to output
    std::string command = cmdString;
    std::string redirect = " 2>&1";
    if (redirect.size() > command.size() || !std::equal(redirect.rbegin(), redirect.rend(), command.rbegin())) {
        command += redirect;
    }

    // Execute the command
    FILE *commandFile = PosixOpen(command.c_str(), "r");

    // Was there an error?
    if (!commandFile) {
        // Return the error
        pContext->StringToLocalUTF8(params[1], params[2], "ERROR: Couldn't execute the command!", NULL);
        return CMD_ERROR;
    }

    // Read the result
    LegacyCommandState state = CMD_SUCCESS;
    std::string output;

    char buffer[MAX_RESULT_LENGTH + 1];
    while (fgets(buffer, sizeof(buffer), commandFile) != NULL) {
        // More than MAX_RESULT_LENGTH?
        if (output.length() + strlen(buffer) >= size_t(params[2] - 1)) {
            // Only make the result full!
            output.append(buffer, (params[2] - output.length()) - 1);
            break;
        }

        // Add buffer to result
        output += buffer;
    }


    if (output.empty()) {
        pContext->StringToLocalUTF8(params[1], params[2], "Empty reading result!", NULL);
        state = CMD_EMPTY;
    } else {
        pContext->StringToLocalUTF8(params[1], params[2], output.c_str(), NULL);
    }

    // Close Posix and return the result
    PosixClose(commandFile);
    return state;
}