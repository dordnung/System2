/**
 * -----------------------------------------------------
 * File        copy.cpp
 * Authors     David Ordnung
 * License     GPLv3
 * Web         http://dordnung.de
 * -----------------------------------------------------
 *
 * Copyright (C) 2013-2017 David Ordnung
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

#include "copy.h"


CopyThread::CopyThread(char *localFile, char *destinationPath, IPluginFunction *callback, int any) : IThread() {
	strcpy(this->file, localFile);
	strcpy(this->copyPath, destinationPath);

	this->function = callback;
	this->data = any;
}

void CopyThread::RunThread(IThreadHandle *pHandle) {
	char fullFilePath[PLATFORM_MAX_PATH + 1];
	char fullCopyPath[PLATFORM_MAX_PATH + 1];

	// Create a thread return struct
	ThreadReturn *threadReturn = new ThreadReturn;

	threadReturn->function = function;
	threadReturn->mode = MODE_COPY;
	threadReturn->finished = 1;
	threadReturn->data = data;

	strcpy(threadReturn->copyFrom, this->file);
	strcpy(threadReturn->copyTo, this->copyPath);

	// Get the full paths to the files
	g_pSM->BuildPath(Path_Game, fullFilePath, sizeof(fullFilePath), file);
	g_pSM->BuildPath(Path_Game, fullCopyPath, sizeof(fullCopyPath), copyPath);

	// Open both files
	std::ifstream file1(fullFilePath, std::ifstream::binary);
	std::ofstream file2(fullCopyPath, std::ofstream::trunc | std::ofstream::binary);

	if (file1.bad() || file2.bad()) {
		// Couldn't open a file
		threadReturn->result = CMD_ERROR;
	}
	else {
		// Copy the file
		file2 << file1.rdbuf();
		threadReturn->result = CMD_SUCCESS;
	}

	// Close the files
	if (file1.good()) {
		file1.close();
	}

	if (file2.good()) {
		file2.close();
	}

	// Add return status to queue
	system2Extension.addToQueue(threadReturn);
}
