/**
 * -----------------------------------------------------
 * File        natives.cpp
 * Authors     Popoklopsi, Sourcemod
 * License     GPLv3
 * Web         http://popoklopsi.de
 * -----------------------------------------------------
 *
 * Copyright (C) 2013-2016 Popoklopsi, Sourcemod
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
#include "download.h"
#include "ftp.h"
#include "page.h"


cell_t NativeGetPage(IPluginContext *pContext, const cell_t *params) {
	char *post;
	char *agent;
	char *url;

	pContext->LocalToString(params[2], &url);
	pContext->LocalToString(params[3], &post);
	pContext->LocalToString(params[4], &agent);

	// Start the thread that gets the content of the page
	PageThread *pageThread = new PageThread(url, post, agent, pContext->GetFunctionById(params[1]), params[5]);
	threader->MakeThread(pageThread);

	return 1;
}


cell_t NativeDownloadFileUrl(IPluginContext *pContext, const cell_t *params) {
	char *localFile;
	char *url;

	pContext->LocalToString(params[2], &url);
	pContext->LocalToString(params[3], &localFile);

	// Start the thread that download the content
	DownloadThread *downloadThread = new DownloadThread(url, localFile, pContext->GetFunctionById(params[1]), params[4]);
	threader->MakeThread(downloadThread);

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

	// Start the thread that downloads a file from FTP
	FTPThread *ftpThread = new FTPThread(remoteFile, localFile, host, username, password, params[7], pContext->GetFunctionById(params[1]), MODE_DOWNLOAD, params[8]);
	threader->MakeThread(ftpThread);

	return 1;
}


cell_t NativeUploadFile(IPluginContext *pContext, const cell_t *params) {
	char *remoteFile;
	char *localFile;
	char *host;
	char *username;
	char *password;

	pContext->LocalToString(params[2], &localFile);
	pContext->LocalToString(params[3], &remoteFile);
	pContext->LocalToString(params[4], &host);
	pContext->LocalToString(params[5], &username);
	pContext->LocalToString(params[6], &password);

	// Start the thread that uploads a file to FTP
	FTPThread *ftpThread = new FTPThread(remoteFile, localFile, host, username, password, params[7], pContext->GetFunctionById(params[1]), MODE_UPLOAD, params[8]);
	threader->MakeThread(ftpThread);

	return 1;
}


cell_t NativeCopyFile(IPluginContext *pContext, const cell_t *params) {
	char *file;
	char *path;

	pContext->LocalToString(params[2], &file);
	pContext->LocalToString(params[3], &path);

	// Start the thread that copys a file
	CopyThread *copyThread = new CopyThread(file, path, pContext->GetFunctionById(params[1]), params[4]);
	threader->MakeThread(copyThread);

	return 1;
}


cell_t NativeRunThreadCommand(IPluginContext *pContext, const cell_t *params) {
	char command[2048];

	smutils->FormatString(command, sizeof(command), pContext, params, 2);

	// Start the thread that executes the command
	CommandThread *commandThread = new CommandThread(command, pContext->GetFunctionById(params[1]), 0);
	threader->MakeThread(commandThread);

	return 1;
}


cell_t NativeRunThreadCommandWithData(IPluginContext *pContext, const cell_t *params) {
	char command[2048];

	smutils->FormatString(command, sizeof(command), pContext, params, 3);

	// Start the thread that executes the command
	CommandThread *commandThread = new CommandThread(command, pContext->GetFunctionById(params[1]), params[2]);
	threader->MakeThread(commandThread);

	return 1;
}


cell_t NativeExtractArchive(IPluginContext *pContext, const cell_t *params) {
	char *file;
	char *folder;

	char zdir[PLATFORM_MAX_PATH + 1];
	char ldir[PLATFORM_MAX_PATH + 1];
	char rdir[PLATFORM_MAX_PATH + 1];
	char command[(PLATFORM_MAX_PATH * 3) + 10];

	FILE *testExist;

	pContext->LocalToString(params[2], &file);
	pContext->LocalToString(params[3], &folder);

	// Build the path to the files and folders
#if defined _WIN32
	g_pSM->BuildPath(Path_SM, zdir, sizeof(zdir), "data/system2/7z.exe");
#else
	g_pSM->BuildPath(Path_SM, zdir, sizeof(zdir), "data/system2/7z");
#endif
	g_pSM->BuildPath(Path_Game, ldir, sizeof(ldir), "%s", file);
	g_pSM->BuildPath(Path_Game, rdir, sizeof(rdir), "%s", folder);

	// Test if the local file exists
	if ((testExist = fopen(zdir, "rb")) != NULL) {
		fclose(testExist);

		// Create the extract command
#if defined _WIN32
		sprintf(command, "\"\"%s\" x \"%s\" -o\"%s\" -mmt -aoa\"", zdir, ldir, rdir);
#else
		sprintf(command, "\"%s\" x \"%s\" -o\"%s\" -mmt -aoa", zdir, ldir, rdir);
#endif

		// Start the thread that executes the command
		CommandThread *commandThread = new CommandThread(command, pContext->GetFunctionById(params[1]), params[4]);
		threader->MakeThread(commandThread);
	} else {
		g_pSM->LogError(myself, "Attention: Coulnd't find %s to extract %s", zdir, ldir);
	}

	return 1;
}


cell_t NativeCompressFile(IPluginContext *pContext, const cell_t *params) {
	char *file;
	char *folder;

	char archive[12];
	char level[6];

	char zdir[PLATFORM_MAX_PATH + 1];
	char ldir[PLATFORM_MAX_PATH + 1];
	char rdir[PLATFORM_MAX_PATH + 1];
	char command[(PLATFORM_MAX_PATH * 3) + 30];

	FILE* testExist;

	pContext->LocalToString(params[2], &file);
	pContext->LocalToString(params[3], &folder);

	// Build the path to the files and folders
#if defined _WIN32
	g_pSM->BuildPath(Path_SM, zdir, sizeof(zdir), "data/system2/7z.exe");
#else
	g_pSM->BuildPath(Path_SM, zdir, sizeof(zdir), "data/system2/7z");
#endif
	g_pSM->BuildPath(Path_Game, ldir, sizeof(ldir), "%s", file);
	g_pSM->BuildPath(Path_Game, rdir, sizeof(rdir), "%s", folder);

	// Get the compress level
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
		case 4:
		{
			strcpy(level, "-mx9");
			break;
		}
	}

	// Get the archive to use
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
		case 4:
		{
			strcpy(archive, "-ttar");
			break;
		}
	}

	// 7z exists?
	if ((testExist = fopen(zdir, "rb")) != NULL) {
		fclose(testExist);

		// Create the compress command
#if defined _WIN32
		sprintf(command, "\"\"%s\" a %s \"%s\" \"%s\" -mmt %s\"", zdir, archive, rdir, ldir, level);
#else
		sprintf(command, "\"%s\" a %s \"%s\" \"%s\" -mmt %s", zdir, archive, rdir, ldir, level);
#endif

		// Start the thread that executes the command
		CommandThread *commandThread = new CommandThread(command, pContext->GetFunctionById(params[1]), params[6]);
		threader->MakeThread(commandThread);
	} else {
		g_pSM->LogError(myself, "Attention: Coulnd't find %s to compress %s", zdir, ldir);
	}

	return 1;
}


cell_t NativeRunCommand(IPluginContext *pContext, const cell_t *params) {
	char cmdString[2048];
	char buffer[MAX_RESULT_LENGTH];
	char resultString[MAX_RESULT_LENGTH];

	// Format the command string
	smutils->FormatString(cmdString, sizeof(cmdString), pContext, params, 3);

	// Prevent output to console
	if (strstr(cmdString, "2>&1") == NULL) {
		strcat(cmdString, " 2>&1");
	}

	// Execute the command
	FILE *command = PosixOpen(cmdString, "r");
	cell_t result = 0;

	// Was there an error?
	if (!command) {
		// Return the error
		pContext->StringToLocal(params[1], params[2], "ERROR: Couldn't execute the command!");

		return 2;
	}

	// Read the result
	while (fgets(buffer, sizeof(buffer), command) != NULL) {
		size_t realsize = strlen(buffer);

		// More than MAX_RESULT_LENGTH?
		if (strlen(resultString) + realsize >= size_t(params[2] - 1)) {
			// Only make the result full!
			strncat(resultString, buffer, (params[2] - strlen(resultString)) - 1);
			pContext->StringToLocal(params[1], params[2], resultString);

			break;
		}

		strcat(resultString, buffer);
	}


	if (strlen(resultString) == 0) {
		pContext->StringToLocal(params[1], params[2], "Empty reading result!");
		result = 1;
	} else {
		pContext->StringToLocal(params[1], params[2], resultString);
	}

	// Close Posix and return the result
	PosixClose(command);
	return result;
}


cell_t NativeGetGameDir(IPluginContext *pContext, const cell_t *params) {
	// Save to string
	pContext->StringToLocal(params[1], params[2], smutils->GetGamePath());

	return 1;
}


// Get the os
cell_t NativeGetOS(IPluginContext *pContext, const cell_t *params) {
	// So what we have now :)
#if defined __WIN32__ || _MSC_VER || __CYGWIN32__ || _Windows || __MSDOS__ || _WIN64 || _WIN32
	return OS_WIN;
#elif defined __unix__ || __linux__ || __unix
	return OS_UNIX;
#elif defined __APPLE__ || __darwin__
	return OS_MAC;
#else
	return OS_UNKNOWN;
#endif
}
