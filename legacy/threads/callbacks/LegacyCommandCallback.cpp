/**
 * -----------------------------------------------------
 * File        LegacyCommandCallback.cpp
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

#include "LegacyCommandCallback.h"


LegacyCommandCallback::LegacyCommandCallback(std::shared_ptr<CallbackFunction_t> callbackFunction, std::string output,
                                             std::string command, int data, LegacyCommandState state)
    : Callback(callbackFunction), output(output), command(command), data(data), state(state) {}

void LegacyCommandCallback::Fire() {
    this->callbackFunction->function->PushString(this->output.c_str());
    this->callbackFunction->function->PushCell(this->output.length() + 1);
    this->callbackFunction->function->PushCell(this->state);
    this->callbackFunction->function->PushCell(this->data);
    this->callbackFunction->function->PushString(this->command.c_str());
    this->callbackFunction->function->Execute(nullptr);
}