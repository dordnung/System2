/**
 * -----------------------------------------------------
 * File        LegacyNatives.h
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

#ifndef _SYSTEM2_LEGACY_NATIVES_H_
#define _SYSTEM2_LEGACY_NATIVES_H_

#include "extension.h"


cell_t NativeGetPage(IPluginContext *pContext, const cell_t *params);
cell_t NativeDownloadFileUrl(IPluginContext *pContext, const cell_t *params);
cell_t NativeDownloadFile(IPluginContext *pContext, const cell_t *params);
cell_t NativeUploadFile(IPluginContext *pContext, const cell_t *params);
cell_t NativeCompressFile(IPluginContext *pContext, const cell_t *params);
cell_t NativeExtractArchive(IPluginContext *pContext, const cell_t *params);
cell_t NativeRunThreadCommand(IPluginContext *pContext, const cell_t *params);
cell_t NativeRunThreadCommandWithData(IPluginContext *pContext, const cell_t *params);
cell_t NativeRunCommand(IPluginContext *pContext, const cell_t *params);


const sp_nativeinfo_t system2_legacy_natives[] =
{
    { "System2_GetPage", NativeGetPage },
    { "System2_DownloadFile", NativeDownloadFileUrl },
    { "System2_DownloadFTPFile", NativeDownloadFile },
    { "System2_UploadFTPFile", NativeUploadFile },
    { "System2_CompressFile", NativeCompressFile },
    { "System2_ExtractArchive", NativeExtractArchive },
    { "System2_RunThreadCommand", NativeRunThreadCommand },
    { "System2_RunThreadCommandWithData", NativeRunThreadCommandWithData },
    { "System2_RunCommand", NativeRunCommand },
    { NULL, NULL },
};

#endif