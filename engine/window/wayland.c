/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023, Nikita Romanyuk
 */

#include "../logger.h"
#include "wayland.h"
#include <pch/pch.h>

static void sRegistryGlobal(void* data, struct wl_registry* registry, u32 name, const char* interface, u32 version)
{
	pWaylandWindow* self = (pWaylandWindow*) data;

	if (!strcmp(interface, wl_compositor_interface.name)) {
		self->compositor = wl_registry_bind(registry, name, &wl_compositor_interface, version);
		return;
	}
	if (!strcmp(interface, xdg_wm_base_interface.name)) {
		self->xdgWmBase = wl_registry_bind(registry, name, &xdg_wm_base_interface, version);
		return;
	}
}

static void sXdgWmBasePing(void* data, struct xdg_wm_base* xdgWmBase, u32 serial)
{
	(void) data;
	xdg_wm_base_pong(xdgWmBase, serial);
}

static void sXdgSurfaceConfigure(void* data, struct xdg_surface* xdgSurface, u32 serial)
{
	pWaylandWindow* self = (pWaylandWindow*) data;

	xdg_surface_ack_configure(xdgSurface, serial);
	wl_surface_commit(self->surface);
}

static void sXdgToplevelConfigure(void* data, struct xdg_toplevel* xdgToplevel, i32 width, i32 height, struct wl_array* states)
{
	(void) data;
	(void) xdgToplevel;
	(void) width;
	(void) height;
	(void) states;
}

static void sXdgToplevelClose(void* data, struct xdg_toplevel* xdgToplevel)
{
	(void) xdgToplevel;
	((pWaylandWindow*) data)->isRunning = false;
}

bool pWaylandSupport(void)
{
	/* FIXME: implement */
	return true;
}

i32 pWaylandWindowCreate(pWaylandWindow* self, pWindow* parent)
{
	i32 error = 0;
	self->parent = parent;

	self->display = wl_display_connect(NULL);
	if (!self->display) {
		pLoggerError("Couldn't connect to Wayland display\n");
		error = -1;
		goto exit;
	}

	self->registry = wl_display_get_registry(self->display);
	if (!self->registry) {
		pLoggerError("Couldn't get registry from display\n");
		error = -2;
		goto exit;
	}
	static const struct wl_registry_listener sRegistryListener = {
		.global = sRegistryGlobal,
	};
	wl_registry_add_listener(self->registry, &sRegistryListener, (void*) self);
	wl_display_dispatch(self->display);
	wl_display_roundtrip(self->display);

	/* at this point they should be present */
	if (!self->compositor) {
		pLoggerError("Couldn't get compositor from registry\n");
		error = -3;
		goto exit;
	}
	if (!self->xdgWmBase) {
		pLoggerError("Couldn't get xdg_wm_base from registry\n");
		error = -4;
		goto exit;
	}

	self->surface = wl_compositor_create_surface(self->compositor);
	if (!self->surface) {
		pLoggerError("Couldn't create surface\n");
		error = -5;
		goto exit;
	}

	static const struct xdg_wm_base_listener sXdgWmBaseListener = {
		.ping = sXdgWmBasePing,
	};
	xdg_wm_base_add_listener(self->xdgWmBase, &sXdgWmBaseListener, (void*) self);

	self->xdgSurface = xdg_wm_base_get_xdg_surface(self->xdgWmBase, self->surface);
	if (!self->xdgSurface) {
		pLoggerError("Couldn't get xdg_surface from xdg_wm_base\n");
		error = -6;
		goto exit;
	}
	static const struct xdg_surface_listener sXdgSurfaceListener = {
		.configure = sXdgSurfaceConfigure,
	};
	xdg_surface_add_listener(self->xdgSurface, &sXdgSurfaceListener, (void*) self);

	self->xdgToplevel = xdg_surface_get_toplevel(self->xdgSurface);
	if (!self->xdgToplevel) {
		pLoggerError("Couldn't get xdg_toplevel from xdg_surface\n");
		error = -7;
		goto exit;
	}
	static const struct xdg_toplevel_listener sXdgToplevelListener = {
		.configure = sXdgToplevelConfigure,
		.close = sXdgToplevelClose,
	};
	xdg_toplevel_add_listener(self->xdgToplevel, &sXdgToplevelListener, (void*) self);

	xdg_toplevel_set_title(self->xdgToplevel, self->parent->title);
	xdg_toplevel_set_app_id(self->xdgToplevel, "pogona");
	xdg_toplevel_set_min_size(self->xdgToplevel, self->parent->width, self->parent->height);
	xdg_toplevel_set_max_size(self->xdgToplevel, self->parent->width, self->parent->height);

	wl_surface_commit(self->surface);
	wl_display_roundtrip(self->display);

	self->isRunning = true;

exit:
	return error;
}

bool pWaylandWindowIsRunning(const pWaylandWindow* self)
{
	return self->isRunning;
}

void pWaylandWindowPollEvents(const pWaylandWindow* self)
{
	wl_display_dispatch(self->display);
}

struct wl_display* pWaylandWindowGetDisplay(const pWaylandWindow* self)
{
	return self->display;
}

struct wl_surface* pWaylandWindowGetSurface(const pWaylandWindow* self)
{
	return self->surface;
}

void pWaylandWindowDestroy(pWaylandWindow* self)
{
	self->parent = NULL;
	xdg_toplevel_destroy(self->xdgToplevel);
	xdg_surface_destroy(self->xdgSurface);
	xdg_wm_base_destroy(self->xdgWmBase);
	wl_surface_destroy(self->surface);
	wl_compositor_destroy(self->compositor);
	wl_registry_destroy(self->registry);
	wl_display_disconnect(self->display);
}