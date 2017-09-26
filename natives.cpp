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
#include "download.h"
#include "ftp.h"
#include "page.h"
#include "md5.h"
#include "crc.h"


#if defined __unix__ || defined __linux__ || defined __unix
#include <sys/utsname.h>
#endif


cell_t NativeGetPage(IPluginContext *pContext, const cell_t *params) {
	char *url;
	char *post;
	char *agent;

	pContext->LocalToString(params[2], &url);
	pContext->LocalToString(params[3], &post);
	pContext->LocalToString(params[4], &agent);

	// Start the thread that gets the content of the page
	PageThread *pageThread = new PageThread(url, post, agent, pContext->GetFunctionById(params[1]), params[5]);
	threader->MakeThread(pageThread);

	return 1;
}


cell_t NativeDownloadFileUrl(IPluginContext *pContext, const cell_t *params) {
	char *url;
	char *localFile;

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
	char command[MAX_COMMAND_LENGTH + 1];

	smutils->FormatString(command, sizeof(command), pContext, params, 2);

	// Start the thread that executes the command
	CommandThread *commandThread = new CommandThread(command, pContext->GetFunctionById(params[1]), 0);
	threader->MakeThread(commandThread);

	return 1;
}


cell_t NativeRunThreadCommandWithData(IPluginContext *pContext, const cell_t *params) {
	char command[MAX_COMMAND_LENGTH + 1];

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
	g_pSM->BuildPath(Path_Game, ldir, sizeof(ldir), "%s", file);
	g_pSM->BuildPath(Path_Game, rdir, sizeof(rdir), "%s", folder);

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
		// Logging command
		g_pSM->LogMessage(myself, "Extracting archive: %s", command);

		// Start the thread that executes the command
		CommandThread *commandThread = new CommandThread(command, pContext->GetFunctionById(params[1]), params[4]);
		threader->MakeThread(commandThread);
	} else {
		g_pSM->LogError(myself, "ERROR: Coulnd't find 7-ZIP at %s to extract %s", zdir, ldir);
	}

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
	g_pSM->BuildPath(Path_Game, ldir, sizeof(ldir), "%s", file);
	g_pSM->BuildPath(Path_Game, rdir, sizeof(rdir), "%s", folder);

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
		// Logging command
		g_pSM->LogMessage(myself, "Extracting archive: %s", command);

		// Start the thread that executes the command
		CommandThread *commandThread = new CommandThread(command, pContext->GetFunctionById(params[1]), params[6]);
		threader->MakeThread(commandThread);
	} else {
		g_pSM->LogError(myself, "ERROR: Coulnd't find 7-ZIP at %s to compress %s", zdir, ldir);
	}

	return 1;
}


cell_t NativeRunCommand(IPluginContext *pContext, const cell_t *params) {
	char cmdString[MAX_COMMAND_LENGTH + 1];
	char buffer[MAX_RESULT_LENGTH + 1];
	char resultString[MAX_RESULT_LENGTH + 1];

	// Format the command string
	smutils->FormatString(cmdString, sizeof(cmdString), pContext, params, 3);

	// Prevent output to console
	if (strstr(cmdString, "2>&1") == NULL) {
		strcat(cmdString, " 2>&1");
	}

	// Execute the command
	FILE *command = PosixOpen(cmdString, "r");
	cell_t result = CMD_SUCCESS;

	// Was there an error?
	if (!command) {
		// Return the error
		pContext->StringToLocal(params[1], params[2], "ERROR: Couldn't execute the command!");
		return CMD_ERROR;
	}

	// Read the result
	strcpy(buffer, "");
	strcpy(resultString, "");
	while (fgets(buffer, sizeof(buffer), command) != NULL) {
		// More than MAX_RESULT_LENGTH?
		if (strlen(resultString) + strlen(buffer) >= size_t(params[2] - 1)) {
			// Only make the result full!
			strncat(resultString, buffer, (params[2] - strlen(resultString)) - 1);
			pContext->StringToLocal(params[1], params[2], resultString);

			break;
		}

		strcat(resultString, buffer);
	}


	if (strlen(resultString) == 0) {
		pContext->StringToLocal(params[1], params[2], "Empty reading result!");
		result = CMD_EMPTY;
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


cell_t NativeGetOS(IPluginContext *pContext, const cell_t *params) {
	// So what we have now :)
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

	// Get the string
	pContext->LocalToString(params[1], &str);
	if (strlen(str) == 0) {
		return 1;
	}

	// Calculate the MD5 hash
	MD5 md5 = MD5();
	md5.update(str, strlen(str));
	md5.finalize();

	// Save the MD5 hash to the plugins buffer
	pContext->StringToLocal(params[2], params[3], md5.hexdigest().c_str());
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
	std::ifstream file(fullFilePath, std::ifstream::binary | std::ifstream::ate);
	if (file.bad() || !file.is_open()) {
		pContext->StringToLocal(params[2], params[3], "");
		return 0;
	}

	// Get the size of the file and save the content to a var
	int size = (int)file.tellg();
	if (size < 1) {
		pContext->StringToLocal(params[2], params[3], "");
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
	pContext->StringToLocal(params[2], params[3], md5.hexdigest().c_str());

	return 1;
}


cell_t NativeGetStringCRC32(IPluginContext *pContext, const cell_t *params) {
	char *str;

	// Get the string
	pContext->LocalToString(params[1], &str);
	if (strlen(str) == 0) {
		return 1;
	}

	// Calculate the CRC32 hash
	char crc32[9];
	crc32ToHex(crc32buf(str, strlen(str)), crc32, sizeof(crc32));

	// Save the crc32 hash to the plugins buffer
	pContext->StringToLocal(params[2], params[3], crc32);
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
	std::ifstream file(fullFilePath, std::ifstream::binary | std::ifstream::ate);
	if (file.bad() || !file.is_open()) {
		pContext->StringToLocal(params[2], params[3], "");
		return 0;
	}

	// Get the size of the file and save the content to a var
	int size = (int)file.tellg();
	if (size < 1) {
		pContext->StringToLocal(params[2], params[3], "");
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
	pContext->StringToLocal(params[2], params[3], crc32);

	return 1;
}
