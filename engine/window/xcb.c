/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023, Nikita Romanyuk
 */

#include "config.h"
#include "xcb.h"
#include <engine/event.h>
#include <engine/logger.h>
#include <pch/pch.h>

bool pXCBSupport(void)
{
	return getenv("DISPLAY") != NULL;
}

i32 pXCBWindowCreate(pXCBWindow* self, pWindow* parent)
{
	i32 error = 0;
	self->parent = parent;

	self->connection = xcb_connect(NULL, NULL);
	error = xcb_connection_has_error(self->connection);
	if (error < 0) {
		pLoggerError("Could not connect to XCB: %d\n", error);
		xcb_disconnect(self->connection);
		goto exit;
	}

	/* get the first screen */
	self->screen = xcb_setup_roots_iterator(xcb_get_setup(self->connection)).data;

	self->window = xcb_generate_id(self->connection);
	u32 values[1] = { XCB_EVENT_MASK_STRUCTURE_NOTIFY };
	xcb_create_window(self->connection,
	                  XCB_COPY_FROM_PARENT,
	                  self->window,
	                  self->screen->root,
	                  0, 0,
	                  self->parent->width, self->parent->height,
	                  0,
	                  XCB_WINDOW_CLASS_INPUT_OUTPUT,
	                  self->screen->root_visual,
	                  XCB_CW_EVENT_MASK, values
	);

	xcb_change_property(self->connection,
	                    XCB_PROP_MODE_REPLACE,
	                    self->window,
	                    XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8,
	                    strlen(self->parent->title), self->parent->title
	);

	xcb_map_window(self->connection, self->window);
	xcb_flush(self->connection);

	self->isRunning = true;
exit:
	return error;
}

bool pXCBWindowIsRunning(const pXCBWindow* self)
{
	return self->isRunning;
}

void pXCBWindowPollEvents(const pXCBWindow* self)
{
	xcb_generic_event_t* event = xcb_wait_for_event(self->connection);
	switch (event->response_type & ~0x80) {
	case XCB_CONFIGURE_NOTIFY: {
		const xcb_configure_notify_event_t* configureNotify = (const xcb_configure_notify_event_t*) event;
		if (configureNotify->width != self->parent->width || configureNotify->height != self->parent->height) {
			/* this is resize: confirmed */
			self->parent->width = configureNotify->width;
			self->parent->height = configureNotify->height;
			pEventSend(pEVENT_RESIZED, self->parent);
		}
	} break;
	default: break;
	}
}

xcb_connection_t* pXCBWindowGetConnection(const pXCBWindow* self)
{
	return self->connection;
}

xcb_window_t pXCBWindowGetWindow(const pXCBWindow* self)
{
	return (xcb_window_t) self->window;
}

void pXCBWindowDestroy(pXCBWindow* self)
{
	xcb_destroy_window(self->connection, self->window);
	xcb_disconnect(self->connection);
}
