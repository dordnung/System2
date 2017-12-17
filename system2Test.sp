#include <sourcemod>
#include <profiler>
#include "system2"


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

public void OnPluginStart() {
	RegServerCmd("test_system2", OnTest);
	
	// Create test structure
	BuildPath(Path_SM, path, sizeof(path), "data/system2/temp");
}


public Action OnTest(int args) {
	// Set new file names
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

	// Test copying a file is successful
	PrintToServer("INFO: Copying a file");
	System2_CopyFile(CopyFileCallback, testFileCopyFromPath, testFileCopyToPath, 10);

	// Test compressing a file does work
	PrintToServer("INFO: Compressing a file");

	File file = OpenFile(testFileToCompressPath, "w");
	file.WriteString("This is a file to compress. Content should be equal.", false);
	file.Close();
	System2_Compress(CompressCallback, testFileToCompressPath, testArchivePath, ARCHIVE_ZIP, LEVEL_9, 48);

	// Test execute a threaded command
	PrintToServer("INFO: Execute a threaded command");
	System2_ExecuteThreaded(ExecuteCallback, "echo thisIsATestCommand", 86);
	System2_ExecuteFormattedThreaded(ExecuteCallback, 86, "echo %s", "thisIsATestCommand");

	// Test executing a non threaded command
	char output[128];
	PrintToServer("INFO: Execute a non threaded command");
	assertTrue("Executing a command should be successful", System2_Execute(output, sizeof(output), "echo thisIsANonThreadedTestCommand"));
	TrimString(output);
	assertStringEquals("thisIsANonThreadedTestCommand", output);

	assertTrue("Executing a command should be successful", System2_ExecuteFormatted(output, sizeof(output), "echo %s", "thisIsANonFormattedThreadedTestCommand"));
	TrimString(output);
	assertStringEquals("thisIsANonFormattedThreadedTestCommand", output);

	// Assert GetOs does not return unkown
	PrintToServer("INFO: Testing OS is defined");
	assertValueNotEquals(view_as<int>(OS:OS_UNKNOWN), view_as<int>(System2_GetOS()));

	// Assert calculating MD5 hash of a string
	char md5[33];
	PrintToServer("INFO: Testing MD5 hash of a string");

	System2_GetStringMD5("This is a test string for hashes", md5, sizeof(md5));
	assertStringEquals("4070731cd404d301ab8621b4cb805362", md5);

	// Assert calculating MD5 hash of a file
	char fileMD5[33];
	PrintToServer("INFO: Testing MD5 hash of a file");
	
	assertTrue("Getting the MD5 hash of a file should be successful", System2_GetFileMD5(testFileHashes, fileMD5, sizeof(fileMD5)));
	assertStringEquals("4070731cd404d301ab8621b4cb805362", fileMD5);

	// Assert calculating CRC32 hash of a string
	char crc32[9];
	PrintToServer("INFO: Testing CRC32 hash of a string");

	System2_GetStringCRC32("This is a test string for hashes", crc32, sizeof(crc32));
	assertStringEquals("c627da91", crc32);

	// Assert calculating CRC32 hash of a file
	char filecCRC32[9];
	PrintToServer("INFO: Testing CRC32 hash of a file");
	
	assertTrue("Getting the CRC32 hash of a file should be successful", System2_GetFileCRC32(testFileHashes, filecCRC32, sizeof(filecCRC32)));
	assertStringEquals("c627da91", filecCRC32);

	StopProfiling(profiler);

	// Successfull
	PrintToServer(" ");
	PrintToServer("Executed system2 non threaded tests successfully in %.2f ms!",  GetProfilerTime(profiler) * 1000.0);

	profiler.Close();
}


