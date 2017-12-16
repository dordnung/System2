/**
 * -----------------------------------------------------
 * File        command.h
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

#ifndef _LEGACY_COMMAND_H_
#define _LEGACY_COMMAND_H_

#include "extension.h"

#define MAX_RESULT_LENGTH 4096

 // Define Posix
#if defined  _WIN32
#define PosixOpen _popen
#define PosixClose _pclose

 //#pragma warning(disable: 4996)
#else
#define PosixOpen popen
#define PosixClose pclose
#endif


enum CommandState {
    CMD_SUCCESS,
    CMD_EMPTY,
    CMD_ERROR,
    CMD_PROGRESS
};


class LegacyCommandCallback : public Callback {
private:
    std::string output;
    std::string command;
    int data;

    IPluginFunction *callback;
    CommandState state;

public:
    LegacyCommandCallback(std::string output, std::string command, int data, IPluginFunction *callback, CommandState state);

    virtual void Fire();
};


class LegacyCommandThread : public IThread {
private:
    std::string command;
    IPluginFunction *callback;
    int data;

public:
    LegacyCommandThread(std::string command, IPluginFunction *callback, int data);

    void RunThread(IThreadHandle *pThread);
    void OnTerminate(IThreadHandle *pThread, bool cancel) {
        delete this;
    }
};

#endif