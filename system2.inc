/**
 * -----------------------------------------------------
 * File        system2.inc
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

#if defined _system2_included
    #endinput
#endif

#define _system2_included


/**
 * Max length of a command when using formatted natives.
 */
#define CMD_MAX_LENGTH 2048


/**
 * A list of possible HTTP request methods.
 */
enum HTTPRequestMethod
{
    METHOD_GET,
    METHOD_POST,
    METHOD_PUT,
    METHOD_PATCH,
    METHOD_DELETE,
    METHOD_HEAD
}


/**
 * A list of possible compression levels for the System2_Compress native.
 */
enum CompressLevel
{
    LEVEL_1,            // Weekest
    LEVEL_3,
    LEVEL_5,
    LEVEL_7,
    LEVEL_9             // Strongest
}

/**
 * A list of possible archive formats for the System2_Compress native.
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
 * A list of possible operating systems for the System2_GetOS native.
 */
enum OS
{
    OS_UNKNOWN,         // OS couldn't be determined
    OS_WINDOWS,         // Windows
    OS_UNIX,            // Linux / Unix
    OS_MAC              // MAC
}



/**
 * Called when a HTTP request was finished.
 * The response will only be valid in the callback and will be destroyed afterwards.
 * The request is a copy of the original request and will be destroyed afterwards.
 *
 * @param success       Whether the request could made.
 *                      This not means that the request itself was successful!
 * @param error         If success is false this will contain the error message.
 * @param request       A copy of the made HTTP request.
 *                      Can't be deleted, as it will be destroyed after the callback!
 * @param response      HTTP response of the request. Is null if success is false.
 *                      Can't be deleted, as it will be destroyed after the callback!
 * @param method        The HTTP request method that was made.
 *
 * @noreturn
 */
typeset System2HTTPResponseCallback
{
    function void (bool success, const char[] error, System2HTTPRequest request, System2HTTPResponse response, HTTPRequestMethod method);
};

/**
 * Called when a FTP request was finished.
 * The response will only be valid in the callback and will be destroyed afterwards.
 * The request is a copy of the original request and will be destroyed afterwards.
 *
 * @param success       Whether the request could made.
 *                      This not means that the request itself was successful!
 * @param error         If success is false this will contain the error message.
 * @param request       A copy of the made FTP request.
 *                      Can't be deleted, as it will be destroyed after the callback!
 * @param response      FTP response of the request. Is null if success is false.
 *                      Can't be deleted, as it will be destroyed after the callback!
 * @param uploadedFile  The uploaded file or empty if no file was uploaded.
 *
 * @noreturn
 */
typeset System2FTPResponseCallback
{
    function void (bool success, const char[] error, System2FTPRequest request, System2FTPResponse response, const char[] uploadedFile);
};


/**
 * Called on a frequent interval while data of a HTTP request is being transferred.
 * The request is a copy of the original request and will be destroyed afterwards.
 *
 * @param request       A copy of the made HTTP request.
 *                      Can't be deleted, as it will be destroyed after the callback!
 * @param dlTotal       Total expected download size in bytes.
 * @param dlNow         Downloaded bytes so far.
 * @param ulTotal       Total expected upload size in bytes.
 * @param ulNow         Uploaded bytes so far.
 *
 * @noreturn
 */
typeset System2HTTPProgressCallback
{
    function void (System2HTTPRequest request, int dlTotal, int dlNow, int ulTotal, int ulNow);
};

/**
 * Called on a frequent interval while data of a FTP request is being transferred.
 * The request is a copy of the original request and will be destroyed afterwards.
 *
 * @param request       A copy of the made FTP request.
 *                      Can't be deleted, as it will be destroyed after the callback!
 * @param dlTotal       Total expected download size in bytes.
 * @param dlNow         Downloaded bytes so far.
 * @param ulTotal       Total expected upload size in bytes.
 * @param ulNow         Uploaded bytes so far.
 *
 * @noreturn
 */
typeset System2FTPProgressCallback
{
    function void (System2FTPRequest request, int dlTotal, int dlNow, int ulTotal, int ulNow);
};



