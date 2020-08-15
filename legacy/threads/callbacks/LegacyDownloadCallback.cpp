/**
 * -----------------------------------------------------
 * File        LegacyDownloadCallback.cpp
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

#include "LegacyDownloadCallback.h"

LegacyDownloadCallback::LegacyDownloadCallback(std::shared_ptr<CallbackFunction_t> callbackFunction, std::string curlError, int data)
    : Callback(callbackFunction), finished(true), curlError(curlError), dlTotal(0.0f), dlNow(0.0f), ulTotal(0.0f), ulNow(0.0f), data(data) {}

LegacyDownloadCallback::LegacyDownloadCallback(std::shared_ptr<CallbackFunction_t> callbackFunction,
                                               bool finished, std::string curlError,
                                               float dlTotal, float dlNow, float ulTotal, float ulNow, int data)
    : Callback(callbackFunction), finished(finished), curlError(curlError), dlTotal(dlTotal), dlNow(dlNow), ulTotal(ulTotal), ulNow(ulNow), data(data) {}

void LegacyDownloadCallback::Fire() {
    this->callbackFunction->function->PushCell(this->finished);
    this->callbackFunction->function->PushString(this->curlError.c_str());
    this->callbackFunction->function->PushFloat(this->dlTotal);
    this->callbackFunction->function->PushFloat(this->dlNow);
    this->callbackFunction->function->PushFloat(this->ulTotal);
    this->callbackFunction->function->PushFloat(this->ulNow);
    this->callbackFunction->function->PushCell(this->data);
    this->callbackFunction->function->Execute(nullptr);
}