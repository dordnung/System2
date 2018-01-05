/**
 * -----------------------------------------------------
 * File        extension.h
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

#ifndef _SYSTEM2_EXTENSION_H_
#define _SYSTEM2_EXTENSION_H_

#include "smsdk_ext.h"
#include "Callback.h"

#include <stdio.h>
#include <string.h>
#include <string>
#include <memory>
#include <queue>

#include <curl/curl.h>


class System2Extension : public SDKExtension {
private:
    IMutex * mutex;
    std::queue<std::shared_ptr<Callback>> callbackQueue;
    uint32_t frames;

public:
    void AppendCallback(std::shared_ptr<Callback> callback);

    void GameFrameHit();
    uint32_t GetFrames() {
        return this->frames;
    }

    virtual bool SDK_OnLoad(char *error, size_t maxlength, bool late);
    virtual void SDK_OnUnload();
};


void OnGameFrameHit(bool simulating);

extern System2Extension system2Extension;

#endif