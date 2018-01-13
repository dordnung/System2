/**
 * -----------------------------------------------------
 * File        FTPRequest.cpp
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

#include "FTPRequest.h"
#include "FTPRequestThread.h"


FTPRequest::FTPRequest(std::string url, IPluginFunction *responseCallback)
    : Request(url, responseCallback), appendToFile(false), createMissingDirs(true) {};

FTPRequest::FTPRequest(const FTPRequest &request) :
    Request(request), username(request.username), password(request.password),
    appendToFile(request.appendToFile), createMissingDirs(request.createMissingDirs) {};


void FTPRequest::MakeRequest() {
    this->MakeThread(std::string());
}

void FTPRequest::Upload(std::string uploadFile) {
    this->MakeThread(uploadFile);
}


void FTPRequest::MakeThread(std::string uploadFile) {
    // Make a copy for the thread, so it works independent
    FTPRequestThread *requestThread = new FTPRequestThread(new FTPRequest(*this), uploadFile);
    threader->MakeThread(requestThread);
}