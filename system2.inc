/**
 * -----------------------------------------------------
 * File        system2.inc
 * Authors     David Ordnung
 * License     GPLv3
 * Web         http://dordnung.de
 * -----------------------------------------------------
 * 
 * Copyright (C) 2017 David Ordnung
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

#if defined _system2_included
	#endinput
#endif

#define _system2_included



/**
 * Max Size of a command.
 */
#define CMD_MAX_LENGTH 2048

/**
 * Max Size of the command or page output in CmdCallback.
 */
#define CMD_MAX_RETURN 4096



/**
 * A list of operating systems.
 */
enum OS
{
	OS_UNKNOWN,                 // OS couldn't be determined
	OS_WINDOWS,                 // Windows
	OS_UNIX,                    // Linux / Unix
	OS_MAC                      // MAC
}


/**
 * A list of possible command return states
 */
enum CMDReturn
{
	CMD_SUCCESS,                // Fully finished
	CMD_EMPTY,                  // Result is empty (only System2_RunThreadCommand)
	CMD_ERROR,                  // An error appeared
	CMD_PROGRESS                // Not finished yet -> Wait for other state
}


/**
 * A list of possible archives
 */
enum CompressArchive
{
	ARCHIVE_ZIP,                // zip
	ARCHIVE_7Z,                 // 7 zip
	ARCHIVE_GZIP,               // gzip
	ARCHIVE_BZIP2,              // bzip2
	ARCHIVE_TAR                 // tar
}


/**
 * A list of possible compress levels
 */
enum CompressLevel
{
	LEVEL_1,                    // Weekest
	LEVEL_3,
	LEVEL_5,
	LEVEL_7,
	LEVEL_9                     // Strongest
}



/**
 * Called when finished with a command or when retrieving the content of a page.
 * Maybe called more than once, if output is greater than 4096 bytes.
 * Use status variable to check if it's the last call or not.
 *
 * @param output                Output of the command / page.
 * @param size                  Size of output string.
 * @param status                CMDReturn status.
 * @param data                  Data passed.
 * @param command               The command that was executed. Will be empty with PageGet.
 */
typeset CmdCallback
{
	function void (const char[] output, const int size, CMDReturn status, any data, const char[] command);
	function void (const char[] output, const int size, CMDReturn status, any data);
	function void (const char[] output, const int size, CMDReturn status);
};


/**
 * Called on every update when downloading / uploading a file.
 *
 * @param finished              Is downloading / uploading finished?
 * @param error                 Error when finished. If no error string is empty.
 * @param dltotal               Total downloaded size in bytes.
 * @param dlnow                 Current downloaded size in bytes.
 * @param ultotal               Total uploaded size in bytes.
 * @param ulnow                 Current uploaded size in bytes.
 * @param data                  Data passed.
 */
typeset TransferUpdated
{
	function void (bool finished, const char[] error, float dltotal, float dlnow, float ultotal, float ulnow, any data);
	function void (bool finished, const char[] error, float dltotal, float dlnow, float ultotal, float ulnow);
};


/**
 * Called when finished with System2_CopyFile.
 *
 * @param success               Was copying successfull? (Or couldn't open current or new file).
 * @param from                  File which was copied.
 * @param to                    New file.
 * @param data                  Data passed.
 */
typeset CopyCallback 
{
	function void (bool success, const char[] from, const char[] to, any data);
	function void (bool success, const char[] from, const char[] to);
};



/**
 * Gets the content of a page.
 *
 * @param callback              Callback function when finished. Check callback.status to check if process is finished.
 * @param url                   The URL of the page to load. Attach GET parameters here and leave post parameter empty to perform a GET request.
 * @param post                  POST parameters (use like this: "name=test&pw=test2"). Leave empty to perform a GET request. 
 * @param userAgent             Useragent to use. Leave empty for default one.
 * @param data                  Additional data to pass to the callback.
 *
 * @noreturn
 */
native void System2_GetPage(CmdCallback callback, const char[] url, const char[] post = "", const char[] userAgent = "", any data = INVALID_HANDLE);


/**
 * Downloads a file from an URL.
 *
 * @param updateFunction        Function to call on update. Check updateFunction.finished to check if downloading is finished.
 * @param url                   File URL to download from.
 * @param localFile             Local file to save to.
 * @param data                  Additional data to pass to the callback.
 *
 * @noreturn
 */
native void System2_DownloadFile(TransferUpdated updateFunction, const char[] url, const char[] localFile, any data = INVALID_HANDLE);


