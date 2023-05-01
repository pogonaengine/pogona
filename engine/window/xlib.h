/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023, Nikita Romanyuk
 */

#pragma once

#include "../defines.h"
#include "window.h"
#include <X11/Xlib.h>

typedef struct {
	pWindow* parent;

	Display* display;
	Window   window;
	i32      screen;
} pXlibWindow;

bool pXlibSupport(void);

i32 pXlibWindowCreate(pXlibWindow* self, pWindow* parent);
void pXlibWindowPollEvents(const pXlibWindow* self);
void pXlibWindowDestroy(pXlibWindow* self);
