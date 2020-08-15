/**
 * -----------------------------------------------------
 * File        LegacyCommandThread.cpp
 * Authors     David Ordnung
 * License     GPLv3
 * Web         http://dordnung.de
 * -----------------------------------------------------
 *
 * Copyright (C) 2013-2020 David Ordnung
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

#include "LegacyCommandThread.h"
#include "LegacyCommandCallback.h"
#include "LegacyCommandState.h"


LegacyCommandThread::LegacyCommandThread(std::string command, int data, std::shared_ptr<CallbackFunction_t> callbackFunction)
    : Thread(), command(command), data(data), callbackFunction(callbackFunction) {}


void LegacyCommandThread::Run() {
    // Redirect everything to output
    std::string redirect = " 2>&1";
    std::string realCommand = this->command;
    if (redirect.size() > this->command.size() || !std::equal(redirect.rbegin(), redirect.rend(), this->command.rbegin())) {
        realCommand += redirect;
    }

    // Execute the command
    FILE *commandFile = PosixOpen(realCommand.c_str(), "r");

    LegacyCommandState state = CMD_SUCCESS;
    std::string output;

    // Was there an error?
    if (commandFile) {
        bool found = false;

        char buffer[MAX_RESULT_LENGTH + 1];
        while (fgets(buffer, sizeof(buffer), commandFile)) {
            found = true;

            // More than MAX_RESULT_LENGTH?
            if (output.length() + strlen(buffer) >= MAX_RESULT_LENGTH) {
                // We only can push a string with a length of MAX_RESULT_LENGTH
                system2Extension.AppendCallback(std::make_shared<LegacyCommandCallback>(this->callbackFunction, output, this->command, this->data, CMD_PROGRESS));
                output.clear();
            }

            // Add buffer to result
            output += buffer;
        }

        // Empty result?
        if (output.empty() && !found) {
            output = "Empty reading result!";
            state = CMD_EMPTY;
        }

        // Close
        PosixClose(commandFile);
    } else {
        // Error
        output = "ERROR: Couldn't execute the command!";
        state = CMD_ERROR;
    }

    // Add return status to queue
    system2Extension.AppendCallback(std::make_shared<LegacyCommandCallback>(this->callbackFunction, output, this->command, this->data, state));
}