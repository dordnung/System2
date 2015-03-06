#if defined _system2_included
	#endinput
#endif

#define _system2_included


// Max Size of the Command Output.
#define CMD_MAX_RETURN 4096


/**
 * A list of operating systems.
 */
enum OS
{
	OS_UNKNOWN,                 // Shouldn't happen
	OS_WINDOWS,                 // Windows
	OS_UNIX,                    // Linux / Unix
	OS_MAC                      // MAC
}

/**
 * A list of possible cmd return states
 */
enum CMDReturn
{
	CMD_SUCCESS,                // Fully finished
	CMD_EMPTY,                  // Result is empty (only System2_RunThreadCommand)
	CMD_ERROR,                  // An error appeared
	CMD_PROGRESS                // Not finished yet
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
 * Called when finished with a command.
 * Maybe called more than once, if output is greater than 4096.
 * Use status variable to check if it's the last call or not.
 *
 * @param output                Output of the command / page.
 * @param size                  Size of output string.
 * @param status                CMDReturn status
 * @param data                  Data passed.
 */
funcenum CmdCallback
{
	public(const String:output[], const size, CMDReturn:status, any:data),
	public(const String:output[], const size, CMDReturn:status)
};

/**
 * Called on every update when downloading / uploading a file.
 *
 * @param finished              Is downloading / uploading finished?
 * @param error                 Error when finished. If no error string is empty.
 * @param dltotal               Downloaded size total in bytes.
 * @param dlnow                 Current downloaded size in bytes.
 * @param ultotal               Uploaded size total in bytes.
 * @param ulnow                 Current uploaded size in bytes.
 * @param data                  Data passed.
 */
funcenum TransferUpdated
{
	public(bool:finished, const String:error[], Float:dltotal, Float:dlnow, Float:ultotal, Float:ulnow, any:data),
	public(bool:finished, const String:error[], Float:dltotal, Float:dlnow, Float:ultotal, Float:ulnow)
};

/**
 * Called when finished with System2_CopyFile.
 *
 * @param success               Was copying successfull? (Or couldn't open current or new file)
 * @param from                  File which was copied.
 * @param to                    New file.
 * @param data                  Data passed.
 */
funcenum CopyCallback 
{
	public(bool:success, String:from[], String:to[], any:data),
	public(bool:success, String:from[], String:to[])
};


/**
 * Gets the contents of a page
 *
 * @param callback              Callback function when finished.
 * @param url                   The URL of the page to load.
 * @param post                  POST parameters (use like this: "name=test&pw=test2")
 * @param useragent             Useragent to use. Leave empty for default one.
 * @param data                  Additional data to pass to the callback.
 *
 * @noreturn
 */
native System2_GetPage(CmdCallback:callback, const String:url[], const String:post[] = "", const String:useragent[] = "", any:data = INVALID_HANDLE);

/**
 * Downloads a file from a URL.
 *
 * @param updateFunction        Function to call on update.
 * @param url                   File URL to download from.
 * @param localFile             Local file to save to.
 * @param data                  Additional data to pass to the callback.
 *
 * @noreturn
 */
native System2_DownloadFile(TransferUpdated:updateFunction, const String:url[], const String:localFile[], any:data = INVALID_HANDLE);

/**
 * Downloads a file from a FTP server.
 *
 * @param updateFunction        Function to call on update.
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
native System2_DownloadFTPFile(TransferUpdated:updateFunction, const String:remoteFile[], const String:localFile[], const String:host[], const String:user[] = "", const String:pass[] = "", port = 21, any:data = INVALID_HANDLE);

/**
 * Uploads a file to a FTP server.
 *
 * @param updateFunction        Function to call on update.
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
native System2_UploadFTPFile(TransferUpdated:updateFunction, const String:localFile[], const String:remoteFile[], const String:host[], const String:user[] = "", const String:pass[] = "", port = 21, any:data = INVALID_HANDLE);

/**
 * Copies a file to another location.
 *
 * @param callback              Callback function when finished with copy.
 * @param pathToFile            Path to the file
 * @param pathToCopy            Path to copy to (including filename).
 * @param data                  Additional data to pass to the callback.
 *
 * @noreturn
 */
native System2_CopyFile(CopyCallback:callback, const String:pathToFile[], const String:pathToCopy[], any:data = INVALID_HANDLE);

/**
 * Compresses a file to an archive.
 *
 * @param callback              Callback function when finished with compressing.
 * @param pathToFile            Path to the file / folder to compress.
 * @param pathToCompress        Path to archive file.
 * @param archive               Archive typ.
 * @param level                 Archive compress level.
 * @param data                  Additional data to pass to the callback.
 *
 * @noreturn
 */
native System2_CompressFile(CmdCallback:callback, const String:pathToFile[], const String:pathToArchive[], CompressArchive:archive = ARCHIVE_ZIP, CompressLevel:level = LEVEL_9, any:data = INVALID_HANDLE);

/**
 * Extracts a lot of archive types with 7zip
 *
 * @param callback              Callback function when finished with extracting.
 * @param pathToArchive         Path to the archive file.
 * @param pathToExtract         Path to extract to.
 * @param data                  Additional data to pass to the callback.
 *
 * @noreturn
 */
native System2_ExtractArchive(CmdCallback:callback, const String:pathToArchive[], const String:pathToExtract[], any:data = INVALID_HANDLE);

/**
 * Executes a threaded system command.
 *
 * @param callback              Callback function when executed the command.
 * @param command               Command string format
 * @param ...                   Command string arguments.
 *
 * @noreturn
 */
native System2_RunThreadCommand(CmdCallback:callback, const String:command[], any:...);

/**
 * Executes a non threaded system command.
 *
 * @param output                Variable to store the command output.
 * @param size                  Size of the output variable.
 * @param command               Command string format.
 * @param ...                   Command string arguments.
 *
 * @return                      CMDReturn var.
 */
native CMDReturn:System2_RunCommand(String:output[], size, const String:command[], any:...);

/**
 * Gets the absolute gamedir (e.g. /home/.../.../cstrike)
 *
 * @param gamedir               String to save gamedir in.
 * @param maxlength             Size of the string.
 *
 * @noreturn
 */
native System2_GetGameDir(String:gamedir[], maxlength);

/**
 * Gets the servers operating system
 *
 * @return                      OS_UNKNOWN, OS_WINDOWS, OS_UNIX, OS_MAC
 */
native OS:System2_GetOS();


// Backwards Compatiblity
native RunThreadCommand(CmdCallback:callback, const String:command[], any:...);
native CMDReturn:RunCommand(String:output[], size, const String:command[], any:...);
native GetGameDir(String:gamedir[], maxlength);
native OS:GetOS();



/**
 * Encodes a string for safe url transfer.
 * Written by Peace-Maker (i guess), formatted for better readability.
 *
 * @param stringToEncode        The string to encode.
 * @param maxlength             The maxlength of the string.
 * @param safe                  Adding additional safe strings.
 * @param format                Is the string formatted.
 */
stock URLEncode(String:stringToEncode[], maxlength, String:safe[] = "/", bool:format = false)
{
	decl String:sAlwaysSafe[256];
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
	
	
	new String:sChar[8];
	new String:sReplaceChar[8];
	
	for (new i = 1; i < 256; i++)
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



public Extension:__ext_system2 =
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
	public __ext_system2_SetNTVOptional()
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

		// Backwarts Compatiblity
		MarkNativeAsOptional("RunThreadCommand");
		MarkNativeAsOptional("RunCommand");
		MarkNativeAsOptional("GetGameDir");
		MarkNativeAsOptional("GetOS");
	}
#endif