void PerformLegacyTests() {
	Handle profiler = CreateProfiler();
	StartProfiling(profiler);

	// Assert GetPage works, also test user agent, post data and the any parameter
	PrintToServer("INFO: Getting a simple test page with set user agent");
	System2_GetPage(GetPageCallbackUserAgent, "http://dordnung.de/sourcemod/system2/testPage.php", "", "testUseragent", 5);

	PrintToServer("INFO: Getting a simple test page by POST");
	System2_GetPage(GetPageCallbackPost, "http://dordnung.de/sourcemod/system2/testPage.php", "test=testData");

	PrintToServer("INFO: Getting a long test page");
	strcopy(longPage, sizeof(longPage), "");
	System2_GetPage(GetPageLongCallback, "http://dordnung.de/sourcemod/system2/testPage.php?test");

	// Test download file is successful
	PrintToServer("INFO: Downloading a file");
	System2_DownloadFile(DownloadFileCallback, "http://dordnung.de/sourcemod/system2/testFile.txt", testDownloadFilePath, 8);

	// Test downloading a FTP File
	PrintToServer("INFO: Downloading a file from FTP");
	System2_DownloadFTPFile(DownloadFtpFileCallback, "1MB.zip", testDownloadFtpFile, "speedtest.tele2.net", "", "", 21, 678);

	// Test compressing a file does work
	PrintToServer("INFO: Compressing a file");

	File file = OpenFile(testFileToCompressPath, "w");
	file.WriteString("This is a file to compress. Content should be equal.", false);
	file.Close();
	System2_CompressFile(CompressFileCallback, testFileToCompressPath, testArchivePath, ARCHIVE_ZIP, LEVEL_9, 35);

	// Test running a threaded command
	PrintToServer("INFO: Run a threaded command");
	System2_RunThreadCommandWithData(CommandCallback, 86, "echo thisIsATestCommand");

	// Test running a non threaded command
	char output[128];
	PrintToServer("INFO: Run a non threaded command");
	assertValueEquals(view_as<int>(CMDReturn:CMD_SUCCESS), view_as<int>(System2_RunCommand(output, sizeof(output), "echo thisIsANonThreadedTestCommand")));
	TrimString(output);
	assertStringEquals("thisIsANonThreadedTestCommand", output);

	StopProfiling(profiler);

	// Successfull
	PrintToServer(" ");
	PrintToServer("Executed legacy system2 non threaded tests successfully in %.2f ms!",  GetProfilerTime(profiler) * 1000.0);

	profiler.Close();
}

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
	int callbacks = isLegacy ? 9 : 5;

	// Wait max. 20 seconds for all callbacks
	if (timesTimerCalled >= 20) {
		KillTimer(timer);
		assertValueEquals(callbacks, finishedCallbacks);
		return Plugin_Stop;
	} else if (finishedCallbacks == callbacks) {
		PrintToServer("---------------------------");
		PrintToServer("INFO: All callbacks were called");
		if (!isLegacy) {
			KillTimer(timer);
			TestLegacy();
		}
		return Plugin_Stop;
	}

	timesTimerCalled++;
	return Plugin_Continue;
}


