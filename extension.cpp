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
	char command[2024];
	sysThread* myThread;

	g_pSM->FormatString(command, sizeof(command), pContext, params, 2);

	myThread = new sysThread(command, pContext->GetFunctionById(params[1]));

	threader->MakeThread(myThread);

	return 1;
}

cell_t sys_RunCommand(IPluginContext *pContext, const cell_t *params)
{
	char command[2060];
	char buffer[4096];
	string s_command = command;

	g_pSM->FormatString(command, sizeof(command), pContext, params, 3);

	if (s_command.find("2>&1") == string::npos)
		strcat(command, "2>&1");

	#if defined __WIN32__ || _MSC_VER || __CYGWIN32__ || _Windows || __MSDOS__ || _WIN64 || _WIN32
		FILE* cmdFile = _popen(command, "r");
	#else
		FILE* cmdFile = popen(command, "r");
	#endif

	if (!cmdFile)
	{
		pContext->StringToLocal(params[1], params[2], "ERROR Executing Command!");

		return 2;
	}

	if (fgets(buffer, sizeof(buffer), cmdFile) != NULL)
	{
		pContext->StringToLocal(params[1], params[2], buffer);

		#if defined __WIN32__ || _MSC_VER || __CYGWIN32__ || _Windows || __MSDOS__ || _WIN64 || _WIN32
			_pclose(cmdFile);
		#else
			pclose(cmdFile);
		#endif

		return 0;
	}
	else
	{
		pContext->StringToLocal(params[1], params[2], "EMPTY Reading Result!");

		#if defined __WIN32__ || _MSC_VER || __CYGWIN32__ || _Windows || __MSDOS__ || _WIN64 || _WIN32
			_pclose(cmdFile);
		#else
			pclose(cmdFile);
		#endif

		return 1;
	}
}

cell_t sys_GetGameDir(IPluginContext *pContext, const cell_t *params)
{
	pContext->StringToLocal(params[1], params[2], g_pSM->GetGamePath());

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
	string s_command = Scommand;

	if (s_command.find("2>&1") == string::npos)
		strcat(Scommand, "2>&1");

	function->PushString(Scommand);

	#if defined __WIN32__ || _MSC_VER || __CYGWIN32__ || _Windows || __MSDOS__ || _WIN64 || _WIN32
		FILE* cmdFile = _popen(Scommand, "r");
	#else
		FILE* cmdFile = popen(Scommand, "r");
	#endif

	if (!cmdFile)
	{
		function->PushString("ERROR Executing Command!");
		function->PushCell(2);
	}
	else
	{
		if (fgets(buffer, sizeof(buffer), cmdFile) != NULL)
		{
			function->PushString(buffer);
			function->PushCell(0);

			#if defined __WIN32__ || _MSC_VER || __CYGWIN32__ || _Windows || __MSDOS__ || _WIN64 || _WIN32
				_pclose(cmdFile);
			#else
				pclose(cmdFile);
			#endif
		}
		else
		{
			function->PushString("EMPTY Reading Result!");
			function->PushCell(1);

			#if defined __WIN32__ || _MSC_VER || __CYGWIN32__ || _Windows || __MSDOS__ || _WIN64 || _WIN32
				_pclose(cmdFile);
			#else
				pclose(cmdFile);
			#endif
		}
	}

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