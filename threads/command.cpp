/**
 * -----------------------------------------------------
 * File        command.cpp
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

#include "command.h"


CommandThread::CommandThread(char *command, IPluginFunction *callback, int any) : IThread() {
	strcpy(this->command, command);
	this->function = callback;
	this->data = any;
}

void CommandThread::RunThread(IThreadHandle *pHandle) {
	// Create a thread return struct
	ThreadReturn *threadReturn = new ThreadReturn;

	char buffer[MAX_RESULT_LENGTH];

	threadReturn->function = function;
	threadReturn->mode = MODE_COMMAND;
	threadReturn->result = CMD_SUCCESS;
	threadReturn->data = data;

	strcpy(threadReturn->command, this->command);
	strcpy(threadReturn->resultString, "");

	// Prevent output to console
	if (strstr(this->command, "2>&1") == NULL) {
		strcat(this->command, " 2>&1");
	}

	// Execute the command
	FILE *command = PosixOpen(this->command, "r");

	// Was there an error?
	if (command) {
		bool foundOne = false;

		while (fgets(buffer, sizeof(buffer), command) != NULL) {
			foundOne = true;

			// More than MAX_RESULT_LENGTH?
			if (strlen(threadReturn->resultString) + strlen(buffer) >= MAX_RESULT_LENGTH - 1) {
				// We only can push a string with a length of MAX_RESULT_LENGTH
				ThreadReturn *threadReturn2 = new ThreadReturn;

				threadReturn2->function = function;
				threadReturn2->mode = MODE_COMMAND;
				threadReturn2->result = CMD_PROGRESS;
				threadReturn2->data = data;

				strcpy(threadReturn2->command, this->command);
				strcpy(threadReturn2->resultString, threadReturn->resultString);

				// Add return status to queue
				system2Extension.addToQueue(threadReturn2);
				strcpy(threadReturn->resultString, "");
			}

			// Add result to buffer
			strcat(threadReturn->resultString, buffer);
		}

		// Empty result?
		if (strlen(threadReturn->resultString) == 0 && !foundOne) {
			strcpy(threadReturn->resultString, "Empty reading result!");
			threadReturn->result = CMD_EMPTY;
		}

		// Close
		PosixClose(command);
	}
	else {
		// Error
		strcpy(threadReturn->resultString, "ERROR: Couldn't execute the command!");
		threadReturn->result = CMD_ERROR;
	}

	// Add return status to queue
	system2Extension.addToQueue(threadReturn);
}