void CopyFileCallback(bool success, const char[] from, const char[] to, any data) {
	PrintToServer("INFO: Copied a file");
	finishedCallbacks++;

	assertValueEquals(10, data);
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

void CompressCallback(bool success, System2ExecuteOutput output, const char[] command, any data) {
	PrintToServer("INFO: Compressed a file, now extract it again");
	finishedCallbacks++;

	assertTrue("Compressing should work", success);
	assertValueEquals(48, data);

	assertTrue("Compressing should create an archive", FileExists(testArchivePath));

	// Now extract it again
	DeleteFile(testFileToCompressPath);
	System2_Extract(ExtractCallback, testArchivePath, path, 234);
}

void ExtractCallback(bool success, System2ExecuteOutput output, const char[] command, any data) {
	PrintToServer("INFO: Extracted a file");
	finishedCallbacks++;

	assertTrue("Extracting should work", success);
	assertValueEquals(234, data);

	assertTrue("Extracting a file should create a new file ;)", FileExists(testFileToCompressPath));

	char fileData[256];

	File file = OpenFile(testFileToCompressPath, "r");
	file.ReadString(fileData, sizeof(fileData));
	file.Close();

	assertStringEquals("This is a file to compress. Content should be equal.", fileData);
	DeleteFile(testArchivePath);
}

void ExecuteCallback(bool success, System2ExecuteOutput output, const char[] command, any data) {
	PrintToServer("INFO: Executed the threaded command %s", command);
	finishedCallbacks++;

	assertTrue("Executing a command should work", success);
	assertValueEquals(86, data);
	assertStringEquals("echo thisIsATestCommand", command);

	char output2[128];
	output.GetOutput(output2, sizeof(output2));
	assertValueEquals(strlen(output2), output.Size);

	TrimString(output2);
	assertStringEquals("thisIsATestCommand", output2);

	output.GetOutput(output2, sizeof(output2), 4);
	TrimString(output2);
	assertStringEquals("IsATestCommand", output2);
}


void GetPageCallbackUserAgent(const char[] output, const int size, CMDReturn status, any data, const char[] command) {
	PrintToServer("INFO: Got a page with set user agent");
	finishedCallbacks++;

	assertValueEquals(view_as<int>(CMDReturn:CMD_SUCCESS), view_as<int>(status));
	assertValueEquals(strlen(output) + 1, size);
	assertValueEquals(5, data);
	assertStringEquals("testUseragent", output);
	assertStringEquals("", command);
}

void GetPageCallbackPost(const char[] output, const int size, CMDReturn status) {
	PrintToServer("INFO: Got a page by POST");
	finishedCallbacks++;

	assertValueEquals(view_as<int>(CMDReturn:CMD_SUCCESS), view_as<int>(status));
	assertValueEquals(strlen(output) + 1, size);
	assertStringEquals("testData", output);
}

void GetPageLongCallback(const char[] output, const int size, CMDReturn status) {
	PrintToServer("INFO: Got a long page with size %d (%d)", size, strlen(longPage));

	StrCat(longPage, sizeof(longPage), output);

	if (strlen(longPage) < 4238) {
		assertValueEquals(view_as<int>(CMDReturn:CMD_PROGRESS), view_as<int>(status));
		assertValueEquals(strlen(output) + 1, size);
	} else {
		finishedCallbacks++;

		assertValueEquals(view_as<int>(CMDReturn:CMD_SUCCESS), view_as<int>(status));
		assertValueEquals(strlen(output) + 1, size);
		assertValueEquals(4238, strlen(longPage));

		for (int i = 0; i < 4238; i++) {
			asserCharEquals((i % 26) + 97, longPage[i]);
		}
	}
}

void DownloadFileCallback(bool finished, const char[] error, float dltotal, float dlnow, float ultotal, float ulnow, any data) {
	assertValueEquals(8, data);
	assertStringEquals("", error);

	assertTrue("Downloading a file should create a new file ;)", FileExists(testDownloadFilePath));

	if (finished) {
		PrintToServer("INFO: Finished downloading a file");
		finishedCallbacks++;

		char fileData[256];

		File file = OpenFile(testDownloadFilePath, "r");
		file.ReadString(fileData, sizeof(fileData));
		file.Close();

		assertStringEquals("This is a test file. Content should be equal.", fileData);
	}
}

void DownloadFtpFileCallback(bool finished, const char[] error, float dltotal, float dlnow, float ultotal, float ulnow, any data) {
	assertValueEquals(678, data);

	// This division is just for the stacktrace line
	if (finished) {
		finishedCallbacks++;

		assertStringEquals("", error);
		PrintToServer("INFO: Finished downloading a file from FTP, uploading it again");

		assertTrue("Downloading a FTP file should create a new file ;)", FileExists(testDownloadFtpFile));
		System2_UploadFTPFile(UploadFtpFileCallback, testDownloadFtpFile, "upload/system2.zip", "speedtest.tele2.net", "", "", 21, 555);
	} else {
		assertStringEquals("", error);
	}
}

void UploadFtpFileCallback(bool finished, const char[] error, float dltotal, float dlnow, float ultotal, float ulnow, any data) {
	assertValueEquals(555, data);

	// This division is just for the stacktrace line
	if (finished) {
		PrintToServer("INFO: Finished uploading a file to FTP");
		finishedCallbacks++;

		assertStringEquals("", error);
	} else {
		assertStringEquals("", error);
	}
}

void CompressFileCallback(const char[] output, const int size, CMDReturn status, any data, const char[] command) {
	PrintToServer("INFO: Compressed a file, now extract it again");
	finishedCallbacks++;

	assertValueEquals(view_as<int>(CMDReturn:CMD_SUCCESS), view_as<int>(status));
	assertValueEquals(35, data);

	assertTrue("Compressing a file should create a new file ;)", FileExists(testArchivePath));

	// Now extract it again
	DeleteFile(testFileToCompressPath);
	System2_ExtractArchive(ExtractArchiveCallback, testArchivePath, path, 43);
}

void ExtractArchiveCallback(const char[] output, const int size, CMDReturn status, any data) {
	PrintToServer("INFO: Extracted a file");
	finishedCallbacks++;

	assertValueEquals(view_as<int>(CMDReturn:CMD_SUCCESS), view_as<int>(status));
	assertValueEquals(43, data);

	assertTrue("Decompressing a file should create a new file ;)", FileExists(testFileToCompressPath));

	char fileData[256];

	File file = OpenFile(testFileToCompressPath, "r");
	file.ReadString(fileData, sizeof(fileData));
	file.Close();

	assertStringEquals("This is a file to compress. Content should be equal.", fileData);
}

void CommandCallback(const char[] output, const int size, CMDReturn status, any data, const char[] command) {
	PrintToServer("INFO: Called the threaded command %s", command);
	finishedCallbacks++;

	assertValueEquals(view_as<int>(CMDReturn:CMD_SUCCESS), view_as<int>(status));
	assertValueEquals(86, data);
	assertStringEquals("echo thisIsATestCommand", command);

	char output2[128];
	strcopy(output2, sizeof(output2), output);
	TrimString(output2);
	assertStringEquals("thisIsATestCommand", output2);
}

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