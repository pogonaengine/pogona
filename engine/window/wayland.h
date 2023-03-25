/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023, Nikita Romanyuk
 */

#pragma once

#include "../defines.h"
#include "window.h"
#include <wayland-client.h>

typedef struct {
	pWindow* parent;
} pWaylandWindow;

bool pWaylandSupport(void);

i32 pWaylandWindowCreate(pWaylandWindow* self, pWindow* parent);
struct wl_display* pWaylandWindowGetDisplay(const pWaylandWindow* self);
struct wl_surface* pWaylandWindowGetSurface(const pWaylandWindow* self);
void pWaylandWindowDestroy(pWaylandWindow* self);
