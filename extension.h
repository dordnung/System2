/**
 * -----------------------------------------------------
 * File        extension.h
 * Authors     David <popoklopsi> Ordnung, Sourcemod
 * License     GPLv3
 * Web         http://popoklopsi.de
 * -----------------------------------------------------
 * 
 * 
 * Copyright (C) 2013 David <popoklopsi> Ordnung, Sourcemod
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



#ifndef _INCLUDE_SOURCEMOD_EXTENSION_PROPER_H_
#define _INCLUDE_SOURCEMOD_EXTENSION_PROPER_H_







//// INCLUDSE


// c++
#include <stdio.h>
#include <string.h>



// Sourcemod
#include "smsdk_ext.h"


// We need curl
#include <curl/curl.h>







//// ENUMS


// OS List
enum OS
{
	OS_Unknown,
	OS_Windows,
	OS_Linux,
	OS_Mac
};


// Modes
enum MODES
{
	MODE_COMMAND,
	MODE_DOWNLOAD,
	MODE_UPLOAD,
	MODE_COPY
};







//// CLASSES


// System2 Extension Class
class System2Extension : public SDKExtension
{
public:
	virtual bool SDK_OnLoad(char *error, size_t maxlength, bool late);
	virtual void SDK_OnUnload();
};





// system Thread
class sysThread : public IThread
{
private:
	char Scommand[2048];
	IPluginFunction* function;

public:
	void RunThread(IThreadHandle *pThread);
	void OnTerminate(IThreadHandle *pThread, bool cancel) {}

	sysThread(char* command, IPluginFunction* callback) : IThread()
	{	
		strcpy(Scommand, command);
		function = callback;
	}

};




// FTP Thread
class FTPThread : public IThread
{
private:
	// Data we need
	char remoteFile[PLATFORM_MAX_PATH + 1];
	char localFile[PLATFORM_MAX_PATH + 1];
	char host[PLATFORM_MAX_PATH + 1];
	char username[128];
	char password[128];

	int port;
	MODES mode;

	IPluginFunction* function;

public:
	void RunThread(IThreadHandle *pThread);
	void OnTerminate(IThreadHandle *pThread, bool cancel) {}

	// Constructor
	FTPThread(char* rmFile, char* lcFile, char* url, char* user, char* pw, int por, IPluginFunction* callback, MODES mod) : IThread()
	{	
		strcpy(remoteFile, rmFile);
		strcpy(localFile, lcFile);
		strcpy(host, url);
		strcpy(username, user);
		strcpy(password, pw);

		port = por;
		mode = mod;
		function = callback;
	}

};





// Download Thread
class DownloadThread : public IThread
{
private:
	// Data we need
	char url[PLATFORM_MAX_PATH + 1];
	char localFile[PLATFORM_MAX_PATH + 1];

	IPluginFunction* function;

public:
	void RunThread(IThreadHandle *pThread);
	void OnTerminate(IThreadHandle *pThread, bool cancel) {}

	// Constructor
	DownloadThread(char* host, char* file, IPluginFunction* callback) : IThread()
	{	
		strcpy(url, host);
		strcpy(localFile, file);

		function = callback;
	}

};





// Copy Thread
class CopyThread : public IThread
{
private:
	// Data we need
	char file[PLATFORM_MAX_PATH + 1];
	char copyPath[PLATFORM_MAX_PATH + 1];

	IPluginFunction* function;

public:
	void RunThread(IThreadHandle *pThread);
	void OnTerminate(IThreadHandle *pThread, bool cancel) {}

	// Constructor
	CopyThread(char* lfile, char* path, IPluginFunction* callback) : IThread()
	{	
		strcpy(file, lfile);
		strcpy(copyPath, path);

		function = callback;
	}

};







//// STRUCTS


// Struct for result
typedef struct
{
public:
	// Chars
	char pResultString[4096];
	char pCommandString[2048];
	char curlError[CURL_ERROR_SIZE + 1];

	// finished?
	int finished;
	int count;
	int update;

	// doubles
	double dltotal;
	double dlnow;
	double ultotal;
	double ulnow;

	// Mode
	MODES mode;


	IPluginFunction* pFunc;
	cell_t result;

} ThreadReturn;




// Struct for Curl
struct FtpFile 
{
  const char *filename;
  FILE *stream;
};








//// METHODS


// Curl recieve
size_t file_write(void *buffer, size_t size, size_t nmemb, void *stream);
size_t ftp_upload(void *buffer, size_t size, size_t nmemb, void *stream);
int progress_updated(void *p, double dltotal, double dlnow, double ultotal, double ulnow);



// Natives
cell_t sys_CompressFile(IPluginContext *pContext, const cell_t *params);
cell_t sys_CopyFile(IPluginContext *pContext, const cell_t *params);
cell_t sys_ExtractArchive(IPluginContext *pContext, const cell_t *params);
cell_t sys_DownloadFileUrl(IPluginContext *pContext, const cell_t *params);
cell_t sys_DownloadFile(IPluginContext *pContext, const cell_t *params);
cell_t sys_UploadFile(IPluginContext *pContext, const cell_t *params);
cell_t sys_RunThreadCommand(IPluginContext *pContext, const cell_t *params);
cell_t sys_RunCommand(IPluginContext *pContext, const cell_t *params);
cell_t sys_GetGameDir(IPluginContext *pContext, const cell_t *params);
cell_t sys_GetOS(IPluginContext *pContext, const cell_t *params);




// Game Frame Hit
void OnGameFrameHit(bool simulating);



#endif