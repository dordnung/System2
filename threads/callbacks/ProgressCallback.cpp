/**
 * -----------------------------------------------------
 * File        ProgressCallback.cpp
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

#include "ProgressCallback.h"
#include "RequestHandler.h"


ProgressCallback::ProgressCallback(Request *request, int dlTotal, int dlNow, int ulTotal, int ulNow, int data)
    : request(request), dlTotal(dlTotal), dlNow(dlNow), ulTotal(ulTotal), ulNow(ulNow), data(data) {};


void ProgressCallback::Fire() {
    if (request->progressCallback->IsRunnable()) {
        // Create a temporary request handle, so in the callback the correct request will be used
        IdentityToken_t *owner = request->progressCallback->GetParentContext()->GetIdentity();
        Handle_t requestHandle = requestHandler.CreateLocaleHandle<Request>(this->request, owner);

        request->progressCallback->PushCell(requestHandle);
        request->progressCallback->PushCell(this->dlTotal);
        request->progressCallback->PushCell(this->dlNow);
        request->progressCallback->PushCell(this->ulTotal);
        request->progressCallback->PushCell(this->ulNow);
        request->progressCallback->Execute(NULL);

        // Delete the request handle when finished
        if (requestHandle != BAD_HANDLE) {
            requestHandler.FreeHandle(requestHandle, owner);
        }
    } else {
        // The request will only be deleted by the handle, but as it will not be invoked we have to delete it manually
        delete this->request;
    }
}

void ProgressCallback::Abort() {
    // The request will only be deleted by the handle, but as it will not be invoked we have to delete it manually
    delete this->request;
}