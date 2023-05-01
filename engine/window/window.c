/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023, Nikita Romanyuk
 */

#include "../logger.h"
#include "window.h"
#include <config.h>
#include <pch/pch.h>

#ifdef pWAYLAND
# include "wayland.h"
#endif

#ifdef pXCB
# include "xcb.h"
#endif

#ifdef pXLIB
# include "xlib.h"
#endif

typedef bool (*PFN_support)(void);
typedef struct {
	PFN_support pfn;
	i8 type;
} SupportTable;

static const SupportTable sTypePriority[pWINDOW_MAX_TYPES] = {
#ifdef pWAYLAND
	{ pWaylandSupport, pWINDOW_TYPE_WAYLAND },
#endif
#ifdef pXCB
	{ pXCBSupport, pWINDOW_TYPE_XCB },
#endif
#ifdef pXLIB
	{ pXlibSupport, pWINDOW_TYPE_XLIB },
#endif
};

static i8 sPickType(void)
{
	for (u32 i = 0; i < pWINDOW_MAX_TYPES; ++i) {
		if (sTypePriority[i].pfn())
			return sTypePriority[i].type;
	}
	return pWINDOW_TYPE_INVALID;
}

i32 pWindowCreate(pWindow* self, const char* title, u32 width, u32 height)
{
	self->type = sPickType();
	if (self->type == pWINDOW_TYPE_INVALID) {
		pLoggerError("Host doesn't support any window api\n");
		return -1;
	}

	strncpy(self->title, title, pWINDOW_TITLE_SIZE);
	self->width  = width;
	self->height = height;

	switch (self->type) {
#ifdef pWAYLAND
	case pWINDOW_TYPE_WAYLAND:
		self->api = calloc(1, sizeof(pWaylandWindow));
		pWaylandWindowCreate(self->api, self);
		break;
#endif
#ifdef pXCB
	case pWINDOW_TYPE_XCB:
		self->api = calloc(1, sizeof(pXCBWindow));
		pXCBWindowCreate(self->api, self);
		break;
#endif
#ifdef pXLIB
	case pWINDOW_TYPE_XLIB:
		self->api = calloc(1, sizeof(pXlibWindow));
		pXlibWindowCreate(self->api, self);
		break;
#endif
	default:
		assert(false && "unreachable");
	}
	return 0;
}

bool pWindowIsRunning(const pWindow* self)
{
	switch (self->type) {
#ifdef pWAYLAND
	case pWINDOW_TYPE_WAYLAND:
		return pWaylandWindowIsRunning((pWaylandWindow*) self->api);
#endif
#ifdef pXCB
	case pWINDOW_TYPE_XCB:
		return pXCBWindowIsRunning((pXCBWindow*) self->api);
#endif
#ifdef pXLIB
	case pWINDOW_TYPE_XLIB:
		return pXlibWindowIsRunning((pXlibWindow*) self->api);
#endif
	default:
		assert(false && "unreachable");
	}
}

void pWindowPollEvents(const pWindow* self)
{
	switch (self->type) {
#ifdef pWAYLAND
	case pWINDOW_TYPE_WAYLAND:
		pWaylandWindowPollEvents((pWaylandWindow*) self->api);
		return;
#endif
#ifdef pXCB
	case pWINDOW_TYPE_XCB:
		pXCBWindowPollEvents((pXCBWindow*) self->api);
		return;
#endif
#ifdef pXLIB
	case pWINDOW_TYPE_XLIB:
		pXlibWindowPollEvents((pXlibWindow*) self->api);
		return;
#endif
	default:
		assert(false && "unreachable");
	}
}

const void* pWindowGetApi(const pWindow* self)
{
	return self->api;
}

void pWindowDestroy(pWindow* self)
{
	memset(self->title, 0, pWINDOW_TITLE_SIZE);
	self->width  = 0;
	self->height = 0;

	switch (self->type) {
#ifdef pWAYLAND
	case pWINDOW_TYPE_WAYLAND:
		pWaylandWindowDestroy((pWaylandWindow*) self->api);
		break;
#endif
#ifdef pXCB
	case pWINDOW_TYPE_XCB:
		pXCBWindowDestroy((pXCBWindow*) self->api);
		break;
#endif
#ifdef pXLIB
	case pWINDOW_TYPE_XLIB:
		pXlibWindowDestroy((pXlibWindow*) self->api);
		break;
#endif
	default:
		assert(false && "unreachable");
	}

	free(self->api);
}