/**
 * Called when finished with the System2_CopyFile native.
 *
 * @param success       Whether copying was successful (will fail if couldn't open 'from' or 'to' file).
 * @param from          Path to file that was copied.
 * @param to            Path to the new copied file.
 * @param data          Data passed to the copy native.
 *
 * @noreturn
 */
typeset System2CopyCallback
{
    function void (bool success, const char[] from, const char[] to, any data);
    function void (bool success, const char[] from, const char[] to);
};



/**
 * Called when finished with System2_ExecuteThreaded or System2_ExecuteFormattedThreaded native.
 * The output will only be valid in the callback and will be destroyed afterwards.
 *
 * @param success       Whether the execution was successful or not.
 *                      This not means that the command itself was successful!
 * @param output        Output of the execution. Is null if success is false.
 *                      Can't be deleted, as it will be destroyed after the callback!
 * @param command       The executed command.
 * @param data          Data passed to the execution native.
 *
 * @noreturn
 */
typeset System2ExecuteCallback
{
    function void (bool success, System2ExecuteOutput output, const char[] command, any data);
    function void (bool success, System2ExecuteOutput output, const char[] command);
};



/**
 * Basic methodmap for a request.
 * Use System2HTTPRequest or System2FTPRequest to actually create a request.
 */
methodmap System2Request < Handle {
    /**
     * Sets the URL of the request.
     * Query parameters have to be given directly in the URL.
     * Use System2_URLEncode to encode the URL or query parameters.
     *
     * @param url       URL to use for the request.
     *
     * @noreturn
     * @error           Invalid Request.
     */
    public native void SetURL(const char[] url);

    /**
     * Retrieves the URL of the request.
     *
     * @param url       Buffer to store URL in.
     * @param maxlength Maxlength of the buffer.
     *
     * @noreturn
     * @error           Invalid Request.
     */
    public native void GetURL(char[] url, int maxlength);

    /**
     * Sets the port of the request.
     *
     * @param port      Port to use for the request.
     *
     * @noreturn
     * @error           Invalid Request.
     */
    public native void SetPort(int port);

    /**
     * Returns the port of the request.
     *
     * @return          The port of the request or 0 if none set.
     * @error           Invalid Request.
     */
    public native int GetPort();

    /**
     * Sets the file to write the output of the request to.
     * Use this to download the output of a response to a file.
     *
     * @param file      File to write the output to.
     *
     * @noreturn
     * @error           Invalid Request.
     */
    public native void SetOutputFile(const char[] file);

    /**
     * Retrieves the file to write the output of the request to.
     *
     * @param file      Buffer to store file in.
     * @param maxlength Maxlength of the buffer.
     *
     * @noreturn
     * @error           Invalid Request.
     */
    public native void GetOutputFile(char[] file, int maxlength);

    /**
     * Sets whether to verify the SSL certificate on connections using SSL.
     * Only disable this, when you know what you do!
     *
     * @param verify    True to verify SSL certificate, otherwise false.
     *
     * @noreturn
     * @error           Invalid Request.
     */
    public native void SetVerifySSL(bool verify);

    /**
     * Returns whether SSL certificates will be verified.
     *
     * @return          True if SSL certificates will be verified, otherwise false.
     * @error           Invalid Request.
     */
    public native bool GetVerifySSL();

    /**
     * The timeout for the request.
     */
    property int Timeout {
        /**
         * Returns the timeout for the request.
         *
         * @return          The timeout of the request in milliseconds or 0 if none set.
         * @error           Invalid Request.
         */
        public native get();

        /**
         * Sets the timeout for the request.
         *
         * @param millis    The timeout for the request in milliseconds.
         *
         * @noreturn
         * @error           Invalid Request.
         */
        public native set(int millis);
    }

    /**
     * The any data bound to the request.
     */
    property any Any {
        /**
         * Returns the any data that was bound to this request.
         *
         * @return          The any data that was bound or 0 if none set.
         * @error           Invalid Request.
         */
        public native get();

        /**
         * Sets any data to bind to the request.
         *
         * @param Any       The any data to add.
         *
         * @noreturn
         * @error           Invalid Request.
         */
        public native set(any Any);
    }
}


/**
 * Methodmap to create a HTTP request.
 */
