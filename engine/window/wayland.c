/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023, Nikita Romanyuk
 */

#include "wayland.h"
#include <pch/pch.h>

bool pWaylandSupport(void)
{
	/* FIXME: implement */
	return true;
}

i32 pWaylandWindowCreate(pWaylandWindow* self, pWindow* parent)
{
	self->parent = parent;
	return 0;
}

struct wl_display* pWaylandWindowGetDisplay(const pWaylandWindow* self)
{
	assert(false && "unimplemented");
	return NULL;
}

struct wl_surface* pWaylandWindowGetSurface(const pWaylandWindow* self)
{
	assert(false && "unimplemented");
	return NULL;
}

void pWaylandWindowDestroy(pWaylandWindow* self)
{
	self->parent = NULL;
}
