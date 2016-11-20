/**
 * -----------------------------------------------------
 * File        natives.h
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

#ifndef _NATIVES_H_
#define _NATIVES_H_

#include "extension.h"


cell_t NativeGetPage(IPluginContext *pContext, const cell_t *params);
cell_t NativeCompressFile(IPluginContext *pContext, const cell_t *params);
cell_t NativeCopyFile(IPluginContext *pContext, const cell_t *params);
cell_t NativeExtractArchive(IPluginContext *pContext, const cell_t *params);
cell_t NativeDownloadFileUrl(IPluginContext *pContext, const cell_t *params);
cell_t NativeDownloadFile(IPluginContext *pContext, const cell_t *params);
cell_t NativeUploadFile(IPluginContext *pContext, const cell_t *params);
cell_t NativeRunThreadCommand(IPluginContext *pContext, const cell_t *params);
cell_t NativeRunThreadCommandWithData(IPluginContext *pContext, const cell_t *params);
cell_t NativeRunCommand(IPluginContext *pContext, const cell_t *params);
cell_t NativeGetGameDir(IPluginContext *pContext, const cell_t *params);
cell_t NativeGetOS(IPluginContext *pContext, const cell_t *params);


const sp_nativeinfo_t system2_natives[] =
{
	// Backwards Compatiblity
	{ "RunThreadCommand", NativeRunThreadCommand },
	{ "RunCommand", NativeRunCommand },
	{ "GetGameDir", NativeGetGameDir },
	{ "GetOS", NativeGetOS },

	{ "System2_GetPage", NativeGetPage },
	{ "System2_CopyFile", NativeCopyFile },
	{ "System2_CompressFile", NativeCompressFile },
	{ "System2_ExtractArchive", NativeExtractArchive },
	{ "System2_DownloadFile", NativeDownloadFileUrl },
	{ "System2_DownloadFTPFile", NativeDownloadFile },
	{ "System2_UploadFTPFile", NativeUploadFile },
	{ "System2_RunThreadCommand", NativeRunThreadCommand },
	{ "System2_RunThreadCommandWithData", NativeRunThreadCommandWithData },
	{ "System2_RunCommand", NativeRunCommand },
	{ "System2_GetGameDir", NativeGetGameDir },
	{ "System2_GetOS", NativeGetOS },
	{ NULL, NULL },
};


#endif