methodmap System2HTTPRequest < System2Request {
    /**
     * Creates a new HTTP Request.
     *
     * @param url       URL to use for the request.
     * @param callback  Response callback to call when the request is finished.
     *
     * @noreturn
     * @error           Invalid Request.
     */
    public native System2HTTPRequest(const char[] url, System2HTTPResponseCallback callback);


    /**
     * Sets the progress callback for the transfer of the request.
     * This is useful when downloading or uploading stuff.
     *
     * @param callback  Progress callback to call for the transfer.
     *
     * @noreturn
     * @error           Invalid Request.
     */
    public native void SetProgressCallback(System2HTTPProgressCallback callback);


    /**
     * Sets the body data to send with the request.
     * Use System2_URLEncode to encode the data.
     *
     * @param data      Body data to send with the request.
     *
     * @noreturn
     * @error           Invalid Request.
     */
    public native void SetData(const char[] data);

    /**
     * Retrieves the body data of the request.
     *
     * @param url       Buffer to store data in.
     * @param maxlength Maxlength of the buffer.
     *
     * @noreturn
     * @error           Invalid Request.
     */
    public native void GetData(char[] data, int maxlength);

    /**
     * Sets a HTTP request header.
     * Use System2_URLEncode to encode the header.
     *
     * @param name      Name of the header.
     * @param value     Value to set the header to.
     *
     * @noreturn
     * @error           Invalid Request.
     */
    public native void SetHeader(const char[] header, const char[] value);

    /**
     * Retrieves a HTTP request header
     *
     * @param name      Name of the header to retrieve.
     * @param value     Buffer to store value in.
     * @param maxlength Maxlength of the buffer.
     *
     * @return          True if header was set, otherwise false.
     * @error           Invalid Request.
     */
    public native bool GetHeader(const char[] header, char[] value, int maxlength);

    /**
     * Returns the number of set headers.
     *
     * @return          The number of set headers.
     * @error           Invalid Request.
     */
    public native int GetHeadersCount();

    /**
     * Retrieves all set header names of the request.
     *
     * @param names     Array of char arrays to store the names of the headers in.
     * @param maxNames  Max. header names to retrieve (first dimension of the names array).
     * @param maxName   Maxlength of a single header name (second dimension of the names array).
     *
     * @noreturn
     * @error           Invalid Request.
     */
    public native void GetHeadersArray(char[][] names, int maxNames, int maxName);

    /**
     * Returns all header names of the request as ArrayList.
     * Attention: ArrayList has to be deleted after use!
     *
     * @return          ArrayList with all request header names.
     * @error           Invalid Request.
     */
    public ArrayList GetHeaders() {
        int numHeaders = this.GetHeadersCount();
        char[][] headersArray = new char[numHeaders][128];

        ArrayList headers = new ArrayList(128);
        for (int i=0; i < numHeaders; i++) {
            headers.PushString(headersArray[i]);
        }

        return headers;
    }


    /**
     * Sets the user agent for the request.
     *
     * @param userAgent User agent to use.
     *
     * @noreturn
     * @error           Invalid Request.
     */
    public native void SetUserAgent(const char[] userAgent);

    /**
     * Sets basic authentication for the request.
     *
     * @param username  Username to use for basic authentication.
     * @param password  Password to use for basic authentication.
     *
     * @noreturn
     * @error           Invalid Request.
     */
    public native void SetBasicAuthentication(const char[] username, const char[] password);


    /**
     * Sends the request with the GET method.
     *
     * @noreturn
     * @error           Invalid Request.
     */
    public native void GET();

    /**
     * Sends the request with the POST method.
     *
     * @noreturn
     * @error           Invalid Request.
     */
    public native void POST();
    
    /**
     * Sends the request with the PUT method.
     *
     * @noreturn
     * @error           Invalid Request.
     */
    public native void PUT();
    
    /**
     * Sends the request with the PATCH method.
     *
     * @noreturn
     * @error           Invalid Request.
     */
    public native void PATCH();
    
    /**
     * Sends the request with the DELETE method.
     *
     * @noreturn
     * @error           Invalid Request.
     */
    public native void DELETE();
    
    /**
     * Sends the request with the HEAD method.
     *
     * @noreturn
     * @error           Invalid Request.
     */
    public native void HEAD();

    /**
     * The follow redirects property.
     */
    property bool FollowRedirects {
        /**
         * Returns whether the request follow redirects or not.
         * Default is to follow redirects.
         *
         * @return          True if it follow redirects, otherwise false.
         * @error           Invalid Request.
         */
        public native get();

        /**
         * Sets whether to follow redirects or not.
         * Default is to follow redirects.
         *
         * @param follow    True to follow redirects, otherwise false.
         *
         * @noreturn
         * @error           Invalid Request.
         */
        public native set(bool follow);
    }

    /**
     * The auto referer property.
     */
    property bool AutoReferer {
        /**
         * Returns whether to automatically append refereres on redirect.
         * Default is not to automatically append refereres.
         *
         * @return          True if referers will automatically append, otherwise false.
         * @error           Invalid Request.
         */
        public native get();

        /**
         * Sets whether to automatically append refereres on redirect.
         * This will only work when the FollowRedirects property is enabled.
         * Default is not to automatically append refereres.
         *
         * @param auto      True to automatically append refereres, otherwise false.
         *
         * @noreturn
         * @error           Invalid Request.
         */
        public native set(bool auto);
    }
}


