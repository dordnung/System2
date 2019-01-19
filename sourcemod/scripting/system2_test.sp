/**
 * -----------------------------------------------------
 * File        system2_test.sp
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

// This file is really ugly, but it do what it should do - testing :)

#include <sourcemod>
#include <profiler>
#include <system2>


char path[PLATFORM_MAX_PATH + 1];
char testDownloadFilePath[PLATFORM_MAX_PATH + 1];
char testDownloadFtpFile[PLATFORM_MAX_PATH + 1];
char testFileCopyFromPath[PLATFORM_MAX_PATH + 1];
char testFileCopyToPath[PLATFORM_MAX_PATH + 1];
char testFileToCompressPath[PLATFORM_MAX_PATH + 1];
char testFileHashes[PLATFORM_MAX_PATH + 1];
char testArchivePath[PLATFORM_MAX_PATH + 1];

char longPage[4300];
int finishedCallbacks = 0;
bool isRunning = false;
Handle runningTimer = INVALID_HANDLE;

// Stuff which will be tested
enum TestMethods
{
    TEST_COPY,

    TEST_LONG,
    TEST_BODY,
    TEST_AGENT,
    TEST_FOLLOW,
    TEST_NOT_FOLLOW,
    TEST_TIMEOUT,
    TEST_AUTH,
    TEST_METHOD,
    TEST_HEADER,
    TEST_DEFLATE,
    TEST_VERIFY_SSL,
    TEST_NOT_VERIFY_SSL,
    TEST_DOWNLOAD,
    TEST_PROXY,
    
    TEST_FTP_DIRECTORY,
    TEST_FTP_DOWNLOAD,
    TEST_FTP_UPLOAD,

    TEST_COMPRESS,
    TEST_EXTRACT,
    TEST_EXECUTE,
}


public void OnPluginStart() {
    BuildPath(Path_SM, path, sizeof(path), "data/system2/temp");
    RegServerCmd("system2_test", OnTest);
}


public Action OnTest(int args) {
    if (isRunning) {
        PrintToServer("Tests are already running. Please wait...");
        return Plugin_Handled;
    }

    // Max 30 seconds
    isRunning = true;
    runningTimer = CreateTimer(30.0, OnHardReset);

    // Set needed files to random names
    Format(testDownloadFilePath, sizeof(testDownloadFilePath), "%s/testFile_%d.txt", path, GetURandomInt());
    Format(testDownloadFtpFile, sizeof(testDownloadFtpFile), "%s/testFtpFile_%d.zip", path, GetURandomInt());
    Format(testFileCopyFromPath, sizeof(testFileCopyFromPath), "%s/testCopyFromFile_%d.txt", path, GetURandomInt());
    Format(testFileCopyToPath, sizeof(testFileCopyToPath), "%s/testCopyToFile_%d.txt", path, GetURandomInt());
    Format(testFileToCompressPath, sizeof(testFileToCompressPath), "%s/testCompressFile_%d.txt", path, GetURandomInt());
    Format(testFileHashes, sizeof(testFileHashes), "%s/testMD5_%d.txt", path, GetURandomInt());
    Format(testArchivePath, sizeof(testArchivePath), "%s/testCompressFile_%d.zip", path, GetURandomInt());

    // Create test structure
    if (!DirExists(path)) {
        CreateDirectory(path, 493);
    }

    File file = OpenFile(testFileCopyFromPath, "w");
    file.WriteString("This is a copied file. Content should be equal.", false);
    file.Close();

    file = OpenFile(testFileHashes, "w");
    file.WriteString("This is a test string for hashes", false);
    file.Close();

    finishedCallbacks = 0;

    PrintToServer("");
    PrintToServer("");
    PrintToServer("Testing system2...");
    PrintToServer("---------------------------");
    PrintToServer("");
    PrintToServer("");

    PerformTests();

    PrintToServer("");
    PrintToServer("Still testing callbacks...");
    PrintToServer("---------------------------");
    PrintToServer("");

    CheckCallbacksCalled();

    return Plugin_Handled;
}


public void TestLegacy() {
    finishedCallbacks = 0;

    PrintToServer("");
    PrintToServer("");
    PrintToServer("Testing legacy system2...");
    PrintToServer("---------------------------");
    PrintToServer("");
    PrintToServer("");

    PerformLegacyTests();

    PrintToServer("");
    PrintToServer("Still testing callbacks...");
    PrintToServer("---------------------------");
    PrintToServer("");

    CheckLegacyCallbacksCalled();
}


void PerformTests() {
    Handle profiler = CreateProfiler();
    StartProfiling(profiler);

    // Showing the game dir
    char gameDir[PLATFORM_MAX_PATH + 1];
    System2_GetGameDir(gameDir, sizeof(gameDir));
    PrintToServer("INFO: The game dir is %s", gameDir);

    // Test URL encode
    PrintToServer("INFO: Test URL encode a string");

    char urlEncodeString[16] = "te st";
    assertTrue("URL encode should be successful", System2_URLEncode(urlEncodeString, sizeof(urlEncodeString), "%s test", urlEncodeString));
    assertStringEquals("te%20st%20test", urlEncodeString);

    // Test URL decode
    PrintToServer("INFO: Test URL decode a string");

    char urlDecodeString[16] = "te%20st";
    assertTrue("URL decode should be successful", System2_URLDecode(urlDecodeString, sizeof(urlDecodeString), "%s%%20test", urlDecodeString));
    assertStringEquals("te st test", urlDecodeString);

    // Test copying a file is successful
    PrintToServer("INFO: Test copying a file");
    System2_CopyFile(CopyFileCallback, testFileCopyFromPath, testFileCopyToPath, TEST_COPY);

    // Test 7-zip is available
    PrintToServer("INFO: Test 7-zip is available");

    char binDir[PLATFORM_MAX_PATH];
    assertTrue("7-ZIP should be available", System2_Check7ZIP(binDir, sizeof(binDir), true));
    if (System2_GetOS() == OS_WINDOWS) {
        assertTrue("7-ZIP bin dir should end with 7z.exe", StrContains(binDir, "data\\system2\\win\\7z.exe") > -1);
    } else {
        assertTrue("7-ZIP bin dir should end with 7z", StrContains(binDir, "data/system2/linux/i386/7z") > -1);
    }

    // Test compressing a file does work
    PrintToServer("INFO: Test compressing a file");

    File file = OpenFile(testFileToCompressPath, "w");
    file.WriteString("This is a file to compress. Content should be equal.", false);
    file.Close();
    assertTrue("7-ZIP should be available", System2_Compress(ExecuteCallback, testFileToCompressPath, testArchivePath, ARCHIVE_ZIP, LEVEL_9, TEST_COMPRESS));

    // Test execute a threaded command
    PrintToServer("INFO: Test execute a threaded command");
    System2_ExecuteThreaded(ExecuteCallback, "echo thisIsATestCommand", TEST_EXECUTE);
    System2_ExecuteFormattedThreaded(ExecuteCallback, TEST_EXECUTE, "echo %s", "thisIsATestCommand");

    // Test executing a non threaded command
    char output[128];
    PrintToServer("INFO: Test execute a non threaded command");
    assertTrue("Executing a command should be successful", System2_Execute(output, sizeof(output), "echo thisIsANonThreadedTestCommand"));
    TrimString(output);
    assertStringEquals("thisIsANonThreadedTestCommand", output);

    assertTrue("Executing a command should be successful", System2_ExecuteFormatted(output, sizeof(output), "echo %s", "thisIsANonFormattedThreadedTestCommand"));
    TrimString(output);
    assertStringEquals("thisIsANonFormattedThreadedTestCommand", output);

    // Test request stuff
    PerformRequestTests();

    // Assert GetOs does not return unkown
    PrintToServer("INFO: Test OS is defined");
    OS os = System2_GetOS();
    assertValueNotEquals(view_as<int>(OS:OS_UNKNOWN), view_as<int>(os));
    PrintToServer("INFO: The OS is: %s", (os == OS:OS_WINDOWS) ? "Windows" : ((os == OS:OS_UNIX) ? "Linux" : "MAC"));

    // Assert calculating MD5 hash of a string
    char md5[33];
    PrintToServer("INFO: Test MD5 hash of a string");

    System2_GetStringMD5("This is a test string for hashes", md5, sizeof(md5));
    assertStringEquals("4070731cd404d301ab8621b4cb805362", md5);

    // Assert calculating MD5 hash of a file
    char fileMD5[33];
    PrintToServer("INFO: Test MD5 hash of a file");
    
    assertTrue("Getting the MD5 hash of a file should be successful", System2_GetFileMD5(testFileHashes, fileMD5, sizeof(fileMD5)));
    assertStringEquals("4070731cd404d301ab8621b4cb805362", fileMD5);

    // Assert calculating CRC32 hash of a string
    char crc32[9];
    PrintToServer("INFO: Test CRC32 hash of a string");

    System2_GetStringCRC32("This is a test string for hashes", crc32, sizeof(crc32));
    assertStringEquals("c627da91", crc32);

    // Assert calculating CRC32 hash of a file
    char filecCRC32[9];
    PrintToServer("INFO: Test CRC32 hash of a file");
    
    assertTrue("Getting the CRC32 hash of a file should be successful", System2_GetFileCRC32(testFileHashes, filecCRC32, sizeof(filecCRC32)));
    assertStringEquals("c627da91", filecCRC32);

    StopProfiling(profiler);

    // Successfull
    PrintToServer(" ");
    PrintToServer("Executed system2 non threaded tests successfully in %.2f ms!",  GetProfilerTime(profiler) * 1000.0);

    profiler.Close();
}

void PerformRequestTests() {
    // Test long page
    PrintToServer("INFO: Test getting a long page");
    System2HTTPRequest httpRequest = new System2HTTPRequest(HttpResponseCallback, "https://dordnung.de/sourcemod/system2/testPage.php?%s", "long");
    httpRequest.Timeout = 60;
    httpRequest.Any = TEST_LONG;
    httpRequest.GET();

    // Test body data
    PrintToServer("INFO: Test send body data");
    httpRequest.Any = TEST_BODY;
    httpRequest.SetURL("https://dordnung.de/sourcemod/system2/testPage.php?%s", "body");
    httpRequest.SetData("test=%s", "testData");
    httpRequest.POST();
    httpRequest.SetData("");

    // Test user agent
    PrintToServer("INFO: Test user agent is set");
    httpRequest.Any = TEST_AGENT;
    httpRequest.SetURL("https://dordnung.de/sourcemod/system2/testPage.php?agent");
    httpRequest.SetUserAgent("System2Test%s", "UserAgent");
    httpRequest.GET();
    httpRequest.SetUserAgent("");

    // Test not follow redirects
    PrintToServer("INFO: Test not follow redirects");
    httpRequest.Any = TEST_NOT_FOLLOW;
    httpRequest.SetURL("https://dordnung.de/sourcemod/system2/testPage.php?follow");
    httpRequest.FollowRedirects = false;
    httpRequest.GET();

    // Test follow redirects
    PrintToServer("INFO: Test follow redirects");
    httpRequest.Any = TEST_FOLLOW;
    httpRequest.SetURL("https://dordnung.de/sourcemod/system2/testPage.php?follow");
    httpRequest.FollowRedirects = true;
    httpRequest.GET();

    // Test timeout
    PrintToServer("INFO: Test timeout for request");
    httpRequest.Any = TEST_TIMEOUT;
    httpRequest.SetURL("https://dordnung.de/sourcemod/system2/testPage.php?timeout");
    httpRequest.Timeout = 2;
    httpRequest.GET();
    httpRequest.Timeout = 60;

    // Test auth
    PrintToServer("INFO: Test basic auth");
    httpRequest.Any = TEST_AUTH;
    httpRequest.SetURL("https://dordnung.de/sourcemod/system2/testPage.php?auth");
    httpRequest.SetBasicAuthentication("testUsername", "testPassword");
    httpRequest.GET();
    httpRequest.SetBasicAuthentication("", "");

    // Test GET
    PrintToServer("INFO: Test GET method");
    httpRequest.Any = TEST_METHOD;
    httpRequest.SetURL("https://dordnung.de/sourcemod/system2/testPage.php?method");
    httpRequest.GET();

    // Test POST
    PrintToServer("INFO: Test POST method");
    httpRequest.POST();

    // Test PUT
    PrintToServer("INFO: Test PUT method");
    httpRequest.PUT();

    // Test PATCH
    PrintToServer("INFO: Test PATCH method");
    httpRequest.PATCH();

    // Test DELETE
    PrintToServer("INFO: Test DELETE method");
    httpRequest.DELETE();

    // Test HEAD
    PrintToServer("INFO: Test HEAD method");
    httpRequest.HEAD();

    // Test Header
    httpRequest.Any = TEST_HEADER;
    PrintToServer("INFO: Test set header on a request");
    httpRequest.SetURL("https://dordnung.de/sourcemod/system2/testPage.php?header");
    httpRequest.SetHeader("System2Test", "%s", "ATestHeader");
    httpRequest.GET();
    httpRequest.SetHeader("System2Test", "");

    // Test deflate
    httpRequest.Any = TEST_DEFLATE
    PrintToServer("INFO: Test get deflated content");
    httpRequest.SetURL("https://dordnung.de/sourcemod/system2/testPage.php?deflate");
    httpRequest.SetHeader("Accept-Encoding", "deflate");
    httpRequest.GET();
    httpRequest.SetHeader("Accept-Encoding", "");

    // Test verify ssl
    PrintToServer("INFO: Test verifying ssl");
    httpRequest.Any = TEST_VERIFY_SSL;
    httpRequest.SetURL("https://www.unsecure.com/");
    httpRequest.GET();

    // Test not verify ssl
    PrintToServer("INFO: Test not verifying ssl");
    httpRequest.Any = TEST_NOT_VERIFY_SSL;
    httpRequest.SetURL("https://www.wiki.com/");
    httpRequest.SetVerifySSL(false);
    httpRequest.GET();
    httpRequest.SetVerifySSL(true);

    // Test Proxy
    httpRequest.Any = TEST_PROXY;
    PrintToServer("INFO: Test using a proxy");
    httpRequest.SetURL("https://dordnung.de/sourcemod/system2/testPage.php?method");
    httpRequest.SetProxy("http://dordnung.de:8888");
    httpRequest.SetProxyAuthentication("system2", "test");
    httpRequest.GET();

    // Test Download
    httpRequest.Any = TEST_DOWNLOAD;
    PrintToServer("INFO: Test downloading a file");
    httpRequest.SetURL("https://dordnung.de/sourcemod/system2/testFile.txt");
    httpRequest.SetOutputFile("%s", testDownloadFilePath);
    httpRequest.SetProgressCallback(HttpProgressCallback);
    httpRequest.GET();

    // Delete the request
    delete httpRequest;
    
    // Test FTP directory listing
    PrintToServer("INFO: Test list a FTP directory");
    System2FTPRequest ftpRequest = new System2FTPRequest(ftpResponseCallback, "%s://speedtest.tele2.net/", "ftp");
    ftpRequest.Any = TEST_FTP_DIRECTORY;
    ftpRequest.ListFilenamesOnly = true;
    ftpRequest.SetPort(21);
    ftpRequest.StartRequest();
    ftpRequest.ListFilenamesOnly = false;

    // Test downloading a FTP File
    PrintToServer("INFO: Test downloading a file from FTP");
    ftpRequest.Any = TEST_FTP_DOWNLOAD;
    ftpRequest.AppendToFile = false;
    ftpRequest.CreateMissingDirs = true;
    ftpRequest.SetURL("speedtest.tele2.net/1MB.zip");
    ftpRequest.SetPort(21);
    ftpRequest.SetOutputFile(testDownloadFtpFile);
    ftpRequest.SetProgressCallback(ftpProgressCallback);
    ftpRequest.StartRequest();

    // Delete the request
    delete ftpRequest;
}

void PerformLegacyTests() {
    Handle profiler = CreateProfiler();
    StartProfiling(profiler);

    // Assert GetPage works, also test user agent, post data
    PrintToServer("INFO: Test getting a simple test page with set user agent");
    System2_GetPage(LegacyCommandCallback, "https://dordnung.de/sourcemod/system2/testPage.php?agent", "", "testUseragent", TEST_AGENT);

    PrintToServer("INFO: Test getting a simple test page by POST");
    System2_GetPage(LegacyCommandCallback, "https://dordnung.de/sourcemod/system2/testPage.php?body", "test=testData", "", TEST_BODY);

    PrintToServer("INFO: Test getting a long test page");
    strcopy(longPage, sizeof(longPage), "");
    System2_GetPage(LegacyCommandCallback, "https://dordnung.de/sourcemod/system2/testPage.php?long", "", "", TEST_LONG);

    // Test download file is successful
    PrintToServer("INFO: Test downloading a file");
    System2_DownloadFile(LegacyProgressCallback, "https://dordnung.de/sourcemod/system2/testFile.txt", testDownloadFilePath, TEST_DOWNLOAD);

    // Test downloading a FTP File
    PrintToServer("INFO: Test downloading a file from FTP");
    System2_DownloadFTPFile(LegacyProgressCallback, "1MB.zip", testDownloadFtpFile, "speedtest.tele2.net", "", "", 21, TEST_FTP_DOWNLOAD);

    // Test compressing a file does work
    PrintToServer("INFO: Test compressing a file");

    File file = OpenFile(testFileToCompressPath, "w");
    file.WriteString("This is a file to compress. Content should be equal.", false);
    file.Close();
    System2_CompressFile(LegacyCommandCallback, testFileToCompressPath, testArchivePath, ARCHIVE_ZIP, LEVEL_9, TEST_COMPRESS);

    // Test running a threaded command
    PrintToServer("INFO: Test run a threaded command");
    System2_RunThreadCommandWithData(LegacyCommandCallback, TEST_EXECUTE, "echo thisIsATestCommand");

    // Test running a non threaded command
    char output[128];
    PrintToServer("INFO: Test run a non threaded command");
    assertValueEquals(view_as<int>(CMDReturn:CMD_SUCCESS), view_as<int>(System2_RunCommand(output, sizeof(output), "echo thisIsANonThreadedTestCommand")));
    TrimString(output);
    assertStringEquals("thisIsANonThreadedTestCommand", output);

    StopProfiling(profiler);

    // Successfull
    PrintToServer(" ");
    PrintToServer("Executed legacy system2 non threaded tests successfully in %.2f ms!",  GetProfilerTime(profiler) * 1000.0);

    profiler.Close();
}


/** CALLBACKS */

