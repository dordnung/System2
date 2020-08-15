/**
 * -----------------------------------------------------
 * File        Thread.cpp
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

#include "Thread.h"
#include "extension.h"
#include <memory>


Thread::Thread() : shouldTerminate(false), threader(nullptr) {};

Thread::~Thread() {
    this->TerminateThread();
}

void Thread::RunThread() {
    if (!this->threader) {
        system2Extension.RegisterThread(this);

        this->threader = std::make_unique<std::thread>([this]() -> void {
            this->Run();
            system2Extension.UnregisterThread(this);

            // Delete ourself when finished
            delete this;
        });
    }
}

void Thread::TerminateThread() {
    if (this->threader) {
        {
            std::lock_guard<std::mutex> lock(this->lock);
            this->shouldTerminate = true;
        }

        this->threader->join();
        this->threader = nullptr;
        this->shouldTerminate = false;
    }
}

bool Thread::ShouldTerminate() {
    std::lock_guard<std::mutex> lock(this->lock);
    return this->shouldTerminate;
}