/**
 * Methodmap to create a FTP request.
 */
methodmap System2FTPRequest < System2Request {
    /**
     * Creates a new FTP Request.
     *
     * @param url       FTP URL to use for the request.
     * @param callback  Response callback to call when the request is finished.
     *
     * @noreturn
     * @error           Invalid Request.
     */
    public native System2FTPRequest(const char[] url, System2FTPResponseCallback callback);


    /**
     * Sets the progress callback for the transfer of the request.
     *
     * @param callback  Progress callback to call for the transfer.
     *
     * @noreturn
     * @error           Invalid Request.
     */
    public native void SetProgressCallback(System2FTPProgressCallback callback);


    /**
     * Sets authentication for the request.
     *
     * @param username  Username to use for authentication.
     * @param password  Password to use for authentication.
     *
     * @noreturn
     * @error           Invalid Request.
     */
    public native void SetAuthentication(const char[] username, const char[] password);


    /**
     * Sends the request.
     * May result in a directory listing or a file download.
     *
     * @noreturn
     * @error           Invalid Request.
     */
    public native void Request();

    /**
     * Sends the request in order to upload content to a file.
     *
     * @param file      File to upload.

     * @noreturn
     * @error           Invalid Request.
     */
    public native void Upload(const char[] file);


    /**
     * The append to file property.
     */
    property bool AppendToFile {
        /**
         * Returns whether the request appends to the FTP file or replaces it when uploading a file.
         * Default the file will be replaced.
         *
         * @return          True if it append, false if replacing.
         * @error           Invalid Request.
         */
        public native get();

        /**
         * Sets whether to append to the FTP file or to replace it when uploading a file. 
         * Default the file will be replaced.
         *
         * @param append    True to append to file, otherwise false to replace it.
         *
         * @noreturn
         * @error           Invalid Request.
         */
        public native set(bool append);
    }

    /**
     * The create missing dirs property.
     */
    property bool CreateMissingDirs {
        /**
         * Returns whether the request creates missing dirs when uploading a file.
         * Default missing dirs will be created.
         *
         * @return          True if it creates missing dirs, otherwise false.
         * @error           Invalid Request.
         */
        public native get();

        /**
         * Sets whether to create missing dirs when uploading a file.
         * Default missing dirs will be created.
         *
         * @param create    True to create missing dirs, otherwise false.
         *
         * @noreturn
         * @error           Invalid Request.
         */
        public native set(bool create);
    }
}



/**
 * Basic methodmap for a response.
 */