void CopyFileCallback(bool success, const char[] from, const char[] to, any data) {
    PrintToServer("INFO: Got file copy callback");
    finishedCallbacks++;

    assertValueEquals(view_as<int>(TEST_COPY), data);
    assertValueEquals(true, success);
    assertStringEquals(testFileCopyFromPath, from);
    assertStringEquals(testFileCopyToPath, to);

    assertTrue("Copying a file should create a new file ;)", FileExists(testFileCopyToPath));

    char fileData[256];

    File file = OpenFile(testFileCopyToPath, "r");
    file.ReadString(fileData, sizeof(fileData));
    file.Close();

    assertStringEquals("This is a copied file. Content should be equal.", fileData);
}

void ExecuteCallback(bool success, const char[] command, System2ExecuteOutput output, any data) {
    finishedCallbacks++;

    if (data == TEST_COMPRESS) {
        PrintToServer("INFO: Got compress callback, now extract it again");

        assertTrue("Compressing should work", success);
        assertValueEquals(0, output.ExitStatus);

        assertTrue("Compressing should create an archive", FileExists(testArchivePath));

        // Now extract it again
        DeleteFile(testFileToCompressPath);
        assertTrue("7-ZIP should be available", System2_Extract(ExecuteCallback, testArchivePath, path, TEST_EXTRACT));
    } else if (data == TEST_EXTRACT) {
        PrintToServer("INFO: Got extract callback");

        assertTrue("Extracting should work", success);
        assertValueEquals(0, output.ExitStatus);

        assertTrue("Extracting a file should create a new file ;)", FileExists(testFileToCompressPath));

        char fileData[256];

        File file = OpenFile(testFileToCompressPath, "r");
        file.ReadString(fileData, sizeof(fileData));
        file.Close();

        assertStringEquals("This is a file to compress. Content should be equal.", fileData);
        DeleteFile(testArchivePath);
    } else if (data == TEST_EXECUTE) {
        PrintToServer("INFO: Got execute callback: %s", command);

        assertTrue("Executing a command should work", success);
        assertValueEquals(0, output.ExitStatus);
        assertStringEquals("echo thisIsATestCommand", command);

        char output2[32];
        assertValueEquals(19, output.GetOutput(output2, sizeof(output2)));
        assertValueEquals(strlen(output2), output.Length);

        TrimString(output2);
        assertStringEquals("thisIsATestCommand", output2);

        // Test offset
        assertValueEquals(15, output.GetOutput(output2, sizeof(output2), 4));
        TrimString(output2);
        assertStringEquals("IsATestCommand", output2);

        // Test delimiter
        assertValueEquals(7, output.GetOutput(output2, sizeof(output2), 4, "Command", false));
        TrimString(output2);
        assertStringEquals("IsATest", output2);

        // Test delimiter with include
        assertValueEquals(14, output.GetOutput(output2, sizeof(output2), 4, "Command"));
        TrimString(output2);
        assertStringEquals("IsATestCommand", output2);

        // Test short offset
        char output3[3];
        assertValueEquals(2, output.GetOutput(output3, sizeof(output3), 4));
        TrimString(output3);
        assertStringEquals("Is", output3);
    }
}

