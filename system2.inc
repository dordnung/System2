#if defined _system2_included
#endinput
#endif
#define _system2_included

enum OS
{
	OS_UNKNOWN
	OS_WINDOWS,
	OS_UNIX,
	OS_MAC
}

/**
 * Called when finished with RunThreadCommand !
 */
funcenum SysCallback 
{
	public(const String:command[])
};

/**
 * Execute a threaded system command
 *
 * @param command			Command to execute.
 * @param cb			  	Callback function.
 *
 * @noreturn.
 */
native RunThreadCommand(const String:command[], SysCallback:cb);

/**
 * Execute a non threaded system command
 *
 * @param command			Command to execute.
 *
 * @noreturn.
 */
native RunCommand(const String:command[]);

/**
 * Get the absolute gamedir (e.g. /home/.../.../cstrike)
 *
 * @param GameDir			String to save gamedir in.
 * @param maxlength			Size of the String.
 *
 * @noreturn.
 */
native GetGameDir(String:GameDir[], maxlength);

/**
 * Get the OS, on which the server runs
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
