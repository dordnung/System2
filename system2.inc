#if defined _system2_included
#endinput
#endif
#define _system2_included

// Max Size of the Command Output
#define CMD_MAX_RETURN 4096

enum OS
{
	OS_UNKNOWN,
	OS_WINDOWS,
	OS_UNIX,
	OS_MAC
}

enum Cmd_Return
{
	CMD_SUCCESS,
	CMD_EMPTY,
	CMD_ERROR,
}

/**
 * Called when finished with RunThreadCommand !
 */
funcenum CmdCallback 
{
	public(const String:command[], const String:output[], Cmd_Return:status)
};

/**
 * Execute a threaded system command
 
 * @param cb			  	Callback function.
 * @param command			Command string format
 * @param ...				Command string arguments.
 *
 * @noreturn
 */
native RunThreadCommand(CmdCallback:cb, const String:command[], any:...);

/**
 * Execute a non threaded system command
 *
 * @param output			Output variable
 * @param size				Size of the output variable
 * @param command			Command string format
 * @param ...				Command string arguments.
 *
 * @return					Cmd_Return var
 */
native Cmd_Return:RunCommand(String:output[], size, const String:command[], any:...);

/**
 * Get the absolute gamedir (e.g. /home/.../.../cstrike)
 *
 * @param gamedir			String to save gamedir in.
 * @param maxlength			Size of the String.
 *
 * @noreturn
 */
native GetGameDir(String:gamedir[], maxlength);

/**
 * Get the Servers Running System
 *
 * @return						OS_UNKNOWN, OS_WINDOWS, OS_UNIX, OS_MAC
 */
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
	MarkNativeAsOptional("RunThreadCommand");
	MarkNativeAsOptional("RunCommand");
	MarkNativeAsOptional("GetGameDir");
	MarkNativeAsOptional("GetOS");
}
#endif