methodmap System2Response < Handle {
    /**
     * Retrieves the last URL in the response.
     * This may differ from the request URL, if a redirect was followed.
     *
     * @param url       Buffer to store last URL in.
     * @param maxlength Maxlength of the URL buffer.
     *
     * @noreturn
     * @error           Invalid Response.
     */
    public native void GetLastUrl(char[] url, int maxlength);


    /**
     * Retrieves the content of the response.
     * This shouldn't be used when retrieved binary stuff.
     *
     * @param content   Buffer to store the content in.
     * @param maxlength Maxlength of the content buffer.
     * @param start     Start byte to start reading from.
     *                  You can use this to retrieve the content step by step.
     *
     * @return          Number of unread bytes. 0 if all bytes were read.
     * @error           Invalid Response.
     */
    public native int GetContent(char[] content, int maxlength, int start = 0);


    property int ContentSize {
        /**
         * Returns the size of the complete content.
         *
         * @return      Size of the content.
         * @error       Invalid Response.
         */
        public native get();
    }

    property int StatusCode {
        /**
         * Returns the status code of the response.
         *
         * @return      The status code.
         * @error       Invalid Response.
         */
        public native get();
    }

    property float TotalTime {
        /**
         * Returns the total time from the request until the response in seconds.
         *
         * @return      Total time from the request until the response in seconds.
         * @error       Invalid Response.
         */
        public native get();
    }
}


/**
 * Methodmap for a response of a HTTP request.
 */
methodmap System2HTTPResponse < System2Response {
    /**
     * Retrieves a HTTP response header
     *
     * @param name      Name of the header to retrieve.
     * @param value     Buffer to store value in.
     * @param maxlength Maxlength of the buffer.
     *
     * @return          True if header was set, otherwise false.
     * @error           Invalid Response.
     */
    public native bool GetHeader(const char[] name, char[] value, int maxlength);

    /**
     * Returns the number of set headers.
     *
     * @return          The number of set headers.
     * @error           Invalid Response.
     */
    public native int GetHeadersCount();

    /**
     * Retrieves all header names of the response.
     *
     * @param names     Array of char arrays to store the names of the headers in.
     * @param maxNames  Max. header names to retrieve (first dimension of the names array).
     * @param maxName   Maxlength of a single header name (second dimension of the names array).
     *
     * @noreturn
     * @error           Invalid Response.
     */
    public native void GetHeadersArray(char[][] names, int maxNames, int maxName);

    /**
     * Returns all header names of the response as ArrayList.
     * Attention: ArrayList has to be deleted after use!
     *
     * @return          ArrayList with all response header names.
     * @error           Invalid Response.
     */
    public ArrayList GetHeaders() {
        int numHeaders = this.GetHeadersCount();
        char[][] headersArray = new char[numHeaders][128];

        ArrayList headers = new ArrayList(128);
        for (int i=0; i < numHeaders; i++) {
            headers.PushString(headersArray[i]);
        }

        return headers;
    }
}


/**
 * Methodmap for a response of a FTP request.
 * Currently there are no special methods.
 */
methodmap System2FTPResponse < System2Response {
}



/**
 * Methodmap for the output of an execution.
 */
methodmap System2ExecuteOutput < Handle {
    /**
     * Retrieves the output of the command execution.
     *
     * @param output    Buffer to store the output in.
     * @param maxlength Maxlength of the output buffer.
     * @param start     Start byte to start reading from.
     *                  You can use this to retrieve the output step by step. 
     * 
     * @return          Number of unread bytes. 0 if all bytes were read.
     * @error           Invalid Output.
     */
    public native void GetOutput(char[] output, int maxlength, int start = 0);

    property int Size {
        /**
         * Returns the size of the complete output.
         *
         * @return      Size of the output.
         * @error       Invalid Output.
         */
        public native get();
    }
}



/**
 * Converts a plain string to an URL encoded string.
 * Be aware that the output string may be larger then the input string.
 *
 * @param input         String to encode.
 * @param output        Buffer to store encoded string in. May point to the input string (will replace input string).
 * @param maxlength     Maxlength of the output buffer.
 *
 * @return              True on success, false otherwise.
 */
native bool System2_URLEncode(const char[] input, char[] output, int maxlength);


/**
 * Converts an URL encoded string to a plain string.
 *
 * @param input         String to decode.
 * @param output        Buffer to store decoded string in. May point to the input string (will replace input string).
 * @param maxlength     Maxlength of the output buffer.
 *
 * @return              True on success, false otherwise.
 */
