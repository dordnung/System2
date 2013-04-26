/**
 * -----------------------------------------------------
 * File        extension.cpp
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





//// INCLUDES


#include "extension.h"
#include "sh_vector.h"






//// DEFINES


// Some Pre Definies
#if defined __WIN32__ || _MSC_VER || __CYGWIN32__ || _Windows || __MSDOS__ || _WIN64 || _WIN32
	#define PosixOpen _popen
	#define PosixClose _pclose
	
	#pragma warning(disable: 4996)
#else
	#define PosixOpen popen
	#define PosixClose pclose
#endif








//// GLOBAL VARS


// Global variables
CVector<ThreadReturn *> vecPawnReturn;
IMutex* g_pPawnMutex;




// Native List
const sp_nativeinfo_t system2_natives[] = 
{
	// Backwards Compatiblity
	{"RunThreadCommand",			sys_RunThreadCommand},
	{"RunCommand",					sys_RunCommand},
	{"GetGameDir",					sys_GetGameDir},
	{"GetOS",						sys_GetOS},

	// New ones
	{"System2_CopyFile",			sys_CopyFile},
	{"System2_CompressFile",		sys_CompressFile},
	{"System2_ExtractArchive",		sys_ExtractArchive},
	{"System2_DownloadFile",		sys_DownloadFileUrl},
	{"System2_DownloadFTPFile",		sys_DownloadFile},
	{"System2_UploadFTPFile",		sys_UploadFile},
	{"System2_RunThreadCommand",	sys_RunThreadCommand},
	{"System2_RunCommand",			sys_RunCommand},
	{"System2_GetGameDir",			sys_GetGameDir},
	{"System2_GetOS",				sys_GetOS},
	{NULL,							NULL},
};







//// System2Extension


// Extension loaded
bool System2Extension::SDK_OnLoad(char *error, size_t err_max, bool late)
{
	// Add natives and register extension
	sharesys->AddNatives(myself, system2_natives);
	sharesys->RegisterLibrary(myself, "system2");



	// Thread stuff
	smutils->AddGameFrameHook(&OnGameFrameHit);

	g_pPawnMutex = threader->MakeMutex();


	// Loaded
	return true;
}



// Extension unloaded
void System2Extension::SDK_OnUnload()
{
	// Remove stuff
	smutils->RemoveGameFrameHook(&OnGameFrameHit);

	g_pPawnMutex->DestroyThis();
}









//// FRAME HIT


// Game Frame hit
void OnGameFrameHit(bool simulating)
{
	// Lock the mutex
	if (!g_pPawnMutex->TryLock())
	{
		// Couldn't lock
		return;
	}
	

	// No forwards to push?
	if (!vecPawnReturn.empty())
	{
		// Get last forward
		ThreadReturn *pReturn = vecPawnReturn.back();

		// Delete it
		vecPawnReturn.pop_back();

		if (pReturn != NULL)
		{
			// Execute it
			IPluginFunction* pFunc = pReturn->pFunc;

			if (pReturn->mode == MODE_COMMAND)
			{
				pFunc->PushString(pReturn->pCommandString);
				pFunc->PushString(pReturn->pResultString);
				pFunc->PushCell(pReturn->result);
			}
			else if (pReturn->mode != MODE_COPY)
			{
				pFunc->PushCell(pReturn->finished);
				pFunc->PushString(pReturn->curlError);

				pFunc->PushFloat((float)pReturn->dltotal);
				pFunc->PushFloat((float)pReturn->dlnow);
				pFunc->PushFloat((float)pReturn->ultotal);
				pFunc->PushFloat((float)pReturn->ulnow);
			}


			// Result for copy
			if (pReturn->mode == MODE_COPY)
			{
				pFunc->PushCell(pReturn->result);
			}


			pFunc->Execute(NULL);


			// Delete it from stack
			if (((pReturn->mode == MODE_COMMAND || pReturn->mode == MODE_COPY) || pReturn->finished == 1) && pReturn != NULL)
			{
				delete pReturn;
			}
		}
	}

	// Unlock mutex
	g_pPawnMutex->Unlock();
}











//// CURL STUFF


// Download progress
size_t file_write(void *buffer, size_t size, size_t nmemb, void *stream)
{
	// FTP Struct
	struct FtpFile *out=(struct FtpFile *)stream;

	// Not opened? Open it
	if(out && !out->stream) 
	{
		out->stream = fopen(out->filename, "wb");

		// Could we open the file?
		if (!out->stream)
		{
			return -1;
		}
	}

	// Write
	return fwrite(buffer, size, nmemb, out->stream);
}



// Upload progress
size_t ftp_upload(void *buffer, size_t size, size_t nmemb, void *stream)
{
	curl_off_t nread;

	size_t retcode = fread(buffer, size, nmemb, (FILE *)stream);

	nread = (curl_off_t)retcode;

	return retcode;
}





// Progress Updated
int progress_updated(void *p, double dltotal, double dlnow, double ultotal, double ulnow)
{
	// Get progress strucht
	ThreadReturn* myp = (ThreadReturn *)p;

	if (myp != NULL)
	{
		// Update data
		myp->dlnow = dlnow;
		myp->dltotal = dltotal;
		myp->ultotal = ultotal;
		myp->ulnow = ulnow;


		if ((myp->count % myp->update) == 0)
		{
			// Update status
			myp->finished = 0;


			// Lock mutex and write to vec
			if (g_pPawnMutex->TryLock())
			{
				vecPawnReturn.push_back(myp);

				g_pPawnMutex->Unlock();
			}
		}
	
		// Update count
		myp->count++;
	}


	return 0;
}












//// NATIVES


// Download a file from a ftp server
cell_t sys_DownloadFileUrl(IPluginContext *pContext, const cell_t *params)
{
	// chars
	char *localFile;
	char *url;

	// Get Chars
	pContext->LocalToString(params[2], &url);
	pContext->LocalToString(params[3], &localFile);


	// Start new thread
	DownloadThread* myThread = new DownloadThread(url, localFile, pContext->GetFunctionById(params[1]));
	threader->MakeThread(myThread);



	return 1;
}






// Download a file from a ftp server
cell_t sys_DownloadFile(IPluginContext *pContext, const cell_t *params)
{
	// chars
	char *remoteFile;
	char *localFile;
	char *host;
	char *username;
	char *password;

	// Get Chars
	pContext->LocalToString(params[2], &remoteFile);
	pContext->LocalToString(params[3], &localFile);
	pContext->LocalToString(params[4], &host);
	pContext->LocalToString(params[5], &username);
	pContext->LocalToString(params[6], &password);


	// Start new thread
	FTPThread* myThread = new FTPThread(remoteFile, localFile, host, username, password, params[7], pContext->GetFunctionById(params[1]), MODE_DOWNLOAD);
	threader->MakeThread(myThread);



	return 1;
}





// Upload a file to a ftp server
cell_t sys_UploadFile(IPluginContext *pContext, const cell_t *params)
{
	// chars
	char *remoteFile;
	char *localFile;
	char *host;
	char *username;
	char *password;


	// Get Chars
	pContext->LocalToString(params[2], &localFile);
	pContext->LocalToString(params[3], &remoteFile);
	pContext->LocalToString(params[4], &host);
	pContext->LocalToString(params[5], &username);
	pContext->LocalToString(params[6], &password);


	// Start new thread
	FTPThread* myThread = new FTPThread(remoteFile, localFile, host, username, password, params[7], pContext->GetFunctionById(params[1]), MODE_UPLOAD);
	threader->MakeThread(myThread);



	return 1;
}





// Copy a file
cell_t sys_CopyFile(IPluginContext *pContext, const cell_t *params)
{
	// chars
	char *file;
	char *path;


	// Get Chars
	pContext->LocalToString(params[2], &file);
	pContext->LocalToString(params[3], &path);


	// Start new thread
	CopyThread* myThread = new CopyThread(file, path, pContext->GetFunctionById(params[1]));
	threader->MakeThread(myThread);



	return 1;
}






// Run a system command, threaded
cell_t sys_RunThreadCommand(IPluginContext *pContext, const cell_t *params)
{
	// command
	char command[2048];

	// Get command
	smutils->FormatString(command, sizeof(command), pContext, params, 2);


	// Start new thread
	sysThread* myThread = new sysThread(command, pContext->GetFunctionById(params[1]));
	threader->MakeThread(myThread);



	return 1;
}






// Extracts a lot of archives
cell_t sys_ExtractArchive(IPluginContext *pContext, const cell_t *params)
{
	// Files
	char *file;
	char *folder;

	// Dir to 7z
	char zdir[PLATFORM_MAX_PATH + 1];
	char ldir[PLATFORM_MAX_PATH + 1];
	char rdir[PLATFORM_MAX_PATH + 1];
	char command[(PLATFORM_MAX_PATH * 3) + 10];

	FILE* testExist;



	// Get Paths
	pContext->LocalToString(params[2], &file);
	pContext->LocalToString(params[3], &folder);


	// Build Paths
	#if defined _WIN32
		g_pSM->BuildPath(Path_SM, zdir, sizeof(zdir), "data/system2/7z.exe");
	#else
		g_pSM->BuildPath(Path_SM, zdir, sizeof(zdir), "data/system2/7z");
	#endif
	g_pSM->BuildPath(Path_Game, ldir, sizeof(ldir), "%s", file);
	g_pSM->BuildPath(Path_Game, rdir, sizeof(rdir), "%s", folder);


	if ((testExist = fopen(zdir, "rb")) != NULL)
	{
		// Close
		fclose(testExist);


		// Make command
		#if defined _WIN32
			sprintf(command, "\"\"%s\" x \"%s\" -o\"%s\" -mmt -aoa\"", zdir, ldir, rdir);
		#else
			sprintf(command, "\"%s\" x \"%s\" -o\"%s\" -mmt -aoa", zdir, ldir, rdir);
		#endif


		// Start new thread
		sysThread* myThread = new sysThread(command, pContext->GetFunctionById(params[1]));
		threader->MakeThread(myThread);
	}
	else
	{
		g_pSM->LogError(myself, "Attention: Coulnd't find %s to extract %s", zdir, ldir);
	}

	return 1;
}






// Compresses a file to a archive
cell_t sys_CompressFile(IPluginContext *pContext, const cell_t *params)
{
	// Files
	char *file;
	char *folder;

	char archive[12];
	char level[6];

	// Dir to 7z
	char zdir[PLATFORM_MAX_PATH + 1];
	char ldir[PLATFORM_MAX_PATH + 1];
	char rdir[PLATFORM_MAX_PATH + 1];
	char command[(PLATFORM_MAX_PATH * 3) + 30];

	FILE* testExist;



	// Get Paths
	pContext->LocalToString(params[2], &file);
	pContext->LocalToString(params[3], &folder);


	// Build Paths
	#if defined _WIN32
		g_pSM->BuildPath(Path_SM, zdir, sizeof(zdir), "data/system2/7z.exe");
	#else
		g_pSM->BuildPath(Path_SM, zdir, sizeof(zdir), "data/system2/7z");
	#endif
	g_pSM->BuildPath(Path_Game, ldir, sizeof(ldir), "%s", file);
	g_pSM->BuildPath(Path_Game, rdir, sizeof(rdir), "%s", folder);



	// Get Compress level
	switch(params[5])
	{
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




	// Get Archive
	switch(params[4])
	{
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
	if ((testExist = fopen(zdir, "rb")) != NULL)
	{
		// Close
		fclose(testExist);


		// Make command
		#if defined _WIN32
			sprintf(command, "\"\"%s\" a %s \"%s\" \"%s\" -mmt %s\"", zdir, archive, rdir, ldir, level);
		#else
			sprintf(command, "\"%s\" a %s \"%s\" \"%s\" -mmt %s", zdir, archive, rdir, ldir, level);
		#endif


		// Start new thread
		sysThread* myThread = new sysThread(command, pContext->GetFunctionById(params[1]));
		threader->MakeThread(myThread);
	}
	else
	{
		g_pSM->LogError(myself, "Attention: Coulnd't find %s to compress %s", zdir, ldir);
	}

	return 1;
}








// Run a system command, threaded
cell_t sys_RunCommand(IPluginContext *pContext, const cell_t *params)
{
	// buffer
	char buffer[4096];
	char buffer2[4096];
	bool found = false;

	// Format string
	smutils->FormatString(buffer, sizeof(buffer), pContext, params, 3);


	// Output linker
	if (strstr(buffer, "2>&1") == NULL)
	{
		strcat(buffer, " 2>&1");
	}
	

	// Execute
	FILE* cmdFile = PosixOpen(buffer, "r");
	cell_t result = 0;


	// Empty buffer
	strcpy(buffer, "");


	// Error?
	if (!cmdFile)
	{
		// Return error
		pContext->StringToLocal(params[1], params[2], "ERROR Executing Command!");

		return 2;
	}
	

	// Empty result?
	while (fgets(buffer2, sizeof(buffer2), cmdFile) != NULL)
	{
		found = true;
		strcat(buffer, buffer2);
	}


	if (!found)
	{
		pContext->StringToLocal(params[1], params[2], "EMPTY Reading Result!");

		result = 1;
	}
	else
	{
		pContext->StringToLocal(params[1], params[2], buffer);
	}

	// Close Posix
	PosixClose(cmdFile);


	// Return result
	return result;
}






// Get the game dir
cell_t sys_GetGameDir(IPluginContext *pContext, const cell_t *params)
{
	// Save to string
	pContext->StringToLocal(params[1], params[2], smutils->GetGamePath());

	return 1;
}






// Get the os
cell_t sys_GetOS(IPluginContext *pContext, const cell_t *params)
{
	// So what we have now :)
	#if defined __WIN32__ || _MSC_VER || __CYGWIN32__ || _Windows || __MSDOS__ || _WIN64 || _WIN32
		return OS_Windows;
	#elif defined __unix__ || __linux__ || __unix
		return OS_Linux;
	#elif defined __APPLE__ || __darwin__
		return OS_Mac;
	#else
		return OS_Unknown;
	#endif
}









//// THREAD FOR SYSTEM COMMANDS


// Thread executed
void sysThread::RunThread(IThreadHandle *pHandle)
{
	char buffer[4096];
	bool found = false;


	// Get func 
	ThreadReturn *pReturn = new ThreadReturn;
	
	// Save to func
	pReturn->pFunc = function;
	pReturn->mode = MODE_COMMAND;
	pReturn->finished = 1;
	pReturn->result = 0;

	strcpy(pReturn->pCommandString, Scommand);
	

	// Add linking
	if (strstr(Scommand, "2>&1") == NULL)
	{
		strcat(Scommand, " 2>&1");
	}
	

	// Ecevute
	FILE* cmdFile = PosixOpen(Scommand, "r");


	// Error?
	if (cmdFile)
	{
		// Empty result?
		while (fgets(buffer, sizeof(buffer), cmdFile) != NULL)
		{
			found = true;
			strcat(pReturn->pResultString, buffer);
		}

		if (!found)
		{
			strcpy(pReturn->pResultString, "EMPTY Reading Result!");

			pReturn->result = 1;
		}

		
		// Close
		PosixClose(cmdFile);
	}
	else
	{
		// Error occured
		strcpy(pReturn->pResultString, "ERROR Executing Command!");

		pReturn->result = 2;
	}
	

	// Lock mutex and write to vec
	g_pPawnMutex->Lock();

	vecPawnReturn.push_back(pReturn);

	g_pPawnMutex->Unlock();
}












//// DOWNLOAD THREAD


// File Download Thread executed
void DownloadThread::RunThread(IThreadHandle *pHandle)
{
	// Full Path
	char fullLocalPath[PLATFORM_MAX_PATH + 1];


	// Get func 
	ThreadReturn *pReturn = new ThreadReturn;

	// Save to func
	pReturn->pFunc = function;
	pReturn->mode = MODE_DOWNLOAD;
	pReturn->count = 0;
	pReturn->finished = 0;
	pReturn->update = 50;

	strcpy(pReturn->curlError, "");



	// Get Full Path
	g_pSM->BuildPath(Path_Game, fullLocalPath, sizeof(fullLocalPath), "%s", localFile);


	// Info
	struct FtpFile ftpfile=
	{
		fullLocalPath,
		NULL
	};


	// Curl
	CURL *curl;
	CURLcode res;


	// Init. Curl
	curl = curl_easy_init();


	if (curl)
	{
		// Set up Curl
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, pReturn->curlError);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, file_write);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ftpfile);
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
		curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, progress_updated);
		curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, pReturn);

		// Perform
		res = curl_easy_perform(curl);

		// Good result?
		pReturn->result = (int)res;

		// Clean
		curl_easy_cleanup(curl);
	}


	// Close file if opened
	if (ftpfile.stream != NULL)
	{
		fclose(ftpfile.stream);
	}


	// We are finished
	pReturn->finished = 1;


	// Lock mutex and write to vec
	g_pPawnMutex->Lock();

	vecPawnReturn.push_back(pReturn);

	g_pPawnMutex->Unlock();
}









//// FTP THREAD


// FTP Thread executed
void FTPThread::RunThread(IThreadHandle *pHandle)
{
	// For Upload
	FILE *localReadFile = NULL;
	curl_off_t fsize;

	char fullLocalPath[PLATFORM_MAX_PATH + 1];
	char fullHost[PLATFORM_MAX_PATH + 20];




	// Get func 
	ThreadReturn *pReturn = new ThreadReturn;

	// Save to func
	pReturn->pFunc = function;
	pReturn->mode = mode;
	pReturn->finished = 0;
	pReturn->count = 0;
	pReturn->update = 80;

	strcpy(pReturn->curlError, "");


	// Curl
	CURL *curl;
	CURLcode res;



	// Get Full Path
	g_pSM->BuildPath(Path_Game, fullLocalPath, sizeof(fullLocalPath), "%s", localFile);


	struct FtpFile ftpfile=
	{
		fullLocalPath,
		NULL
	};



	// Open File
	if (mode == MODE_UPLOAD)
	{
		pReturn->update = 20;
		localReadFile = fopen(fullLocalPath, "rb");
	}


	// Uploading and file exists?
	if (mode != MODE_UPLOAD || localReadFile != NULL)
	{
		// Init. Curl
		curl_global_init(CURL_GLOBAL_DEFAULT);
		curl = curl_easy_init();


		if (curl)
		{
			// Get hole URL
			sprintf(fullHost, "ftp://%s/%s", host, remoteFile);



			// Set up Curl
			curl_easy_setopt(curl, CURLOPT_URL, fullHost);
			curl_easy_setopt(curl, CURLOPT_PORT, port);
			curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, pReturn->curlError);
			curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
			curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, progress_updated);
			curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, pReturn);


			// Login?
			if (strlen(username) > 1)
			{
				char loginData[256];

				sprintf(loginData, "%s:%s", username, password);

				curl_easy_setopt(curl, CURLOPT_USERPWD, loginData);
			}


			// Upload stuff
			if (mode == MODE_UPLOAD)
			{
				// Get size
				fseek(localReadFile, 0L, SEEK_END);
				fsize = (curl_off_t)ftell(localReadFile);
				fseek(localReadFile, 0L, SEEK_SET);

				curl_easy_setopt(curl, CURLOPT_READFUNCTION, ftp_upload);
				curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
				curl_easy_setopt(curl, CURLOPT_FTP_CREATE_MISSING_DIRS, CURLFTP_CREATE_DIR_RETRY);
				curl_easy_setopt(curl, CURLOPT_READDATA, localReadFile);
				curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)fsize);

			}
			else
			{
				curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, file_write);
				curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ftpfile);
			}




			// Perform
			res = curl_easy_perform(curl);


			// Good result?
			pReturn->result = (int)res;


			// Clean
			curl_easy_cleanup(curl);
		}




		// Close file if opened
		if (ftpfile.stream != NULL)
		{
			fclose(ftpfile.stream);
		}


		// Close Read File
		if (localReadFile != NULL)
		{
			fclose(localReadFile);
		}



		// Now clean global curl
		curl_global_cleanup();
	}



	// We are finished
	pReturn->finished = 1;



	// Lock mutex and write to vec
	while (!g_pPawnMutex->TryLock())
	{
		// We need this callback!
	}
	

	if (pReturn != NULL)
	{
		vecPawnReturn.push_back(pReturn);
	}

	g_pPawnMutex->Unlock();
}









//// COPY THREAD


// Copy Thread executed
void CopyThread::RunThread(IThreadHandle *pHandle)
{
	size_t len = 0 ;
	char buffer[BUFSIZ] = {'\0'} ;

	// Full Path
	char fullFilePath[PLATFORM_MAX_PATH + 1];
	char fullCopyPath[PLATFORM_MAX_PATH + 1];



	// Get func 
	ThreadReturn *pReturn = new ThreadReturn;

	// Save to func
	pReturn->pFunc = function;
	pReturn->mode = MODE_COPY;
	pReturn->finished = 1;
	strcpy(pReturn->curlError, "");



	// Get Full Path
	g_pSM->BuildPath(Path_Game, fullFilePath, sizeof(fullFilePath), "%s", file);
	g_pSM->BuildPath(Path_Game, fullCopyPath, sizeof(fullCopyPath), "%s", copyPath);




	// Open both files
	FILE* in = fopen(fullFilePath, "rb") ;
	FILE* out = fopen(fullCopyPath, "wb") ;

	if (in == NULL || out == NULL)
	{
		pReturn->result = 0;
	}
	else
	{
		while ((len = fread(buffer, BUFSIZ, 1, in)) > 0)
		{
			fwrite(buffer, BUFSIZ, 1, out);
		}

		pReturn->result = 1;
	}


	// Close files
	if (in != NULL)
	{
		fclose(in);
	}

	if (out != NULL)
	{
		fclose(out);
	}


	// Lock mutex and write to vec
	g_pPawnMutex->Lock();

	vecPawnReturn.push_back(pReturn);

	g_pPawnMutex->Unlock();
}







//// LINKING


// Linking extension
System2Extension g_System2Extension;

SMEXT_LINK(&g_System2Extension);