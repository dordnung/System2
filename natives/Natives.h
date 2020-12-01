/**
 * -----------------------------------------------------
 * File        Natives.h
 * Authors     David Ordnung
 * License     GPLv3
 * Web         http://dordnung.de
 * -----------------------------------------------------
 *
 * Copyright (C) 2013-2020 David Ordnung
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

#ifndef _SYSTEM2_NATIVES_H_
#define _SYSTEM2_NATIVES_H_

#include "extension.h"

cell_t NativeRequest_SetURL(IPluginContext* pContext, const cell_t* params);
cell_t NativeRequest_GetURL(IPluginContext* pContext, const cell_t* params);
cell_t NativeRequest_SetPort(IPluginContext* pContext, const cell_t* params);
cell_t NativeRequest_GetPort(IPluginContext* pContext, const cell_t* params);
cell_t NativeRequest_SetOutputFile(IPluginContext* pContext, const cell_t* params);
cell_t NativeRequest_GetOutputFile(IPluginContext* pContext, const cell_t* params);
cell_t NativeRequest_SetVerifySSL(IPluginContext* pContext, const cell_t* params);
cell_t NativeRequest_GetVerifySSL(IPluginContext* pContext, const cell_t* params);
cell_t NativeRequest_SetProxy(IPluginContext* pContext, const cell_t* params);
cell_t NativeRequest_SetProxyAuthentication(IPluginContext* pContext, const cell_t* params);
cell_t NativeRequest_GetTimeout(IPluginContext* pContext, const cell_t* params);
cell_t NativeRequest_SetTimeout(IPluginContext* pContext, const cell_t* params);
cell_t NativeRequest_GetAnyData(IPluginContext* pContext, const cell_t* params);
cell_t NativeRequest_SetAnyData(IPluginContext* pContext, const cell_t* params);
cell_t NativeRequest_SetMaxSendSpeed(IPluginContext* pContext, const cell_t* params);
cell_t NativeRequest_SetMaxRecvSpeed(IPluginContext* pContext, const cell_t* params);

cell_t NativeHTTPRequest_HTTPRequest(IPluginContext* pContext, const cell_t* params);
cell_t NativeHTTPRequest_SetProgressCallback(IPluginContext* pContext, const cell_t* params);
cell_t NativeHTTPRequest_SetData(IPluginContext* pContext, const cell_t* params);
cell_t NativeHTTPRequest_GetData(IPluginContext* pContext, const cell_t* params);
cell_t NativeHTTPRequest_SetHeader(IPluginContext* pContext, const cell_t* params);
cell_t NativeHTTPRequest_GetHeader(IPluginContext* pContext, const cell_t* params);
cell_t NativeHTTPRequest_GetHeaderName(IPluginContext* pContext, const cell_t* params);
cell_t NativeHTTPRequest_GetHeaders(IPluginContext* pContext, const cell_t* params);
cell_t NativeHTTPRequest_SetUserAgent(IPluginContext* pContext, const cell_t* params);
cell_t NativeHTTPRequest_SetBasicAuthentication(IPluginContext* pContext, const cell_t* params);
cell_t NativeHTTPRequest_GET(IPluginContext* pContext, const cell_t* params);
cell_t NativeHTTPRequest_POST(IPluginContext* pContext, const cell_t* params);
cell_t NativeHTTPRequest_PUT(IPluginContext* pContext, const cell_t* params);
cell_t NativeHTTPRequest_PATCH(IPluginContext* pContext, const cell_t* params);
cell_t NativeHTTPRequest_DELETE(IPluginContext* pContext, const cell_t* params);
cell_t NativeHTTPRequest_HEAD(IPluginContext* pContext, const cell_t* params);
cell_t NativeHTTPRequest_GetFollowRedirects(IPluginContext* pContext, const cell_t* params);
cell_t NativeHTTPRequest_SetFollowRedirects(IPluginContext* pContext, const cell_t* params);

cell_t NativeFTPRequest_FTPRequest(IPluginContext* pContext, const cell_t* params);
cell_t NativeFTPRequest_SetProgressCallback(IPluginContext* pContext, const cell_t* params);
cell_t NativeFTPRequest_SetAuthentication(IPluginContext* pContext, const cell_t* params);
cell_t NativeFTPRequest_SetInputFile(IPluginContext* pContext, const cell_t* params);
cell_t NativeFTPRequest_GetInputFile(IPluginContext* pContext, const cell_t* params);
cell_t NativeFTPRequest_StartRequest(IPluginContext* pContext, const cell_t* params);
cell_t NativeFTPRequest_GetAppendToFile(IPluginContext* pContext, const cell_t* params);
cell_t NativeFTPRequest_SetAppendToFile(IPluginContext* pContext, const cell_t* params);
cell_t NativeFTPRequest_GetCreateMissingDirs(IPluginContext* pContext, const cell_t* params);
cell_t NativeFTPRequest_SetCreateMissingDirs(IPluginContext* pContext, const cell_t* params);
cell_t NativeFTPRequest_GetListFilenamesOnly(IPluginContext* pContext, const cell_t* params);
cell_t NativeFTPRequest_SetListFilenamesOnly(IPluginContext* pContext, const cell_t* params);

cell_t NativeResponse_GetLastURL(IPluginContext* pContext, const cell_t* params);
cell_t NativeResponse_GetContent(IPluginContext* pContext, const cell_t* params);
cell_t NativeResponse_GetContentLength(IPluginContext* pContext, const cell_t* params);
cell_t NativeResponse_GetStatusCode(IPluginContext* pContext, const cell_t* params);
cell_t NativeResponse_GetTotalTime(IPluginContext* pContext, const cell_t* params);
cell_t NativeResponse_GetDownloadSize(IPluginContext* pContext, const cell_t* params);
cell_t NativeResponse_GetUploadSize(IPluginContext* pContext, const cell_t* params);
cell_t NativeResponse_GetDownloadSpeed(IPluginContext* pContext, const cell_t* params);
cell_t NativeResponse_GetUploadSpeed(IPluginContext* pContext, const cell_t* params);

cell_t NativeHTTPResponse_GetContentType(IPluginContext* pContext, const cell_t* params);
cell_t NativeHTTPResponse_GetHeader(IPluginContext* pContext, const cell_t* params);
cell_t NativeHTTPResponse_GetHeaderName(IPluginContext* pContext, const cell_t* params);
cell_t NativeHTTPResponse_GetHeaders(IPluginContext* pContext, const cell_t* params);
cell_t NativeHTTPResponse_GetHTTPVersion(IPluginContext* pContext, const cell_t* params);

cell_t NativeURLEncode(IPluginContext* pContext, const cell_t* params);
cell_t NativeURLDecode(IPluginContext* pContext, const cell_t* params);

cell_t NativeCopyFile(IPluginContext* pContext, const cell_t* params);

cell_t NativeCheck7ZIP(IPluginContext* pContext, const cell_t* params);
cell_t NativeCompress(IPluginContext* pContext, const cell_t* params);
cell_t NativeExtract(IPluginContext* pContext, const cell_t* params);
bool Get7ZIPExecutable(bool force32Bit, std::string& binDir);

cell_t NativeExecuteThreaded(IPluginContext* pContext, const cell_t* params);
cell_t NativeExecuteFormattedThreaded(IPluginContext* pContext, const cell_t* params);
cell_t NativeExecuteOutput_GetOutput(IPluginContext* pContext, const cell_t* params);
cell_t NativeExecuteOutput_GetLength(IPluginContext* pContext, const cell_t* params);
cell_t NativeExecuteOutput_GetExitStatus(IPluginContext* pContext, const cell_t* params);

cell_t NativeExecute(IPluginContext* pContext, const cell_t* params);
cell_t NativeExecuteFormatted(IPluginContext* pContext, const cell_t* params);
cell_t NativeExecuteCommand(std::string command, IPluginContext* pContext, const cell_t* params);

cell_t NativeGetGameDir(IPluginContext* pContext, const cell_t* params);
cell_t NativeGetOS(IPluginContext* pContext, const cell_t* params);

cell_t NativeGetStringMD5(IPluginContext* pContext, const cell_t* params);
cell_t NativeGetFileMD5(IPluginContext* pContext, const cell_t* params);
cell_t NativeGetStringCRC32(IPluginContext* pContext, const cell_t* params);
cell_t NativeGetFileCRC32(IPluginContext* pContext, const cell_t* params);

const sp_nativeinfo_t system2_natives[] =
{
    { "System2Request.SetURL", NativeRequest_SetURL },
    { "System2Request.GetURL", NativeRequest_GetURL },
    { "System2Request.SetPort", NativeRequest_SetPort },
    { "System2Request.GetPort", NativeRequest_GetPort },
    { "System2Request.SetOutputFile", NativeRequest_SetOutputFile },
    { "System2Request.GetOutputFile", NativeRequest_GetOutputFile },
    { "System2Request.SetVerifySSL", NativeRequest_SetVerifySSL },
    { "System2Request.GetVerifySSL", NativeRequest_GetVerifySSL },
    { "System2Request.SetProxy", NativeRequest_SetProxy },
    { "System2Request.SetProxyAuthentication", NativeRequest_SetProxyAuthentication },
    { "System2Request.Timeout.get", NativeRequest_GetTimeout },
    { "System2Request.Timeout.set", NativeRequest_SetTimeout },
    { "System2Request.Any.get", NativeRequest_GetAnyData },
    { "System2Request.Any.set", NativeRequest_SetAnyData },
    { "System2Request.MaxSendSpeed.set", NativeRequest_SetMaxSendSpeed },
    { "System2Request.MaxRecvSpeed.set", NativeRequest_SetMaxRecvSpeed },

    { "System2HTTPRequest.System2HTTPRequest", NativeHTTPRequest_HTTPRequest },
    { "System2HTTPRequest.SetProgressCallback", NativeHTTPRequest_SetProgressCallback },
    { "System2HTTPRequest.SetData", NativeHTTPRequest_SetData },
    { "System2HTTPRequest.GetData", NativeHTTPRequest_GetData },
    { "System2HTTPRequest.SetHeader", NativeHTTPRequest_SetHeader },
    { "System2HTTPRequest.GetHeader", NativeHTTPRequest_GetHeader },
    { "System2HTTPRequest.GetHeaderName", NativeHTTPRequest_GetHeaderName },
    { "System2HTTPRequest.SetUserAgent", NativeHTTPRequest_SetUserAgent },
    { "System2HTTPRequest.SetBasicAuthentication", NativeHTTPRequest_SetBasicAuthentication },
    { "System2HTTPRequest.GET", NativeHTTPRequest_GET },
    { "System2HTTPRequest.POST", NativeHTTPRequest_POST },
    { "System2HTTPRequest.PUT", NativeHTTPRequest_PUT },
    { "System2HTTPRequest.PATCH", NativeHTTPRequest_PATCH },
    { "System2HTTPRequest.DELETE", NativeHTTPRequest_DELETE },
    { "System2HTTPRequest.HEAD", NativeHTTPRequest_HEAD },
    { "System2HTTPRequest.FollowRedirects.get", NativeHTTPRequest_GetFollowRedirects },
    { "System2HTTPRequest.FollowRedirects.set", NativeHTTPRequest_SetFollowRedirects },
    { "System2HTTPRequest.Headers.get", NativeHTTPRequest_GetHeaders },

    { "System2FTPRequest.System2FTPRequest", NativeFTPRequest_FTPRequest },
    { "System2FTPRequest.SetProgressCallback", NativeFTPRequest_SetProgressCallback },
    { "System2FTPRequest.SetAuthentication", NativeFTPRequest_SetAuthentication },
    { "System2FTPRequest.SetInputFile", NativeFTPRequest_SetInputFile },
    { "System2FTPRequest.GetInputFile", NativeFTPRequest_GetInputFile },
    { "System2FTPRequest.StartRequest", NativeFTPRequest_StartRequest },
    { "System2FTPRequest.AppendToFile.get", NativeFTPRequest_GetAppendToFile },
    { "System2FTPRequest.AppendToFile.set", NativeFTPRequest_SetAppendToFile },
    { "System2FTPRequest.CreateMissingDirs.get", NativeFTPRequest_GetCreateMissingDirs },
    { "System2FTPRequest.CreateMissingDirs.set", NativeFTPRequest_SetCreateMissingDirs },
    { "System2FTPRequest.ListFilenamesOnly.get", NativeFTPRequest_GetListFilenamesOnly },
    { "System2FTPRequest.ListFilenamesOnly.set", NativeFTPRequest_SetListFilenamesOnly },

    { "System2Response.GetLastURL", NativeResponse_GetLastURL },
    { "System2Response.GetContent", NativeResponse_GetContent },
    { "System2Response.ContentLength.get", NativeResponse_GetContentLength },
    { "System2Response.StatusCode.get", NativeResponse_GetStatusCode },
    { "System2Response.TotalTime.get", NativeResponse_GetTotalTime },
    { "System2Response.DownloadSize.get", NativeResponse_GetDownloadSize },
    { "System2Response.UploadSize.get", NativeResponse_GetUploadSize },
    { "System2Response.DownloadSpeed.get", NativeResponse_GetDownloadSpeed },
    { "System2Response.UploadSpeed.get", NativeResponse_GetUploadSpeed },

    { "System2HTTPResponse.GetContentType", NativeHTTPResponse_GetContentType },
    { "System2HTTPResponse.GetHeader", NativeHTTPResponse_GetHeader },
    { "System2HTTPResponse.GetHeaderName", NativeHTTPResponse_GetHeaderName },
    { "System2HTTPResponse.HTTPVersion.get", NativeHTTPResponse_GetHTTPVersion },
    { "System2HTTPResponse.Headers.get", NativeHTTPResponse_GetHeaders },

    { "System2_URLEncode", NativeURLEncode },
    { "System2_URLDecode", NativeURLDecode },

    { "System2_CopyFile", NativeCopyFile },

    { "System2_Check7ZIP", NativeCheck7ZIP },
    { "System2_Compress", NativeCompress },
    { "System2_Extract", NativeExtract },

    { "System2_ExecuteThreaded", NativeExecuteThreaded },
    { "System2_ExecuteFormattedThreaded", NativeExecuteFormattedThreaded },
    { "System2ExecuteOutput.GetOutput", NativeExecuteOutput_GetOutput },
    { "System2ExecuteOutput.Length.get", NativeExecuteOutput_GetLength },
    { "System2ExecuteOutput.ExitStatus.get", NativeExecuteOutput_GetExitStatus },

    { "System2_Execute", NativeExecute },
    { "System2_ExecuteFormatted", NativeExecuteFormatted },

    { "System2_GetGameDir", NativeGetGameDir },
    { "System2_GetOS", NativeGetOS },

    { "System2_GetStringMD5", NativeGetStringMD5 },
    { "System2_GetFileMD5", NativeGetFileMD5 },
    { "System2_GetStringCRC32", NativeGetStringCRC32 },
    { "System2_GetFileCRC32", NativeGetFileCRC32 },
    { nullptr, nullptr },
};

#endif