native bool System2_URLDecode(const char[] input, char[] output, int maxlength);



/**
 * Copies a file to another location.
 *
 * @param callback      Callback function when finished with copy.
 * @param from          Path to the file to copy.
 * @param to            Path to the file to copy to (including filename). File will be replaced if it exists.
 * @param data          Additional data to pass to the callback.
 *
 * @noreturn
 */
native void System2_CopyFile(System2CopyCallback callback, const char[] from, const char[] to, any data = 0);


/**
 * Compresses a file or folder to an archive.
 *
 * @param callback      Callback function when finished with compressing.
 * @param path          Path to the file / folder to compress.
 * @param archive       Path to the archive file to compress to (including filename).
 * @param archiveType   Archive type to use.
 * @param level         Compress level to use.
 * @param data          Additional data to pass to the callback.
 *
 * @noreturn
 */
native void System2_Compress(System2ExecuteCallback callback, const char[] path, const char[] archive, CompressArchive archiveType = ARCHIVE_ZIP, CompressLevel level = LEVEL_9, any data = 0);


/**
 * Extracts a lot of archive types with 7zip.
 *
 * @param callback      Callback function when finished with extracting.
 * @param archive       Path to the archive file to extract.
 * @param extractDir    Path to the directory to extract to.
 * @param data          Additional data to pass to the callback.
 *
 * @noreturn
 */
native void System2_Extract(System2ExecuteCallback callback, const char[] archive, const char[] extractDir, any data = 0);



/**
 * Executes a threaded system command.
 * Hint: Append 2>&1 to your command to retrieve also output to stderr.
 *
 * @param callback  Callback function when command was executed.
 * @param command   Command to execute.
 * @param data      Data to pass to the callback.
 *
 * @noreturn
 */
native void System2_ExecuteThreaded(System2ExecuteCallback callback, const char[] command, any data);


/**
 * Executes a threaded system command with support for a formatted command.
 * Note that the maxlength of the command is CMD_MAX_LENGTH, use System2_ExecuteThreaded if you need more.
 * Hint: Append 2>&1 to your command to retrieve also output to stderr.
 *
 * @param callback  Callback function when command was executed.
 * @param data      Data to pass to the callback.
 * @param command   Command string format.
 * @param ...       Command string arguments.
 *
 * @noreturn
 */
native void System2_ExecuteFormattedThreaded(System2ExecuteCallback callback, any data, const char[] command, any ...);


/**
 * Executes a non threaded system command.
 * Hint: Append 2>&1 to your command to retrieve also output to stderr.
 *
 * @param output    Buffer to store the output in.
 * @param maxlength Maxlength of the output buffer.
 * @param command   Command to execute.
 *
 * @return          True on success, otherwise false.
 */
native bool System2_Execute(char[] output, int maxlength, const char[] command);


/**
 * Executes a non threaded system command with support for a formatted command.
 * Note that the maxlength of the command is CMD_MAX_LENGTH, use System2_Execute if you need more.
 * Hint: Append 2>&1 to your command to retrieve also output to stderr.
 *
 * @param output    Buffer to store the output in.
 * @param maxlength Maxlength of the output buffer.
 * @param command   Command string format.
 * @param ...       Command string arguments.
 *
 * @return          True on success, otherwise false.
 */
native bool System2_ExecuteFormatted(char[] output, int maxlength, const char[] command, any ...);



/**
 * Retrieves the absolute path to the gamedir of the current running game (e.g. /home/.../.../cstrike).
 * You may need this when executing system commands.
 *
 * @param gamedir       Buffer to store gamedir in.
 * @param maxlength     Maxlength of the buffer.
 *
 * @noreturn
 */
native void System2_GetGameDir(char[] gamedir, int maxlength);


/**
 * Returns the server's operating system.
 *
 * @return              OS_UNKNOWN, OS_WINDOWS, OS_UNIX, OS_MAC.
 */
native OS System2_GetOS();



/**
 * Retrieves the MD5 hex hash of a string.
 *
 * @param string        String to create the MD5 hash of.
 * @param buffer        Buffer to store MD5 hash in.
 * @param maxlength     Maxlength of the buffer. Should be greater or equal to 33 (32 MD5 + 1 terminator).
 *
 * @noreturn
 */
