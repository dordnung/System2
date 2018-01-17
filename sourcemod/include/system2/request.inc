/**
 * -----------------------------------------------------
 * File        request.inc
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

#if defined _system2_request_included
    #endinput
#endif

#define _system2_request_included


/**
 *
 * API for making HTTP and FTP requests.
 *
 */


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
 *
 * @noreturn
 */
typeset System2FTPResponseCallback
{
    function void (bool success, const char[] error, System2FTPRequest request, System2FTPResponse response);
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
         * @param millis    The timeout for the request in milliseconds. 0 to disable.
         *
         * @noreturn
         * @error           Invalid Request.
         */
        public native set(int millis);
    }

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
     * Retrieves the name of a header at a given index.
     * Use GetHeadersCount to retrieve the maximum index.
     *
     * @param index     Index of the header name.
     * @param name      Buffer to store name in.
     * @param maxlength Maxlength of the buffer.
     *
     * @return          True if header was found, otherwise false.
     * @error           Invalid Response.
     */
    public native bool GetHeaderName(int index, char[] name, int maxlength);

    /**
     * Returns the number of set headers.
     *
     * @return          The number of set headers.
     * @error           Invalid Request.
     */
    public native int GetHeadersCount();

    /**
     * Returns all header names of the request as ArrayList.
     * Attention: ArrayList has to be deleted after use!

     * @param maxlength Maxlength of a header.
     *
     * @return          ArrayList with all request header names.
     * @error           Invalid Request.
     */
    public ArrayList GetHeaders(int maxlength = 256) {
        ArrayList headers = new ArrayList(maxlength);

        char[] headerName = new char[maxlength];
        for (int i=0; i < this.GetHeadersCount(); i++) {
            this.GetHeaderName(i, headerName, maxlength);
            headers.PushString(headerName);
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
     * Sets the file to upload to a FTP server.
     * If this is set, an upload approach will be used.
     *
     * @param file      File to upload.
     *
     * @noreturn
     * @error           Invalid Request.
     */
    public native void SetInputFile(const char[] file);

    /**
     * Retrieves the file which should be uploaded.
     *
     * @param file      Buffer to store file in.
     * @param maxlength Maxlength of the buffer.
     *
     * @noreturn
     * @error           Invalid Request.
     */
    public native void GetInputFile(char[] file, int maxlength);


    /**
     * Starts the request.
     *
     * If a input file is set the request will upload this file.
     * Otherwise it may result in a directory listing or a file download.
     *
     * @noreturn
     * @error           Invalid Request.
     */
    public native void StartRequest();


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

    property bool ListFilenamesOnly {
        /**
         * Returns whether only file names should be fetched for directory listing.
         *
         * @return          True if only file names should be fetched
         * @error           Invalid Request.
         */
        public native get();

        /**
         * Sets whether only file names should be fetched for directory listing.
         *
         * @param append    True it only file names should be fetched, otherwise false for full directory listing.
         *
         * @noreturn
         * @error           Invalid Request.
         */
        public native set(bool append);
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
    public native void GetLastURL(char[] url, int maxlength);

    /**
     * Retrieves the content of the response.
     * This shouldn't be used when retrieved binary stuff.
     *
     * @param content   Buffer to store the content in.
     * @param maxlength Maxlength of the content buffer.
     * @param start     Start byte to start reading from.
     *                  You can use this to retrieve the content step by step.
     * @param delimiter Delimiter until which the content should be retrieved.
     *
     * @return          Number of unread bytes. 0 if all bytes were read.
     * @error           Invalid Response.
     */
    public native int GetContent(char[] content, int maxlength, int start = 0, const char[] delimiter = "");


    property int ContentLength {
        /**
         * Returns the length of the complete content.
         *
         * @return      Length of the content.
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
     * Retrieves the name of a header at a given index.
     * Use GetHeadersCount to retrieve the maximum index.
     *
     * @param index     Index of the header name.
     * @param name      Buffer to store name in.
     * @param maxlength Maxlength of the buffer.
     *
     * @return          True if header was found, otherwise false.
     * @error           Invalid Response.
     */
    public native bool GetHeaderName(int index, char[] name, int maxlength);

    /**
     * Returns the number of set headers.
     *
     * @return          The number of set headers.
     * @error           Invalid Response.
     */
    public native int GetHeadersCount();

    /**
     * Returns all header names of the response as ArrayList.
     * Attention: ArrayList has to be deleted after use!

     * @param maxlength Maxlength of a header.
     *
     * @return          ArrayList with all response header names.
     * @error           Invalid Request.
     */
    public ArrayList GetHeaders(int maxlength = 256) {
        ArrayList headers = new ArrayList(maxlength);

        char[] headerName = new char[maxlength];
        for (int i=0; i < this.GetHeadersCount(); i++) {
            this.GetHeaderName(i, headerName, maxlength);
            headers.PushString(headerName);
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