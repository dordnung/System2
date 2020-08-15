/**
 * -----------------------------------------------------
 * File        CopyThread.h
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

#ifndef _SYSTEM2_COPY_THREAD_H_
#define _SYSTEM2_COPY_THREAD_H_

#include "extension.h"
#include "Thread.h"

class CopyThread : public Thread {
private:
    std::string from;
    std::string to;
    int data;

    std::shared_ptr<CallbackFunction_t> callbackFunction;

public:
    CopyThread(std::string from, std::string to, int data, std::shared_ptr<CallbackFunction_t> callbackFunction);

protected:
    void Run();
};

#endif