void HttpResponseCallback(bool success, const char[] error, System2HTTPRequest request, System2HTTPResponse response, HTTPRequestMethod method) {
    finishedCallbacks++;

    // Timeout and verifiy SSL requests should fail
    if (request.Any == TEST_TIMEOUT || request.Any == TEST_VERIFY_SSL) {
        assertFalse("An error was expected", success);
        assertValueNotEquals(0, strlen(error));

        if (request.Any == TEST_TIMEOUT) {
            assertValueEquals(2, request.Timeout);
        } else {
            assertValueEquals(60, request.Timeout);
        }

        return;
    }

    assertTrue("Callback should be successful", success);
    assertStringEquals("", error);
    assertValueEquals(view_as<int>(VERSION_1_1), view_as<int>(response.HTTPVersion));

    char url[64];
    char lastUrl[64];
    char output[256];
    char contentType[64];

    request.GetURL(url, sizeof(url));
    response.GetLastURL(lastUrl, sizeof(lastUrl));
    response.GetContentType(contentType, sizeof(contentType));
    int responseBytes = response.GetContent(output, sizeof(output));

    if (request.Any != TEST_DOWNLOAD) {
        assertValueEquals(0, StrContains(contentType, "text/html"));
    }

    if (request.Any == TEST_LONG) {
        PrintToServer("INFO: Got long callback in %.3fs", response.TotalTime);

        char longOutput[4239];
        assertValueEquals(4238, response.ContentLength);
        assertValueEquals(4238, response.GetContent(longOutput, sizeof(longOutput)));
        assertValueEquals(255, responseBytes);

        assertValueEquals(view_as<int>(METHOD_GET), view_as<int>(method));
        assertStringEquals("https://dordnung.de/sourcemod/system2/testPage.php?long", url);
        assertStringEquals("https://dordnung.de/sourcemod/system2/testPage.php?long", lastUrl);
        assertValueEquals(200, response.StatusCode);

        for (int i = 0; i < 4238; i++) {
            asserCharEquals((i % 26) + 97, longOutput[i]);
        }

        // Test delimiter
        assertValueEquals(25, response.GetContent(longOutput, sizeof(longOutput), 0, "z", false));
        assertStringEquals("abcdefghijklmnopqrstuvwxy", longOutput);

        // Test delimiter with include
        assertValueEquals(26, response.GetContent(longOutput, sizeof(longOutput), 0, "z"));
        assertStringEquals("abcdefghijklmnopqrstuvwxyz", longOutput);

        // Test offset
        assertValueEquals(3, response.GetContent(longOutput, sizeof(longOutput), 23, "z"));
        assertStringEquals("xyz", longOutput);
    } else if (request.Any == TEST_BODY) {
        PrintToServer("INFO: Got body callback in %.3fs", response.TotalTime);

        char data[32];
        request.GetData(data, sizeof(data));
        assertStringEquals("test=testData", data);

        assertValueEquals(view_as<int>(METHOD_POST), view_as<int>(method));
        assertStringEquals("https://dordnung.de/sourcemod/system2/testPage.php?body", url);
        assertStringEquals("https://dordnung.de/sourcemod/system2/testPage.php?body", lastUrl);
        assertValueEquals(13, responseBytes);
        assertValueEquals(200, response.StatusCode);
        assertValueEquals(strlen(output), response.ContentLength);
        assertStringEquals("test=testData", output);
    } else if (request.Any == TEST_AGENT) {
        PrintToServer("INFO: Got useragent callback in %.3fs", response.TotalTime);

        assertValueEquals(view_as<int>(METHOD_GET), view_as<int>(method));
        assertStringEquals("https://dordnung.de/sourcemod/system2/testPage.php?agent", url);
        assertStringEquals("https://dordnung.de/sourcemod/system2/testPage.php?agent", lastUrl);
        assertValueEquals(20, responseBytes);
        assertValueEquals(200, response.StatusCode);
        assertValueEquals(strlen(output), response.ContentLength);
        assertStringEquals("System2TestUserAgent", output);
    } else if (request.Any == TEST_METHOD) {
        if (method == METHOD_GET) {
            PrintToServer("INFO: Got GET method callback in %.3fs", response.TotalTime);
            assertValueEquals(3, responseBytes);
            assertStringEquals("GET", output);
        } else if (method == METHOD_POST) {
            PrintToServer("INFO: Got POST method callback in %.3fs", response.TotalTime);
            assertValueEquals(4, responseBytes);
            assertStringEquals("POST", output);
        } else if (method == METHOD_PUT) {
            PrintToServer("INFO: Got PUT method callback in %.3fs", response.TotalTime);
            assertValueEquals(3, responseBytes);
            assertStringEquals("PUT", output);
        } else if (method == METHOD_DELETE) {
            PrintToServer("INFO: Got DELETE method callback in %.3fs", response.TotalTime);
            assertValueEquals(6, responseBytes);
            assertStringEquals("DELETE", output);
        } else if (method == METHOD_PATCH) {
            PrintToServer("INFO: Got PATCH method callback in %.3fs", response.TotalTime);
            assertValueEquals(5, responseBytes);
            assertStringEquals("PATCH", output);
        } else if (method == METHOD_HEAD) {
            PrintToServer("INFO: Got HEAD method callback in %.3fs", response.TotalTime);

            char headerValue[64];
            assertTrue("There should be a header System2Head", response.GetHeader("System2Head", headerValue, sizeof(headerValue)));
            assertStringEquals("true", headerValue);
        }

        assertStringEquals("https://dordnung.de/sourcemod/system2/testPage.php?method", url);
        assertStringEquals("https://dordnung.de/sourcemod/system2/testPage.php?method", lastUrl);
        assertValueEquals(200, response.StatusCode);
        assertValueEquals(strlen(output), response.ContentLength);
    } else if (request.Any == TEST_AUTH) {
        PrintToServer("INFO: Got auth callback in %.3fs", response.TotalTime);

        assertValueEquals(view_as<int>(METHOD_GET), view_as<int>(method));
        assertStringEquals("https://dordnung.de/sourcemod/system2/testPage.php?auth", url);
        assertStringEquals("https://dordnung.de/sourcemod/system2/testPage.php?auth", lastUrl);
        assertValueEquals(25, responseBytes);
        assertValueEquals(200, response.StatusCode);
        assertValueEquals(strlen(output), response.ContentLength);
        assertStringEquals("testUsername:testPassword", output);
    } else if (request.Any == TEST_DEFLATE) {
        PrintToServer("INFO: Got deflate callback in %.3fs", response.TotalTime);

        assertValueEquals(view_as<int>(METHOD_GET), view_as<int>(method));
        assertStringEquals("https://dordnung.de/sourcemod/system2/testPage.php?deflate", url);
        assertStringEquals("https://dordnung.de/sourcemod/system2/testPage.php?deflate", lastUrl);
        assertValueEquals(24, responseBytes);
        assertValueEquals(200, response.StatusCode);
        assertValueEquals(strlen(output), response.ContentLength);
        assertStringEquals("This is deflated content", output);

        char headerValue[32];
        assertTrue("There should be a header Accept-Encoding", request.GetHeader("Accept-Encoding", headerValue, sizeof(headerValue)));
    } else if (request.Any == TEST_FOLLOW) {
        PrintToServer("INFO: Got follow callback in %.3fs", response.TotalTime);

        assertValueEquals(view_as<int>(METHOD_GET), view_as<int>(method));
        assertStringEquals("https://dordnung.de/sourcemod/system2/testPage.php?follow", url);
        assertStringEquals("https://dordnung.de/sourcemod/system2/testPage.php?followed", lastUrl);
        assertValueEquals(0, responseBytes);
        assertValueEquals(200, response.StatusCode);
        assertValueEquals(strlen(output), response.ContentLength);

        assertTrue("Follow redirect should be enabled", request.FollowRedirects);

        char headerValue[32];
        assertFalse("There should be no header System2Follow", response.GetHeader("System2Follow", headerValue, sizeof(headerValue)));
        assertTrue("There should be a header System2Followed", response.GetHeader("System2Followed", headerValue, sizeof(headerValue)));
        assertStringEquals("true", headerValue);

        // Test get headers
        ArrayList headers = response.GetHeaders();
        
        bool found = false;
        char headerName[256];
        for (int i=0; i < headers.Length; i++) {
            headers.GetString(i, headerName, sizeof(headerName));
            if (StrEqual(headerName, "System2Followed", false)) {
                found = true;
            }
        }

        delete headers;
        assertTrue("There should be a header System2Followed in all headers", found);
    } else if (request.Any == TEST_NOT_FOLLOW) {
        PrintToServer("INFO: Got not follow callback in %.3fs", response.TotalTime);

        assertValueEquals(view_as<int>(METHOD_GET), view_as<int>(method));
        assertStringEquals("https://dordnung.de/sourcemod/system2/testPage.php?follow", url);
        assertStringEquals("https://dordnung.de/sourcemod/system2/testPage.php?follow", lastUrl);
        assertValueEquals(0, responseBytes);
        assertValueEquals(301, response.StatusCode);
        assertValueEquals(strlen(output), response.ContentLength);

        assertFalse("Follow redirect should be disabled", request.FollowRedirects);

        char headerValue[64];
        assertTrue("There should be a header System2Follow", response.GetHeader("System2Follow", headerValue, sizeof(headerValue)));
        assertStringEquals("true", headerValue);
        assertTrue("There should be a header Location", response.GetHeader("Location", headerValue, sizeof(headerValue)));
        assertStringEquals("http://dordnung.de/sourcemod/system2/testPage.php?followed", headerValue);
    } else if (request.Any == TEST_HEADER) {
        PrintToServer("INFO: Got header callback in %.3fs", response.TotalTime);

        assertValueEquals(view_as<int>(METHOD_GET), view_as<int>(method));
        assertStringEquals("https://dordnung.de/sourcemod/system2/testPage.php?header", url);
        assertStringEquals("https://dordnung.de/sourcemod/system2/testPage.php?header", lastUrl);
        assertValueEquals(11, responseBytes);
        assertValueEquals(200, response.StatusCode);
        assertValueEquals(strlen(output), response.ContentLength);
        assertStringEquals("ATestHeader", output);

        // Test get header value
        char headerValue[32];
        assertTrue("There should be a header System2Test", request.GetHeader("System2Test", headerValue, sizeof(headerValue)));
        assertStringEquals("ATestHeader", headerValue);

        // Test GetHeaders
        ArrayList headers = request.GetHeaders();

        bool found = false;
        char headerName[256];
        for (int i=0; i < headers.Length; i++) {
            headers.GetString(i, headerName, sizeof(headerName));
            if (StrEqual(headerName, "System2Test", false)) {
                found = true;
            }
        }

        delete headers;
        assertTrue("There should be a header System2Test in all headers", found);
    } else if (request.Any == TEST_NOT_VERIFY_SSL) {
        PrintToServer("INFO: Got not verifying SSL callback in %.3fs", response.TotalTime);

        assertValueEquals(view_as<int>(METHOD_GET), view_as<int>(method));
        assertStringEquals("https://www.wiki.com/", url);
        assertStringEquals("https://www.wiki.com/", lastUrl);
        assertValueEquals(200, response.StatusCode);
        assertFalse("SSL verifying should be disabled", request.GetVerifySSL());
    } else if (request.Any == TEST_PROXY) {
        PrintToServer("INFO: Got proxy GET method callback in %.3fs", response.TotalTime);
        assertValueEquals(3, responseBytes);
        assertStringEquals("GET", output);

        assertStringEquals("https://dordnung.de/sourcemod/system2/testPage.php?method", url);
        assertStringEquals("https://dordnung.de/sourcemod/system2/testPage.php?method", lastUrl);
        assertValueEquals(200, response.StatusCode);
        assertValueEquals(strlen(output), response.ContentLength);
    } else if (request.Any == TEST_DOWNLOAD) {
        PrintToServer("INFO: Got download callback in %.3fs", response.TotalTime);

        assertValueEquals(view_as<int>(METHOD_GET), view_as<int>(method));
        assertStringEquals("https://dordnung.de/sourcemod/system2/testFile.txt", url);
        assertStringEquals("https://dordnung.de/sourcemod/system2/testFile.txt", lastUrl);
        assertValueEquals(45, responseBytes);
        assertValueEquals(200, response.StatusCode);
        assertValueEquals(strlen(output), response.ContentLength);
        assertStringEquals("This is a test file. Content should be equal.", output);
        assertValueEquals(45, response.DownloadSize);
        assertTrue("Download speed should be more then 0 bytes/s", response.DownloadSpeed > 0);

        // Test correct GetOutputFile
        char fileData[64];
        request.GetOutputFile(fileData, sizeof(fileData));
        assertStringEquals(testDownloadFilePath, fileData);

        // Test correct content in file
        File file = OpenFile(testDownloadFilePath, "r");
        file.ReadString(fileData, sizeof(fileData));
        file.Close();

        DeleteFile(testDownloadFilePath);
        assertStringEquals("This is a test file. Content should be equal.", fileData);
    }
}

