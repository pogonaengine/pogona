/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023, Nikita Romanyuk
 */

#pragma once

#include "../defines.h"
#include "window.h"
#include <wayland-client.h>
#include <xdg-shell-client-protocol.h>

typedef struct {
	pWindow* parent;

	struct wl_display* display;
	struct wl_registry* registry;
	struct wl_compositor* compositor;
	struct wl_surface* surface;
	struct xdg_wm_base* xdgWmBase;
	struct xdg_surface* xdgSurface;
	struct xdg_toplevel* xdgToplevel;
} pWaylandWindow;

bool pWaylandSupport(void);

i32 pWaylandWindowCreate(pWaylandWindow* self, pWindow* parent);
void pWaylandWindowDestroy(pWaylandWindow* self);
