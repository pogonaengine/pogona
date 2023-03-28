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
	bool isRunning;

	Display* display;
	Window   window;
	i32      screen;
} pX11Window;

bool pX11Support(void);

i32 pX11WindowCreate(pX11Window* self, pWindow* parent);
bool pX11WindowIsRunning(const pX11Window* self);
void pX11WindowPollEvents(const pX11Window* self);
Display* pX11WindowGetDisplay(const pX11Window* self);
Window pX11WindowGetWindow(const pX11Window* self);
void pX11WindowDestroy(pX11Window* self);