void HttpProgressCallback(System2HTTPRequest request, int dlTotal, int dlNow, int ulTotal, int ulNow) {
    // Test correct URL
    char url[64];
    request.GetURL(url, sizeof(url));
    assertStringEquals("https://dordnung.de/sourcemod/system2/testFile.txt", url);
    
    // Test correct values
    assertValueEquals(45, dlTotal);
    assertValueEquals(0, ulNow);
    assertValueEquals(0, ulTotal);

    // Test correct GetOutputFile
    char fileData[64];
    request.GetOutputFile(fileData, sizeof(fileData));
    assertStringEquals(testDownloadFilePath, fileData);
}


void ftpResponseCallback(bool success, const char[] error, System2FTPRequest request, System2FTPResponse response) {
    finishedCallbacks++;

    assertTrue("Callback should be successful", success);
    assertStringEquals("", error);

    char url[64];
    char lastUrl[64];
    char output[191];
    request.GetURL(url, sizeof(url));
    response.GetLastURL(lastUrl, sizeof(lastUrl));
    int responseBytes = response.GetContent(output, sizeof(output));

    if (request.Any == TEST_FTP_DIRECTORY) {
        PrintToServer("INFO: Got FTP directory listening callback in %.3fs", response.TotalTime);

        assertStringEquals("ftp://speedtest.tele2.net/", url);
        assertStringEquals("ftp://speedtest.tele2.net/", lastUrl);
        assertValueEquals(226, response.StatusCode);
        assertValueNotEquals(0, response.ContentLength);
        assertValueEquals(true, request.ListFilenamesOnly);
    } else if (request.Any == TEST_FTP_DOWNLOAD) {
        PrintToServer("INFO: Got FTP download callback in %.3fs, uploading it again", response.TotalTime);
   
        assertStringEquals("speedtest.tele2.net/1MB.zip", url);
        assertStringEquals("ftp://speedtest.tele2.net/1MB.zip", lastUrl);
        assertValueEquals(21, request.GetPort());
        assertValueEquals(226, response.StatusCode);
        assertValueEquals(0, response.UploadSpeed);
        assertValueEquals(0, response.UploadSize);
        assertValueEquals(response.ContentLength, response.DownloadSize);
        assertTrue("Download size should be more then 0 bytes", response.DownloadSize > 0);
        assertTrue("Download speed should be more then 0 bytes/s", response.DownloadSpeed > 0);

        // Test correct GetOutputFile
        char fileData[64];
        request.GetOutputFile(fileData, sizeof(fileData));
        assertStringEquals(testDownloadFtpFile, fileData);

        assertTrue("Downloading a FTP file should create a new file ;)", FileExists(testDownloadFtpFile));
        request.Any = TEST_FTP_UPLOAD;
        request.SetURL("ftp://speedtest.tele2.net/upload/system2.zip");
        request.SetOutputFile("");
        request.SetInputFile("%s", testDownloadFtpFile);
        request.StartRequest();
    } else if (request.Any == TEST_FTP_UPLOAD) {
        PrintToServer("INFO: Got FTP upload callback in %.3fs", response.TotalTime);
   
        assertStringEquals("ftp://speedtest.tele2.net/upload/system2.zip", url);
        assertStringEquals("ftp://speedtest.tele2.net/upload/system2.zip", lastUrl);
        assertValueEquals(21, request.GetPort());
        assertValueEquals(0, responseBytes);
        assertValueEquals(226, response.StatusCode);
        assertValueEquals(strlen(output), response.ContentLength);
        assertValueEquals(false, request.AppendToFile);
        assertValueEquals(true, request.CreateMissingDirs);
        assertValueEquals(0, response.DownloadSpeed);
        assertValueEquals(0, response.DownloadSize);
        assertTrue("Upload size should be more then 0 bytes", response.UploadSize > 0);
        assertTrue("Upload speed should be more then 0 bytes/s", response.UploadSpeed > 0);

        // Test correct GetInputFile
        char file[64];
        request.GetInputFile(file, sizeof(file));
        assertStringEquals(testDownloadFtpFile, file);
    }
}

