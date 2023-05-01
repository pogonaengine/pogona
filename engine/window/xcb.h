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

	xcb_connection_t* connection;
	xcb_screen_t*     screen;
	xcb_drawable_t    window; /* NOTE: xcb_drawable_t and xcb_window_t is the same thing, named differently just for convenience */
} pXCBWindow;

bool pXCBSupport(void);

i32 pXCBWindowCreate(pXCBWindow* self, pWindow* parent);
void pXCBWindowDestroy(pXCBWindow* self);
