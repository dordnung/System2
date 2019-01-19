/**
 * -----------------------------------------------------
 * File        system2_http.sp
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

/**
 * This example will retrieve an URL and print information of the server's response.
 * 
 * Usage: system2_http <URL>
 */

#include <sourcemod>
#include <system2>


public void OnPluginStart() {
    RegServerCmd("system2_http", OnCommand);
}


public Action OnCommand(int args) {
    if (args != 1) {
        PrintToServer("Usage: system2_http <URL>");
        return Plugin_Handled;
    }

    char url[256];
    GetCmdArg(1, url, sizeof(url));

    PrintToServer("");
    PrintToServer("INFO: Retrieve URL %s", url);

    System2HTTPRequest httpRequest = new System2HTTPRequest(HttpResponseCallback, url);
    httpRequest.Timeout = 30;
    httpRequest.GET();
    delete httpRequest;

    return Plugin_Handled;
}


void HttpResponseCallback(bool success, const char[] error, System2HTTPRequest request, System2HTTPResponse response, HTTPRequestMethod method) {
    char url[256];
    request.GetURL(url, sizeof(url));

    if (!success) {
        PrintToServer("ERROR: Couldn't retrieve URL %s. Error: %s", url, error);
        PrintToServer("");
        PrintToServer("INFO: Finished");
        PrintToServer("");

        return;
    }

    response.GetLastURL(url, sizeof(url));

    PrintToServer("INFO: Successfully retrieved URL %s in %.0f milliseconds", url, response.TotalTime * 1000.0);
    PrintToServer("");
    PrintToServer("INFO: HTTP Version: %s", (response.HTTPVersion == VERSION_1_0 ? "1.0" : "1.1"));
    PrintToServer("INFO: Status Code: %d", response.StatusCode);
    PrintToServer("INFO: Downloaded %d bytes with %d bytes/seconds", response.DownloadSize, response.DownloadSpeed);
    PrintToServer("INFO: Uploaded %d bytes with %d bytes/seconds", response.UploadSize, response.UploadSpeed);
    PrintToServer("");
    PrintToServer("INFO: Retrieved the following headers:");

    char name[128];
    char value[128];
    ArrayList headers = response.GetHeaders();

    for (int i = 0; i < headers.Length; i++) {
        headers.GetString(i, name, sizeof(name));
        response.GetHeader(name, value, sizeof(value));
        PrintToServer("\t%s: %s", name, value);
    }
    
    PrintToServer("");
    PrintToServer("INFO: Content (%d bytes):", response.ContentLength);
    PrintToServer("");
    
    char content[128];
    for (int found = 0; found < response.ContentLength;) {
        found += response.GetContent(content, sizeof(content), found);
        PrintToServer(content);
    }

    PrintToServer("");
    PrintToServer("INFO: Finished");
    PrintToServer("");
    
    delete headers;
}
