/**
 * -----------------------------------------------------
 * File        page.h
 * Authors     Popoklopsi, Sourcemod
 * License     GPLv3
 * Web         http://popoklopsi.de
 * -----------------------------------------------------
 *
 * Copyright (C) 2013-2016 Popoklopsi, Sourcemod
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

#ifndef _PAGE_H_
#define _PAGE_H_

#include "extension.h"


class PageThread : public IThread {
private:
	char url[PLATFORM_MAX_PATH + 1];
	char post[PLATFORM_MAX_PATH + 1];
	char useragent[64];

	IPluginFunction *function;
	int data;

public:
	PageThread(char *link, char *postmethod, char *agent, IPluginFunction *callback, int any);

	void RunThread(IThreadHandle *pThread);
	void OnTerminate(IThreadHandle *pThread, bool cancel) {}

};

size_t page_get(void *buffer, size_t size, size_t nmemb, void *stream);

#endif