native void System2_GetStringMD5(const char[] str, char[] buffer, int maxlength);


/**
 * Retrieves the MD5 hex hash of a files content.
 *
 * @param file          The path to the file.
 * @param buffer        Buffer to store MD5 hash in.
 * @param maxlength     Maxlength of the buffer. Should be greater or equal to 33 (32 MD5 + 1 terminator).
 *
 * @return              True on success, false when file couldn't be opened.
 */
native bool System2_GetFileMD5(const char[] file, char[] buffer, int maxlength);


/**
 * Retrieves the CRC32 hex hash of a string.
 *
 * @param string        The string to create the CRC32 hash of.
 * @param buffer        Buffer to store CRC32 hash in.
 * @param maxlength     Maxlength of the buffer. Should be greater or equal to 9 (9 CRC32 + 1 terminator).
 *
 * @noreturn
 */
native void System2_GetStringCRC32(const char[] str, char[] buffer, int maxlength);


/**
 * Retrieves the CRC32 hex hash of a files content.
 *
 * @param file          The path to the file.
 * @param buffer        Buffer to store CRC32 hash in.
 * @param maxlength     Maxlength of the buffer. Should be greater or equal to 9 (9 CRC32 + 1 terminator).
 *
 * @return              True on success, false when file couldn't be opened.
 */
native bool System2_GetFileCRC32(const char[] file, char[] buffer, int maxlength);



/**
 *
 * DEPRECATED v2 STUFF, do not use that anymore!
 *
 */

/**
 * Max Size of the command or page output in CmdCallback.
 */
#define CMD_MAX_RETURN 4096


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
#pragma deprecated Use System2HTTPRequest instead.
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
#pragma deprecated Use System2HTTPRequest instead.
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
#pragma deprecated Use System2FTPRequest instead.
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
#pragma deprecated Use System2FTPRequest instead.
native void System2_UploadFTPFile(TransferUpdated updateFunction, const char[] localFile, const char[] remoteFile, const char[] host, const char[] user = "", const char[] pass = "", int port = 21, any data = INVALID_HANDLE);


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
#pragma deprecated Use System2_Compress instead.
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
#pragma deprecated Use System2_Extract instead.
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
#pragma deprecated Use System2_ExecuteThreaded instead.
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
#pragma deprecated Use System2_ExecuteThreaded instead.
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
#pragma deprecated Use System2_Execute instead.
native CMDReturn System2_RunCommand(char[] output, int size, const char[] command, any ...);


/**
 * Encodes a string for safe url transfer.
 * Written by Peace-Maker (i guess), formatted for better readability.
 *
 * @param stringToEncode        The string to encode.
 * @param maxlength             The maxlength of the string.
 * @param safe                  Adding additional safe strings.
 * @param format                Is the string formatted.
 */