void ftpProgressCallback(System2FTPRequest request, int dlTotal, int dlNow, int ulTotal, int ulNow) {
    // Test correct URL
    char url[64];
    request.GetURL(url, sizeof(url));

    if (request.Any == TEST_FTP_DOWNLOAD) {
        assertStringEquals("speedtest.tele2.net/1MB.zip", url);
        assertTrue("Download size should be more then 0 bytes", dlTotal > 0);
        assertValueEquals(0, ulNow);
        assertValueEquals(0, ulTotal);

        // Test correct GetOutputFile
        char file[64];
        request.GetOutputFile(file, sizeof(file));
        assertStringEquals(testDownloadFtpFile, file);
    } else if (request.Any == TEST_FTP_UPLOAD) {
        assertStringEquals("ftp://speedtest.tele2.net/upload/system2.zip", url);
        assertTrue("Upload size should be more then 0 bytes", ulTotal > 0);
        assertValueEquals(0, dlNow);
        assertValueEquals(0, dlTotal);

        // Test correct GetInputFile
        char file[64];
        request.GetInputFile(file, sizeof(file));
        assertStringEquals(testDownloadFtpFile, file);
    }
}


/** LEGACY CALLBACKS */

void LegacyProgressCallback(bool finished, const char[] error, float dltotal, float dlnow, float ultotal, float ulnow, any data) {
    assertStringEquals("", error);

    if (data == TEST_DOWNLOAD) {
        assertTrue("Downloading a file should create a new file ;)", FileExists(testDownloadFilePath));
    }

    if (finished) {
        finishedCallbacks++;

        if (data == TEST_DOWNLOAD) {
            PrintToServer("INFO: Got download callback");

            char fileData[256];

            File file = OpenFile(testDownloadFilePath, "r");
            file.ReadString(fileData, sizeof(fileData));
            file.Close();

            DeleteFile(testDownloadFilePath);
            assertStringEquals("This is a test file. Content should be equal.", fileData);
        } else if (data == TEST_FTP_DOWNLOAD) {
            PrintToServer("INFO: Got FTP download callback, uploading it again");

            assertTrue("Downloading a FTP file should create a new file ;)", FileExists(testDownloadFtpFile));
            System2_UploadFTPFile(LegacyProgressCallback, testDownloadFtpFile, "upload/system2.zip", "speedtest.tele2.net", "", "", 21, TEST_FTP_UPLOAD);
        } else if (data == TEST_FTP_UPLOAD) {
            PrintToServer("INFO: Got FTP upload callback");
        }
    }
}

