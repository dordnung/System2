/**
 * vim: set ts=4 :
 * =============================================================================
 * SourceMod Sample Extension
 * Copyright (C) 2004-2008 AlliedModders LLC.  All rights reserved.
 * =============================================================================
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License, version 3.0, as published by the
 * Free Software Foundation.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * As a special exception, AlliedModders LLC gives you permission to link the
 * code of this program (as well as its derivative works) to "Half-Life 2," the
 * "Source Engine," the "SourcePawn JIT," and any Game MODs that run on software
 * by the Valve Corporation.  You must obey the GNU General Public License in
 * all respects for all other code used.  Additionally, AlliedModders LLC grants
 * this exception to all derivative works.  AlliedModders LLC defines further
 * exceptions, found in LICENSE.txt (as of this writing, version JULY-31-2007),
 * or <http://www.sourcemod.net/license.php>.
 *
 * Version: $Id$
 */

#include "extension.h"
#include "sh_vector.h"

#if defined __WIN32__ || _MSC_VER || __CYGWIN32__ || _Windows || __MSDOS__ || _WIN64 || _WIN32
#define PosixOpen _popen
#else
#define PosixOpen popen
#endif

#if defined __WIN32__ || _MSC_VER || __CYGWIN32__ || _Windows || __MSDOS__ || _WIN64 || _WIN32
#define PosixClose _pclose
#else
#define PosixClose pclose
#endif

CVector<PawnFuncThreadReturn *> vecPawnReturn;

IMutex * g_pPawnMutex;

enum OS
{
	OS_Unknown,
	OS_Windows,
	OS_Linux,
	OS_Mac
};

bool System2Extension::SDK_OnLoad(char *error, size_t err_max, bool late)
{
	sharesys->AddNatives(myself, system2_natives);
	sharesys->RegisterLibrary(myself, "system2");
	smutils->AddGameFrameHook(&OnGameFrameHit);
	g_pPawnMutex = threader->MakeMutex();
	return true;
}

bool System2Extension::QueryRunning(char* error, size_t maxlength)
{
	return true;
}

void System2Extension::SDK_OnAllLoaded()
{
}

void System2Extension::SDK_OnUnload()
{
	smutils->RemoveGameFrameHook(&OnGameFrameHit);
	g_pPawnMutex->DestroyThis();
}

void OnGameFrameHit(bool simulating)
{
	if (!g_pPawnMutex->TryLock())
	{
		return; /* This is totally fine. We'll hit the next frame. We don't need to dead lock. */
	}
	
	if (!vecPawnReturn.empty())
	{
		PawnFuncThreadReturn * pReturn = vecPawnReturn.back();
		vecPawnReturn.pop_back();
		
		IPluginFunction * pFunc = pReturn->pFunc;
		pFunc->PushString(pReturn->pCommandString);
		pFunc->PushString(pReturn->pResultString);
		pFunc->PushCell(pReturn->result);
		pFunc->Execute(NULL);
		
		delete pReturn;
	}
	
	g_pPawnMutex->Unlock();
}

cell_t sys_RunThreadCommand(IPluginContext *pContext, const cell_t *params)
{
	char command[2024];
	sysThread* myThread;

	smutils->FormatString(command, sizeof(command), pContext, params, 2);

	myThread = new sysThread(command, pContext->GetFunctionById(params[1]));

	threader->MakeThread(myThread);

	return 1;
}

cell_t sys_RunCommand(IPluginContext *pContext, const cell_t *params)
{
	char buffer[4096];

	smutils->FormatString(buffer, sizeof(buffer), pContext, params, 3);

	if (strstr(buffer, "2>&1") == NULL)
	{
		strcat(buffer, " 2>&1");
	}
	
	FILE* cmdFile = PosixOpen(buffer, "r");
	
	cell_t result = 0;

	if (!cmdFile)
	{
		pContext->StringToLocal(params[1], params[2], "ERROR Executing Command!");
		return 2;
	}

	if (fgets(buffer, sizeof(buffer), cmdFile) != NULL)
	{
		pContext->StringToLocal(params[1], params[2], buffer);
	}
	else
	{
		pContext->StringToLocal(params[1], params[2], "EMPTY Reading Result!");
		result = 1;
	}

	PosixClose(cmdFile);
	return result;
}

cell_t sys_GetGameDir(IPluginContext *pContext, const cell_t *params)
{
	pContext->StringToLocal(params[1], params[2], smutils->GetGamePath());

	return 1;
}

cell_t sys_GetOS(IPluginContext *pContext, const cell_t *params)
{
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

void sysThread::RunThread(IThreadHandle* pHandle)
{
	char buffer[4096];

	PawnFuncThreadReturn * pReturn = new PawnFuncThreadReturn;
	pReturn->pFunc = function;
	strcpy(pReturn->pCommandString, Scommand);
	
	if (strstr(Scommand, "2>&1") == NULL)
	{
		strcat(Scommand, " 2>&1");
	}
	
	FILE* cmdFile = PosixOpen(Scommand, "r");

	if (cmdFile)
	{
		if (fgets(pReturn->pResultString, sizeof(pReturn->pResultString), cmdFile) == NULL)
		{
			strcpy(pReturn->pResultString, "EMPTY Reading Result!");
			pReturn->result = 1;
		}
		else
		{
			pReturn->result = 0;
		}
		
		PosixClose(cmdFile);
	}
	else
	{
		strcpy(pReturn->pResultString, "ERROR Executing Command!");
		pReturn->result = 2;
	}
	
	g_pPawnMutex->Lock();
	vecPawnReturn.push_back(pReturn);
	g_pPawnMutex->Unlock();
}

void sysThread::OnTerminate(IThreadHandle* pHandle, bool cancel)
{
}

const sp_nativeinfo_t system2_natives[] = 
{
	{"RunThreadCommand",	sys_RunThreadCommand},
	{"RunCommand",			sys_RunCommand},
	{"GetGameDir",			sys_GetGameDir},
	{"GetOS",				sys_GetOS},
	{NULL,					NULL},
};

System2Extension g_System2Extension;

SMEXT_LINK(&g_System2Extension);