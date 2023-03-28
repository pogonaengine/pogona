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

#ifdef pX11
# include "x11.h"
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
#ifdef pX11
	{ pX11Support, pWINDOW_TYPE_X11 },
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
#ifdef pX11
	case pWINDOW_TYPE_X11:
		self->api = calloc(1, sizeof(pX11Window));
		pX11WindowCreate(self->api, self);
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
#ifdef pX11
	case pWINDOW_TYPE_X11:
		return pX11WindowIsRunning((pX11Window*) self->api);
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
#ifdef pX11
	case pWINDOW_TYPE_X11:
		pX11WindowPollEvents((pX11Window*) self->api);
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
#ifdef pX11
	case pWINDOW_TYPE_X11:
		pX11WindowDestroy((pX11Window*) self->api);
		break;
#endif
	default:
		assert(false && "unreachable");
	}

	free(self->api);
}
