/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023, Nikita Romanyuk
 */

#include "config.h"
#include "xlib.h"
#include <X11/Xutil.h>
#include <engine/event.h>
#include <engine/logger.h>
#include <pch/pch.h>

bool pXlibSupport(void)
{
	return getenv("DISPLAY") != NULL;
}

i32 pXlibWindowCreate(pXlibWindow* self, pWindow* parent)
{
	i32 error = 0;
	self->parent = parent;

	self->display = XOpenDisplay(NULL);
	if (!self->display) {
		pLoggerError("Could not open Xlib display\n");
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
		pLoggerError("Could not create Xlib window\n");
		error = -2;
		goto exit;
	}

	/* set the title of the window */
	/* FIXME: superseded by XSetWMProperties, but I'm too lazy to bother about X */
	XSetStandardProperties(self->display, self->window,
	                       self->parent->title,
			       NULL, None, NULL, 0, NULL);

	/* I've also tried to use ResizeRedirectMask (ResizeRequestEvent).
	 * It was sending resize events, but the resolution remained the same.
	 * This caused weird artifacts and lags, at least on a Wayland compositor.
	 *
	 * I thought it was more specific to our use case. TBH it was the first variant I've found on Google. */
	XSelectInput(self->display, self->window, StructureNotifyMask);
	XMapWindow(self->display, self->window);

	self->isRunning = true;
exit:
	return error;
}

bool pXlibWindowIsRunning(const pXlibWindow* self)
{
	return self->isRunning;
}

void pXlibWindowPollEvents(const pXlibWindow* self)
{
	XEvent event;
	XNextEvent(self->display, &event);
	switch (event.type) {
	case ConfigureNotify: {
		XConfigureEvent xconfigure = event.xconfigure;

		/* X server sends XConfigureEvent for multiple purposes.
		 * But we are only interested in resize. */
		if ((u32) xconfigure.width != self->parent->width || (u32) xconfigure.height != self->parent->height) {
			/* this is resize: confirmed */
			self->parent->width  = xconfigure.width;
			self->parent->height = xconfigure.height;
			pEventSend(pEVENT_RESIZED, self->parent);
		}
	} break;
	default: break;
	}
}

Display* pXlibWindowGetDisplay(const pXlibWindow* self)
{
	return self->display;
}

Window pXlibWindowGetWindow(const pXlibWindow* self)
{
	return self->window;
}

void pXlibWindowDestroy(pXlibWindow* self)
{
	XDestroyWindow(self->display, self->window);
	XCloseDisplay(self->display);
}
