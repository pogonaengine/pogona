/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023, Nikita Romanyuk
 */

#pragma once

#include "../defines.h"
#include <config.h>

#define pWINDOW_TITLE_SIZE 64

enum {
	pWINDOW_TYPE_INVALID = -1,
#ifdef pWAYLAND
	pWINDOW_TYPE_WAYLAND,
#endif
#ifdef pXCB
	pWINDOW_TYPE_XCB,
#endif
#ifdef pXLIB
	pWINDOW_TYPE_XLIB,
#endif

	pWINDOW_MAX_TYPES,
};

typedef struct {
	char title[pWINDOW_TITLE_SIZE];
	u32 width, height;
	bool isRunning;
	i8 type;
	void* api; /* managed by the window itself */
} pWindow;

i32 pWindowCreate(pWindow* self, const char* title, u32 width, u32 height);
void pWindowPollEvents(const pWindow* self);
void pWindowDestroy(pWindow* self);
