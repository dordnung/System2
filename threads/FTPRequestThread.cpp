/**
 * -----------------------------------------------------
 * File        FTPRequestThread.cpp
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

#include "FTPRequestThread.h"
#include "FTPResponseCallback.h"


 // Only allow one FTP connection at the same time, because of RFC does not allow multiple connections
IMutex *ftpMutex;

FTPRequestThread::FTPRequestThread(FTPRequest *ftpRequest, bool isDownload)
    : RequestThread(ftpRequest), ftpRequest(ftpRequest), isDownload(isDownload) {};


void FTPRequestThread::RunThread(IThreadHandle *pHandle) {
    bool success = true;

    // Add return status to queue
    system2Extension.AppendCallback(std::make_shared<FTPResponseCallback>(success));
}