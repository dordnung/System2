/**
 * -----------------------------------------------------
 * File        CopyCallback.cpp
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

#include "CopyCallback.h"


CopyCallback::CopyCallback(bool success, std::string from, std::string to, IPluginFunction *callback, int data) {
    this->success = success;
    this->from = from;
    this->to = to;
    this->callback = callback;
    this->data = data;
}


void CopyCallback::Fire() {
    this->callback->PushCell(this->success);
    this->callback->PushString(this->from.c_str());
    this->callback->PushString(this->to.c_str());
    this->callback->PushCell(this->data);
    this->callback->Execute(NULL);
}