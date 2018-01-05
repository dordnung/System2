/**
 * -----------------------------------------------------
 * File        FTPRequest.cpp
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

#include "FTPRequest.h"
#include "FTPRequestThread.h"


FTPRequest::FTPRequest(std::string url, IPluginFunction *responseCallback) : Request(url, responseCallback), appendToFile(false), createMissingDirs(true) {};


void FTPRequest::Download(Handle_t requestHandle, IdentityToken_t *owner) {
    this->makeThread(true, requestHandle, owner);
}

void FTPRequest::Upload(Handle_t requestHandle, IdentityToken_t *owner) {
    this->makeThread(false, requestHandle, owner);
}


void FTPRequest::makeThread(bool isDownload, Handle_t requestHandle, IdentityToken_t *owner) {
    FTPRequestThread *requestThread = new FTPRequestThread(this, requestHandle, owner, isDownload);
    threader->MakeThread(requestThread);
}