/**
 * Downloads a file from a FTP server.
 *
 * @param updateFunction        Function to call on update. Check updateFunction.finished to check if downloading is finished.
 * @param remoteFile            Path to the file on the FTP server.
 * @param localFile             Local file to save to.
 * @param host                  The FTP host.
 * @param user                  The FTP username.
 * @param pass                  The FTP password.
 * @param port                  The FTP port (Default: 21).
 * @param data                  Additional data to pass to the callback.
 *
 * @noreturn
 */
native void System2_DownloadFTPFile(TransferUpdated updateFunction, const char[] remoteFile, const char[] localFile, const char[] host, const char[] user = "", const char[] pass = "", int port = 21, any data = INVALID_HANDLE);


/**
 * Uploads a file to a FTP server.
 *
 * @param updateFunction        Function to call on update. Check updateFunction.finished to check if uploading is finished.
 * @param localFile             Local file to upload.
 * @param remoteFile            Path to the file on the FTP server.
 * @param host                  The FTP host.
 * @param user                  The FTP username.
 * @param pass                  The FTP password.
 * @param port                  The FTP port (Default: 21).
 * @param data                  Additional data to pass to the callback.
 *
 * @noreturn
 */
native void System2_UploadFTPFile(TransferUpdated updateFunction, const char[] localFile, const char[] remoteFile, const char[] host, const char[] user = "", const char[] pass = "", int port = 21, any data = INVALID_HANDLE);


/**
 * Copies a file to another location.
 *
 * @param callback              Callback function when finished with copy. Check callback.success to check if copying was successful.
 * @param pathToFile            Path to the file.
 * @param pathToCopy            Path to copy to (including filename).
 * @param data                  Additional data to pass to the callback.
 *
 * @noreturn
 */
native void System2_CopyFile(CopyCallback callback, const char[] pathToFile, const char[] pathToCopy, any data = INVALID_HANDLE);


/**
 * Compresses a file to an archive.
 *
 * @param callback              Callback function when finished with compressing. Check callback.status to check if process is finished.
 * @param pathToFile            Path to the file / folder to compress.
 * @param pathToCompress        Path to save archive file to (including filename).
 * @param archive               Archive type to use.
 * @param level                 Archive compress level to use.
 * @param data                  Additional data to pass to the callback.
 *
 * @noreturn
 */
native void System2_CompressFile(CmdCallback callback, const char[] pathToFile, const char[] pathToArchive, CompressArchive archive = ARCHIVE_ZIP, CompressLevel level = LEVEL_9, any data = INVALID_HANDLE);


/**
 * Extracts a lot of archive types with 7zip.
 *
 * @param callback              Callback function when finished with extracting. Check callback.status to check if process is finished.
 * @param pathToArchive         Path to the archive file.
 * @param pathToExtract         Path to extract to.
 * @param data                  Additional data to pass to the callback.
 *
 * @noreturn
 */
native void System2_ExtractArchive(CmdCallback callback, const char[] pathToArchive, const char[] pathToExtract, any data = INVALID_HANDLE);


/**
 * Executes a threaded system command.
 *
 * @param callback              Callback function when command was executed. Check callback.status to check if process is finished.
 * @param command               Command string format.
 * @param ...                   Command string arguments.
 *
 * @noreturn
 */
native void System2_RunThreadCommand(CmdCallback callback, const char[] command, any ...);


/**
 * Executes a threaded system command and allows to pass additional data.
 *
 * @param callback              Callback function when command was executed. Check callback.status to check if process is finished.
 * @param data                  Data to pass to the callback.
 * @param command               Command string format.
 * @param ...                   Command string arguments.
 *
 * @noreturn
 */
native void System2_RunThreadCommandWithData(CmdCallback callback, any data, const char[] command, any ...);


/**
 * Executes a non threaded system command.
 *
 * @param output                Variable to store the command output.
 * @param size                  Size of the output variable.
 * @param command               Command string format.
 * @param ...                   Command string arguments.
 *
 * @return                      CMDReturn status.
 */
native CMDReturn System2_RunCommand(char[] output, int size, const char[] command, any ...);


/**
 * Gets the absolute gamedir of the current running game (e.g. /home/.../.../cstrike).
 * You may need this when running system commands.
 *
 * @param gamedir               String to save gamedir in.
 * @param maxlength             Size of the string.
 *
 * @noreturn
 */
native void System2_GetGameDir(char[] gamedir, int maxlength);


/**
 * Gets the servers operating system.
 *
 * @return                      OS_UNKNOWN, OS_WINDOWS, OS_UNIX, OS_MAC.
 */
native OS System2_GetOS();


