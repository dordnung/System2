/**
 * -----------------------------------------------------
 * File        LegacyDownloadCallback.cpp
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

#include "LegacyDownloadCallback.h"


LegacyDownloadCallback::LegacyDownloadCallback(std::string curlError, int data, IPluginFunction *callback) {
    this->finished = true;
    this->curlError = curlError;
    this->dlTotal = 0.0f;
    this->dlNow = 0.0f;
    this->ulTotal = 0.0f;
    this->ulNow = 0.0f;
    this->data = data;
    this->callback = callback;
}

LegacyDownloadCallback::LegacyDownloadCallback(bool finished, std::string curlError, float dlTotal, float dlNow, float ulTotal, float ulNow, int data, IPluginFunction *callback) {
    this->finished = finished;
    this->curlError = curlError;
    this->dlTotal = dlTotal;
    this->dlNow = dlNow;
    this->ulTotal = ulTotal;
    this->ulNow = ulNow;
    this->data = data;
    this->callback = callback;
}


void LegacyDownloadCallback::Fire() {
    this->callback->PushCell(this->finished);
    this->callback->PushString(this->curlError.c_str());
    this->callback->PushFloat(this->dlTotal);
    this->callback->PushFloat(this->dlNow);
    this->callback->PushFloat(this->ulTotal);
    this->callback->PushFloat(this->ulNow);
    this->callback->PushCell(this->data);
    this->callback->Execute(NULL);
}