#pragma deprecated Use System2_URLEncode instead.
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
        MarkNativeAsOptional("System2Request.SetURL");
        MarkNativeAsOptional("System2Request.GetURL");
        MarkNativeAsOptional("System2Request.SetPort");
        MarkNativeAsOptional("System2Request.GetPort");
        MarkNativeAsOptional("System2Request.SetOutputFile");
        MarkNativeAsOptional("System2Request.GetOutputFile");
        MarkNativeAsOptional("System2Request.SetVerifySSL");
        MarkNativeAsOptional("System2Request.GetVerifySSL");
        MarkNativeAsOptional("System2Request.Timeout.get");
        MarkNativeAsOptional("System2Request.Timeout.set");
        MarkNativeAsOptional("System2Request.Any.get");
        MarkNativeAsOptional("System2Request.Any.set");
        
        MarkNativeAsOptional("System2HTTPRequest.System2HTTPRequest");
        MarkNativeAsOptional("System2HTTPRequest.SetProgressCallback");
        MarkNativeAsOptional("System2HTTPRequest.SetData");
        MarkNativeAsOptional("System2HTTPRequest.GetData");
        MarkNativeAsOptional("System2HTTPRequest.SetHeader");
        MarkNativeAsOptional("System2HTTPRequest.GetHeader");
        MarkNativeAsOptional("System2HTTPRequest.GetHeadersCount");
        MarkNativeAsOptional("System2HTTPRequest.GetHeadersArray");
        MarkNativeAsOptional("System2HTTPRequest.SetUserAgent");
        MarkNativeAsOptional("System2HTTPRequest.SetBasicAuthentication");
        MarkNativeAsOptional("System2HTTPRequest.GET");
        MarkNativeAsOptional("System2HTTPRequest.POST");
        MarkNativeAsOptional("System2HTTPRequest.PUT");
        MarkNativeAsOptional("System2HTTPRequest.PATCH");
        MarkNativeAsOptional("System2HTTPRequest.DELETE");
        MarkNativeAsOptional("System2HTTPRequest.HEAD");
        MarkNativeAsOptional("System2HTTPRequest.FollowRedirects.get");
        MarkNativeAsOptional("System2HTTPRequest.FollowRedirects.set");
        MarkNativeAsOptional("System2HTTPRequest.AutoReferer.get");
        MarkNativeAsOptional("System2HTTPRequest.AutoReferer.set");
        
        MarkNativeAsOptional("System2FTPRequest.System2FTPRequest");
        MarkNativeAsOptional("System2FTPRequest.SetProgressCallback");
        MarkNativeAsOptional("System2FTPRequest.SetAuthentication");
        MarkNativeAsOptional("System2FTPRequest.Request");
        MarkNativeAsOptional("System2FTPRequest.Upload");
        MarkNativeAsOptional("System2FTPRequest.AppendToFile.get");
        MarkNativeAsOptional("System2FTPRequest.AppendToFile.set");
        MarkNativeAsOptional("System2FTPRequest.CreateMissingDirs.get");
        MarkNativeAsOptional("System2FTPRequest.CreateMissingDirs.set");
        
        MarkNativeAsOptional("System2Response.GetLastUrl");
        MarkNativeAsOptional("System2Response.GetContent");
        MarkNativeAsOptional("System2Response.ContentSize.get");
        MarkNativeAsOptional("System2Response.StatusCode.get");
        MarkNativeAsOptional("System2Response.TotalTime.get");
        
        MarkNativeAsOptional("System2HTTPResponse.GetHeader");
        MarkNativeAsOptional("System2HTTPResponse.GetHeadersCount");
        MarkNativeAsOptional("System2HTTPResponse.GetHeadersArray");

        MarkNativeAsOptional("System2_URLEncode");
        MarkNativeAsOptional("System2_URLDecode");

        MarkNativeAsOptional("System2_CopyFile");

        MarkNativeAsOptional("System2_Compress");
        MarkNativeAsOptional("System2_Extract");

        MarkNativeAsOptional("System2_ExecuteThreaded");
        MarkNativeAsOptional("System2_ExecuteFormattedThreaded");
        MarkNativeAsOptional("System2ExecuteOutput.GetOutput");
        MarkNativeAsOptional("System2ExecuteOutput.Size.get");

        MarkNativeAsOptional("System2_Execute");
        MarkNativeAsOptional("System2_ExecuteFormatted");

        MarkNativeAsOptional("System2_GetGameDir");
        MarkNativeAsOptional("System2_GetOS");

        MarkNativeAsOptional("System2_GetStringMD5");
        MarkNativeAsOptional("System2_GetFileMD5");
        MarkNativeAsOptional("System2_GetStringCRC32");
        MarkNativeAsOptional("System2_GetFileCRC32");

        // Deprecated v2 stuff
        MarkNativeAsOptional("System2_GetPage");
        MarkNativeAsOptional("System2_DownloadFile");
        MarkNativeAsOptional("System2_DownloadFTPFile");
        MarkNativeAsOptional("System2_UploadFTPFile");
        MarkNativeAsOptional("System2_CompressFile");
        MarkNativeAsOptional("System2_ExtractArchive");
        MarkNativeAsOptional("System2_RunThreadCommand");
        MarkNativeAsOptional("System2_RunThreadCommandWithData");
        MarkNativeAsOptional("System2_RunCommand");
    }
#endif