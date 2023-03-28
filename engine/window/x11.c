/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023, Nikita Romanyuk
 */

#include "x11.h"
#include "config.h"
#include <engine/logger.h>
#include <pch/pch.h>

bool pX11Support(void)
{
	/* FIXME: implement */
	return true;
}

i32 pX11WindowCreate(pX11Window* self, pWindow* parent)
{
	i32 error = 0;
	self->parent = parent;

	self->display = XOpenDisplay(NULL);
	if (!self->display) {
		pLoggerError("Could not open X11 display\n");
		error = -1;
		goto exit;
	}

	self->screen = DefaultScreen(self->display);
	self->window = XCreateSimpleWindow(self->display, RootWindow(self->display, self->screen),
	                                   0, 0,
	                                   self->parent->width, self->parent->height,
	                                   0, 0,
	                                   0);
	if (!self->window) {
		pLoggerError("Could not create X11 window\n");
		error = -2;
		goto exit;
	}

	XMapWindow(self->display, self->window);

	self->isRunning = true;
exit:
	return error;
}

bool pX11WindowIsRunning(const pX11Window* self)
{
	return self->isRunning;
}

void pX11WindowPollEvents(const pX11Window* self)
{
	XEvent event;
	XNextEvent(self->display, &event);
	/* TODO: implement event handling */
}

Display* pX11WindowGetDisplay(const pX11Window* self)
{
	return self->display;
}

Window pX11WindowGetWindow(const pX11Window* self)
{
	return self->window;
}

void pX11WindowDestroy(pX11Window* self)
{
	XDestroyWindow(self->display, self->window);
	XCloseDisplay(self->display);
}
