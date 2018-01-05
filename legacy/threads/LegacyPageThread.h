/**
 * -----------------------------------------------------
 * File        LegacyPageThread.h
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

#ifndef _SYSTEM2_LEGACY_PAGE_THREAD_H_
#define _SYSTEM2_LEGACY_PAGE_THREAD_H_

#include "extension.h"


class LegacyPageThread : public IThread {
private:
    std::string url;
    std::string post;
    std::string useragent;
    int data;

    IPluginFunction *callback;

public:
    typedef struct {
        std::string result;
        int data;
        IPluginFunction *callback;
    } PageInfo;

    LegacyPageThread(std::string url, std::string post, std::string useragent, int data, IPluginFunction *callback);

    void RunThread(IThreadHandle *pThread);
    void OnTerminate(IThreadHandle *pThread, bool cancel);

    static size_t GetPage(void *buffer, size_t size, size_t nmemb, void *userdata);

};

#endif