void LegacyCommandCallback(const char[] output, const int size, CMDReturn status, any data, const char[] command) {
    assertValueEquals(strlen(output) + 1, size);

    if (data == TEST_AGENT) {
        PrintToServer("INFO: Got useragent callback");
        finishedCallbacks++;

        assertStringEquals("", command);
        assertValueEquals(view_as<int>(CMDReturn:CMD_SUCCESS), view_as<int>(status));
        assertStringEquals("testUseragent", output);
    } else if (data == TEST_BODY) {
        PrintToServer("INFO: Got body callback");
        finishedCallbacks++;

        assertStringEquals("", command);
        assertValueEquals(view_as<int>(CMDReturn:CMD_SUCCESS), view_as<int>(status));
        assertStringEquals("test=testData", output);
    } else if (data == TEST_LONG) {
        PrintToServer("INFO: Got long callback");

        assertStringEquals("", command);
        StrCat(longPage, sizeof(longPage), output);

        if (strlen(longPage) < 4238) {
            assertValueEquals(view_as<int>(CMDReturn:CMD_PROGRESS), view_as<int>(status));
        } else {
            finishedCallbacks++;

            assertValueEquals(view_as<int>(CMDReturn:CMD_SUCCESS), view_as<int>(status));
            assertValueEquals(4238, strlen(longPage));

            for (int i = 0; i < 4238; i++) {
                asserCharEquals((i % 26) + 97, longPage[i]);
            }
        }
    } else if (data == TEST_COMPRESS) {
        PrintToServer("INFO: Got compress callback");
        finishedCallbacks++;

        assertTrue("Compressing a file should create a new file ;)", FileExists(testArchivePath));
        assertValueEquals(view_as<int>(CMDReturn:CMD_SUCCESS), view_as<int>(status));

        // Now extract it again
        DeleteFile(testFileToCompressPath);
        System2_ExtractArchive(LegacyCommandCallback, testArchivePath, path, TEST_EXTRACT);
    } else if (data == TEST_EXTRACT) {
        PrintToServer("INFO: Got extract callback");
        finishedCallbacks++;

        assertTrue("Decompressing a file should create a new file ;)", FileExists(testFileToCompressPath));
        assertValueEquals(view_as<int>(CMDReturn:CMD_SUCCESS), view_as<int>(status));

        char fileData[256];

        File file = OpenFile(testFileToCompressPath, "r");
        file.ReadString(fileData, sizeof(fileData));
        file.Close();

        assertStringEquals("This is a file to compress. Content should be equal.", fileData);
    } else if (data == TEST_EXECUTE) {
        PrintToServer("INFO: Got execute callback: %s", command);
        finishedCallbacks++;

        assertStringEquals("echo thisIsATestCommand", command);
        assertValueEquals(view_as<int>(CMDReturn:CMD_SUCCESS), view_as<int>(status));

        char output2[128];
        strcopy(output2, sizeof(output2), output);
        TrimString(output2);
        assertStringEquals("thisIsATestCommand", output2);
    }
}


