/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023, Nikita Romanyuk
 */

#pragma once

#include "../defines.h"
#include "window.h"
#include <xcb/xcb.h>

typedef struct {
	pWindow* parent;
	bool isRunning;

	xcb_connection_t* connection;
	xcb_screen_t*     screen;
	xcb_drawable_t    window; /* NOTE: xcb_drawable_t and xcb_window_t is the same thing, named differently just for convenience */
} pXCBWindow;

bool pXCBSupport(void);

i32 pXCBWindowCreate(pXCBWindow* self, pWindow* parent);
bool pXCBWindowIsRunning(const pXCBWindow* self);
void pXCBWindowPollEvents(const pXCBWindow* self);
xcb_connection_t* pXCBWindowGetConnection(const pXCBWindow* self);
xcb_window_t pXCBWindowGetWindow(const pXCBWindow* self);
void pXCBWindowDestroy(pXCBWindow* self);