/**
 * -----------------------------------------------------
 * File        LegacyCommandCallback.h
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

#ifndef _SYSTEM2_LEGACY_COMMAND_CLLBACK_H_
#define _SYSTEM2_LEGACY_COMMAND_CLLBACK_H_

#include "extension.h"
#include "LegacyCommandState.h"
#include "Callback.h"


class LegacyCommandCallback : public Callback {
private:
    std::string output;
    std::string command;
    int data;

    LegacyCommandState state;

public:
    LegacyCommandCallback(std::shared_ptr<CallbackFunction_t> callbackFunction, std::string output, std::string command, int data, LegacyCommandState state);

    virtual void Fire();
};

#endif