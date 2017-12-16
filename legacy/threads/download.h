/**
 * -----------------------------------------------------
 * File        download.h
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

#ifndef _LEGACY_DOWNLOAD_H_
#define _LEGACY_DOWNLOAD_H_

#include "extension.h"

typedef struct {
    uint32_t lastFrame;
    int data;
    IPluginFunction *callback;
} progress_info;


class LegacyDownloadCallback : public Callback {
private:
    bool finished;
    std::string curlError;
    float dlTotal;
    float dlNow;
    float ulTotal;
    float ulNow;
    int data;
    IPluginFunction *callback;

public:
    LegacyDownloadCallback(std::string curlError, int data, IPluginFunction *callback);
    LegacyDownloadCallback(bool finished, std::string curlError, float dlTotal, float dlNow, float ulTotal, float ulNow, int data, IPluginFunction *callback);

    virtual void Fire();
};


class LegacyDownloadThread : public IThread {
private:
    std::string url;
    std::string localFile;
    int data;

    IPluginFunction *callback;

public:
    LegacyDownloadThread(std::string url, std::string localFile, int data, IPluginFunction *callback);

    void RunThread(IThreadHandle *pThread);
    void OnTerminate(IThreadHandle *pThread, bool cancel) {
        delete this;
    }
};

size_t file_write(void *buffer, size_t size, size_t nmemb, void *userdata);
int progress_updated(void *data, double dltotal, double dlnow, double ultotal, double ulnow);

#endif