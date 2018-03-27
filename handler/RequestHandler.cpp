/**
 * -----------------------------------------------------
 * File        RequestHandler.cpp
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

#include "RequestHandler.h"
#include "Request.h"


RequestHandler::RequestHandler() : handleType(0) {}

void RequestHandler::Initialize() {
    this->handleType =
        handlesys->CreateType("System2Request",
                              this,
                              0,
                              NULL,
                              NULL,
                              myself->GetIdentity(),
                              NULL);
}

void RequestHandler::Shutdown() {
    handlesys->RemoveType(this->handleType, myself->GetIdentity());
}

void RequestHandler::OnHandleDestroy(HandleType_t type, void *object) {
    delete (Request *)object;
}


// Create an instance of the request handler
RequestHandler requestHandler;