/** TIMERS */

int timesTimerCalled = 0;
void CheckCallbacksCalled() {
    // Wait max. 20 seconds for all callbacks
    timesTimerCalled = 0;
    CreateTimer(1.0, OnCheckCallbacks, false, TIMER_REPEAT);
}

void CheckLegacyCallbacksCalled() {
    // Wait max. 20 seconds for all callbacks
    timesTimerCalled = 0;
    CreateTimer(1.0, OnCheckCallbacks, true, TIMER_REPEAT);
}

public Action OnCheckCallbacks(Handle timer, any isLegacy) {
    int callbacks = isLegacy ? 9 : 27;

    // Wait max. 20 seconds for all callbacks
    if (timesTimerCalled >= 20) {
        KillTimer(timer);
        assertValueEquals(callbacks, finishedCallbacks);

        if (runningTimer != INVALID_HANDLE) {
            KillTimer(runningTimer);
            runningTimer = INVALID_HANDLE;
        }

        isRunning = false;
        return Plugin_Stop;
    } else if (finishedCallbacks == callbacks) {
        PrintToServer("---------------------------");
        PrintToServer("INFO: All callbacks were called");
        if (!isLegacy) {
            KillTimer(timer);
            TestLegacy();
        } else {
            if (runningTimer != INVALID_HANDLE) {
                KillTimer(runningTimer);
                runningTimer = INVALID_HANDLE;
            }
            isRunning = false;
        }

        return Plugin_Stop;
    }

    timesTimerCalled++;
    return Plugin_Continue;
}

public Action OnHardReset(Handle timer) {
    isRunning = false;
    runningTimer = INVALID_HANDLE;

    return Plugin_Stop;
}


/** ASSERT STUFF */

stock void assertTrue(const char[] message, bool value) {
    if (!value) {
        ThrowError("FAILURE: %s. Expected true, but was false", message);
    }
}

stock void assertFalse(const char[] message, bool value) {
    if (value) {
        ThrowError("FAILURE: %s. Expected false, but was true", message);
    }
}

stock void asserCharEquals(const char expected, const char actual) {
    if (expected != actual) {
        ThrowError("FAILURE: expected '%s', found '%s'", expected, actual);
    }
}

stock void assertStringEquals(const char[] expected, const char[] actual) {
    if (!StrEqual(expected, actual)) {
        ThrowError("FAILURE: expected '%s', found '%s'", expected, actual);
    }
}

stock void assertValueEquals(int expected, int actual) {
    if (expected != actual) {
        ThrowError("FAILURE: expected '%d', found '%d'", expected, actual);
    }
}

stock void assertValueNotEquals(int notExpected, int actual) {
    if (notExpected == actual) {
        ThrowError("FAILURE: '%d' was not expected", actual);
    }
}