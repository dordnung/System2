/**
 * -----------------------------------------------------
 * File        LegacyFTPThread.h
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

#ifndef _SYSTEM2_LEGACY_FTP_THREAD_H_
#define _SYSTEM2_LEGACY_FTP_THREAD_H_

#include "extension.h"


 // Only allow one FTP connection at the same time, because of RFC does not allow multiple connections
extern IMutex *ftpMutex;

class LegacyFTPThread : public IThread {
private:
    bool download;

    std::string remoteFile;
    std::string localFile;
    std::string host;
    std::string username;
    std::string password;

    int port;
    int data;

    IPluginFunction *callback;

public:
    LegacyFTPThread(bool download, std::string remoteFile, std::string localFile, std::string url, std::string user, std::string pw, int port, int data, IPluginFunction *callback);

    void RunThread(IThreadHandle *pThread);
    void OnTerminate(IThreadHandle *pThread, bool cancel);

    static size_t UploadFTP(void *buffer, size_t size, size_t nmemb, void *userdata);
};

#endif