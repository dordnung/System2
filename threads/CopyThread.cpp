/**
 * -----------------------------------------------------
 * File        CopyThread.cpp
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

#include "CopyThread.h"
#include "CopyCallback.h"

#include <fstream>


CopyThread::CopyThread(IPluginFunction *callback, std::string from, std::string to, int data) : IThread() {
    this->from = from;
    this->to = to;
    this->data = data;
    this->callback = callback;
}


void CopyThread::RunThread(IThreadHandle *pHandle) {
    char filePath[PLATFORM_MAX_PATH + 1];
    char copyPath[PLATFORM_MAX_PATH + 1];

    // Get the full paths to the files
    g_pSM->BuildPath(Path_Game, filePath, sizeof(filePath), this->from.c_str());
    g_pSM->BuildPath(Path_Game, copyPath, sizeof(copyPath), this->to.c_str());

    // Open both files
    std::ifstream file1(filePath, std::ifstream::in | std::ifstream::binary);
    std::ofstream file2(copyPath, std::ofstream::out | std::ofstream::trunc | std::ofstream::binary);

    bool success;
    if (file1.bad() || file2.bad() || !file1.is_open() || !file2.is_open()) {
        // Couldn't open a file
        success = false;
    } else {
        // Copy the file
        file2 << file1.rdbuf();
        success = true;
    }

    // Close the files
    if (file1.is_open()) {
        file1.close();
    }

    if (file2.is_open()) {
        file2.close();
    }

    // Add callback to queue
    system2Extension.AppendCallback(std::make_shared<CopyCallback>(success, this->from, this->to, this->callback, this->data));
}


void CopyThread::OnTerminate(IThreadHandle *pThread, bool cancel) {
    delete this;
}