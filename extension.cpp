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

enum OS
{
	OS_Unknown,
    OS_Windows,
    OS_Linux,
    OS_Mac
};

bool System2Extension::SDK_OnLoad(char *error, size_t err_max, bool late)
{
	g_pShareSys->AddNatives(myself, system2_natives);
	g_pShareSys->RegisterLibrary(myself, "system2");

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
}

cell_t sys_RunThreadCommand(IPluginContext *pContext, const cell_t *params)
{
	char *command;
	sysThread* myThread;

	pContext->LocalToString(params[1], &command);

	myThread = new sysThread(command, pContext->GetFunctionById(params[2]));

	threader->MakeThread(myThread);
	
	return true;
}

cell_t sys_RunCommand(IPluginContext *pContext, const cell_t *params)
{
	char *command;

	pContext->LocalToString(params[1], &command);

	system(command);

	return 1;
}

cell_t sys_GetGameDir(IPluginContext *pContext, const cell_t *params)
{
	const char *GamePath = g_pSM->GetGamePath();

	pContext->StringToLocal(params[1], params[2], GamePath);

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
	system(Scommand);

	function->PushString(Scommand);
	function->Execute(NULL);
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