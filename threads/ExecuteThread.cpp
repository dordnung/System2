/**
 * -----------------------------------------------------
 * File        ExecuteThread.cpp
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

#include "ExecuteThread.h"
#include "ExecuteCallback.h"


ExecuteThread::ExecuteThread(std::string command, int data, std::shared_ptr<CallbackFunction_t> callbackFunction)
    : Thread(), command(command), data(data), callbackFunction(callbackFunction) {}

void ExecuteThread::Run() {
    bool success = true;
    std::string output;
    int exitStatus;

    // Execute the command
    FILE *commandFile = PosixOpen(this->command.c_str(), "r");
    if (commandFile != NULL) {
        char buffer[1024];
        while (fgets(buffer, sizeof(buffer), commandFile) != NULL) {
            // Add buffer to the output
            output += buffer;
        }

        // Close
        exitStatus = PosixClose(commandFile);
    } else {
        success = false;
    }

    // Add return status to queue
    system2Extension.AppendCallback(std::make_shared<ExecuteCallback>(this->callbackFunction, success, exitStatus, output, this->command, this->data));
}