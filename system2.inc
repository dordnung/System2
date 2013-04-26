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
	OS_UNKNOWN,
	OS_WINDOWS,
	OS_UNIX,
	OS_MAC
}




/**
 * A list of possible return statuses of System2_RunThreadCommand.
 */
enum CMDReturn
{
	CMD_SUCCESS,
	CMD_EMPTY,
	CMD_ERROR,
}




/**
 * A list of possible archives of System2_CompressFile.
 */
enum CompressArchive
{
	ARCHIVE_ZIP,
	ARCHIVE_7Z,
	ARCHIVE_GZIP,
	ARCHIVE_BZIP2,
	ARCHIVE_TAR
}




/**
 * A list of possible compress levels of System2_CompressFile.
 */
enum CompressLevel
{
	LEVEL_1,
	LEVEL_3,
	LEVEL_5,
	LEVEL_7,
	LEVEL_9
}









/**
 * Called when finished with System2_RunThreadCommand.
 *
 * @param command				The executed command.
 * @param output				Output of the command.
 * @param status				CMDReturn status
 */
funcenum CmdCallback 
{
	public(const String:command[], const String:output[], CMDReturn:status)
};







/**
 * Called on every update when downloading/uploading a file.
 *
 * @param finished				Is downloading / uploading finished?
 * @param error					Error when finished. If no error string is empty.
 * @param dltotal				Download size total in bytes.
 * @param dlnow					Current download size in bytes.
 * @param ultotal				Upload size total in bytes.
 * @param ulnow					Current upload size in bytes.
 */
funcenum TransferUpdate 
{
	public(bool:finished, const String:error[], Float:dltotal, Float:dlnow, Float:ultotal, Float:ulnow)
};








/**
 * Called when finished with System2_CopyFile.
 *
 * @param success				Was copying successfull? (Or couldn't open current or new file)
 */
funcenum CopyCallback 
{
	public(bool:success)
};









/**
 * Downloads a file from a HTTP site.
 *
 * @param updateFunction		Function to call on update.
 * @param URL					Download URL.
 * @param localFile				Local file to save to.
 */
native System2_DownloadFile(TransferUpdate:updateFunction, const String:URL[], const String:localFile[]);







/**
 * Downloads a file from a FTP server.
 *
 * @param updateFunction		Function to call on update.
 * @param remoteFile			Path to the file on the FTP server.
 * @param localFile				Local file to save to.
 * @param host					The FTP host.
 * @param user					The FTP username.
 * @param pass					The FTP password.
 * @param port					The FTP port (Default: 21).
 */
native System2_DownloadFTPFile(TransferUpdate:updateFunction, const String:remoteFile[], const String:localFile[], const String:host[], const String:user[] = "", const String:pass[] = "", port = 21);







/**
 * Uploads a file to a FTP server.
 *
 * @param updateFunction		Function to call on update.
 * @param localFile				Local file to upload.
 * @param remoteFile			Path to the file on the FTP server.
 * @param host					The FTP host.
 * @param user					The FTP username.
 * @param pass					The FTP password.
 * @param port					The FTP port (Default: 21).
 */
native System2_UploadFTPFile(TransferUpdate:updateFunction, const String:localFile[], const String:remoteFile[], const String:host[], const String:user[] = "", const String:pass[] = "", port = 21);







/**
 * Copies a file
 *
 * @param callback				Callback function.
 * @param pathToFile			Path to the file
 * @param pathToCopy			Path to copy to.
 *
 * @noreturn
 */
native System2_CopyFile(CopyCallback:callback, const String:pathToFile[], const String:pathToCopy[]);







/**
 * Compresses a file to a archive
 *
 * @param callback				Callback function.
 * @param pathToFile			Path to the file/folder to compress.
 * @param pathToCompress		Path to archive file.
 * @param archive				Archive typ
 * @param level					Archive compress level
 *
 * @noreturn
 */
native System2_CompressFile(CmdCallback:callback, const String:pathToFile[], const String:pathToArchive[], CompressArchive:archive = ARCHIVE_ZIP, CompressLevel:level = LEVEL_9);






/**
 * Extracts a lot of archives with 7zip
 *
 * @param callback				Callback function.
 * @param pathToArchive			Path to the archive file.
 * @param pathToExtract			Path to extract to.
 *
 * @noreturn
 */
native System2_ExtractArchive(CmdCallback:callback, const String:pathToArchive[], const String:pathToExtract[]);







/**
 * Executes a threaded system command
 *
 * @param callback			Callback function.
 * @param command			Command string format
 * @param ...				Command string arguments.
 *
 * @noreturn
 */
native System2_RunThreadCommand(CmdCallback:callback, const String:command[], any:...);

// Backwards Compatiblity
native RunThreadCommand(CmdCallback:callback, const String:command[], any:...);







/**
 * Executes a non threaded system command
 *
 * @param output			Output variable
 * @param size				Size of the output variable
 * @param command			Command string format
 * @param ...				Command string arguments.
 *
 * @return					CMDReturn var
 */
native CMDReturn:System2_RunCommand(String:output[], size, const String:command[], any:...);

// Backwards Compatiblity
native CMDReturn:RunCommand(String:output[], size, const String:command[], any:...);







/**
 * Gets the absolute gamedir (e.g. /home/.../.../cstrike)
 *
 * @param gamedir			String to save gamedir in.
 * @param maxlength			Size of the string.
 *
 * @noreturn
 */
native System2_GetGameDir(String:gamedir[], maxlength);

// Backwards Compatiblity
native GetGameDir(String:gamedir[], maxlength);







/**
 * Gets the servers operating system
 *
 * @return						OS_UNKNOWN, OS_WINDOWS, OS_UNIX, OS_MAC
 */
native OS:System2_GetOS();

// Backwards Compatiblity
native OS:GetOS();









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