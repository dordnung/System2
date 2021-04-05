/**
 * -----------------------------------------------------
 * File        FTPResponseCallback.cpp
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

#include "FTPResponseCallback.h"

FTPResponseCallback::FTPResponseCallback(FTPRequest* ftpRequest, std::string error)
    : ResponseCallback(ftpRequest, error) {}

FTPResponseCallback::FTPResponseCallback(FTPRequest* ftpRequest, CURL* curl, std::string content, size_t contentLength)
    : ResponseCallback(ftpRequest, curl, content, contentLength) {}

void FTPResponseCallback::PreFire() {
    // Nothing to do here
}