/**
 * Gets the MD5 hex hash of a string.
 *
 * @param string                The string to create the MD5 hash of.
 * @param buffer                The string the MD5 hash will be recorded in.
 * @param maxlength             Max length of the buffer. Should be greather or equal to 33 (32 MD5 + 1 terminator)
 * @noreturn
 */
native void System2_GetStringMD5(const char[] str, char[] buffer, int maxlength);


/**
 * Gets the MD5 hex hash of a files content.
 *
 * @param file                  The path to the file.
 * @param buffer                The string the MD5 hash will be recorded in.
 * @param maxlength             Max length of the buffer. Should be greather or equal to 33 (32 MD5 + 1 terminator)
 * @return                      True on success, false when file couldn't be opened.
 */
native bool System2_GetFileMD5(const char[] file, char[] buffer, int maxlength);


/**
 * Gets the CRC32 hex hash of a string.
 *
 * @param string                The string to create the CRC32 hash of.
 * @param buffer                The string the CRC32 hash will be recorded in.
 * @param maxlength             Max length of the buffer. Should be greather or equal to 9 (9 CRC32 + 1 terminator)
 * @noreturn
 */
native void System2_GetStringCRC32(const char[] str, char[] buffer, int maxlength);


/**
 * Gets the CRC32 hex hash of a files content.
 *
 * @param file                  The path to the file.
 * @param buffer                The string the CRC32 hash will be recorded in.
 * @param maxlength             Max length of the buffer. Should be greather or equal to 9 (9 CRC32 + 1 terminator)
 * @return                      True on success, false when file couldn't be opened.
 */
native bool System2_GetFileCRC32(const char[] file, char[] buffer, int maxlength);



/**
 * Encodes a string for safe url transfer.
 * Written by Peace-Maker (i guess), formatted for better readability.
 *
 * @param stringToEncode        The string to encode.
 * @param maxlength             The maxlength of the string.
 * @param safe                  Adding additional safe strings.
 * @param format                Is the string formatted.
 */
stock void URLEncode(char[] stringToEncode, int maxlength, char[] safe = "/", bool format = false)
{
	char sAlwaysSafe[256];
	Format(sAlwaysSafe, sizeof(sAlwaysSafe), "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_.-%s", safe);
	
	// Need 2 '%' since sp's Format parses one as a parameter to replace
	// http://wiki.alliedmods.net/Format_Class_Functions_%28SourceMod_Scripting%29
	if (format)
	{
		ReplaceString(stringToEncode, maxlength, "%", "%%25");
	}
	else
	{
		ReplaceString(stringToEncode, maxlength, "%", "%25");
	}
	
	
	char sChar[8];
	char sReplaceChar[8];
	
	for (new int i = 1; i < 256; i++)
	{
		// Skip the '%' double replace ftw..
		if (i == 37)
		{
			continue;
		}
		
		
		Format(sChar, sizeof(sChar), "%c", i);

		if (StrContains(sAlwaysSafe, sChar) == -1 && StrContains(stringToEncode, sChar) != -1)
		{
			if (format)
			{
				Format(sReplaceChar, sizeof(sReplaceChar), "%%%%%02X", i);
			}
			else
			{
				Format(sReplaceChar, sizeof(sReplaceChar), "%%%02X", i);
			}
			
			ReplaceString(stringToEncode, maxlength, sChar, sReplaceChar);
		}
	}
}

public Extension __ext_system2 =
{
	name = "System2",
	file = "system2.ext",

	#if defined AUTOLOAD_EXTENSIONS
		autoload = 1,
	#else
		autoload = 0,
	#endif

	#if defined REQUIRE_EXTENSIONS
		required = 1,
	#else
		required = 0,
	#endif
};


#if !defined REQUIRE_EXTENSIONS
	public void __ext_system2_SetNTVOptional()
	{
		MarkNativeAsOptional("System2_GetPage");
		MarkNativeAsOptional("System2_CompressFile");
		MarkNativeAsOptional("System2_ExtractArchive");
		MarkNativeAsOptional("System2_DownloadFile");
		MarkNativeAsOptional("System2_DownloadFTPFile");
		MarkNativeAsOptional("System2_UploadFTPFile");
		MarkNativeAsOptional("System2_RunThreadCommand");
		MarkNativeAsOptional("System2_RunCommand");
		MarkNativeAsOptional("System2_GetGameDir");
		MarkNativeAsOptional("System2_GetOS");
		MarkNativeAsOptional("System2_GetStringMD5");
		MarkNativeAsOptional("System2_GetFileMD5");
		MarkNativeAsOptional("System2_GetStringCRC32");
		MarkNativeAsOptional("System2_GetFileCRC32");
	}
#endif