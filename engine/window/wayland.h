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
	bool isRunning;

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
bool pWaylandWindowIsRunning(const pWaylandWindow* self);
void pWaylandWindowPollEvents(const pWaylandWindow* self);
struct wl_display* pWaylandWindowGetDisplay(const pWaylandWindow* self);
struct wl_surface* pWaylandWindowGetSurface(const pWaylandWindow* self);
void pWaylandWindowDestroy(pWaylandWindow* self);
