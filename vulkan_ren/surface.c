/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023, Nikita Romanyuk
 */

#include "core.h"
#include "defines.h"
#include "surface.h"
#include <engine/logger.h>
#include <pch/pch.h>

#ifdef pWAYLAND

# include <engine/window/wayland.h>

static i32 sCreateWaylandSurface(struct wl_display* wlDisplay, struct wl_surface* wlSurface)
{
	VkWaylandSurfaceCreateInfoKHR waylandSurfaceCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR,
		.display = wlDisplay,
		.surface = wlSurface,
	};
	rCHECK(vkCreateWaylandSurfaceKHR(gVkCore.instance, &waylandSurfaceCreateInfo, NULL, &gVkCore.surface.surface));
	return 0;
}

#endif

#ifdef pXCB

#include <engine/window/xcb.h>

static i32 sCreateXCBSurface(xcb_connection_t* xcbConnection, xcb_window_t xcbWindow)
{
	VkXcbSurfaceCreateInfoKHR xcbSurfaceCreateInfo = {
		.sType      = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR,
		.connection = xcbConnection,
		.window     = xcbWindow,
	};
	rCHECK(vkCreateXcbSurfaceKHR(gVkCore.instance, &xcbSurfaceCreateInfo, NULL, &gVkCore.surface.surface));
	return 0;
}

#endif

#ifdef pXLIB

#include <engine/window/xlib.h>

static i32 sCreateXlibSurface(Display* xlibDisplay, Window xlibWindow)
{
	VkXlibSurfaceCreateInfoKHR xlibSurfaceCreateInfo = {
		.sType  = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR,
		.dpy    = xlibDisplay,
		.window = xlibWindow,
	};
	rCHECK(vkCreateXlibSurfaceKHR(gVkCore.instance, &xlibSurfaceCreateInfo, NULL, &gVkCore.surface.surface));
	return 0;
}

#endif

static VkFormat sPickImageFormat(void)
{
	u32 formatsCount = gVkCore.surface.surfaceFormatsCount;
	VkSurfaceFormatKHR* formats = gVkCore.surface.surfaceFormats;

	if (formatsCount == 1 && formats[0].format == VK_FORMAT_UNDEFINED)
		return VK_FORMAT_R8G8B8A8_SRGB;

	for (u32 i = 0; i < formatsCount; ++i) {
		if (formats[i].format == VK_FORMAT_R8G8B8A8_SRGB || formats[i].format == VK_FORMAT_B8G8R8A8_SRGB)
			return formats[i].format;
	}

	return formats[0].format;
}

static VkColorSpaceKHR sPickColorSpace(void)
{
	u32 formatsCount = gVkCore.surface.surfaceFormatsCount;
	VkSurfaceFormatKHR* formats = gVkCore.surface.surfaceFormats;

	for (u32 i = 0; i < formatsCount; ++i) {
		if (formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			return formats[i].colorSpace;
	}

	return formats[0].colorSpace;
}

i32 rVkCreateSurface(pWindow* window)
{
	i32 error = 0;
	switch (window->type) {
#ifdef pWAYLAND
	case pWINDOW_TYPE_WAYLAND: {
		struct wl_display* wlDisplay = ((pWaylandWindow*) window->api)->display;
		struct wl_surface* wlSurface = ((pWaylandWindow*) window->api)->surface;
		error = sCreateWaylandSurface(wlDisplay, wlSurface);
		if (error < 0) {
			pLoggerError("Couldn't create VkSurfaceKHR for Wayland\n");
			goto exit;
		}
	} break;
#endif
#ifdef pXCB
	case pWINDOW_TYPE_XCB: {
		xcb_connection_t* xcbConnection = ((pXCBWindow*) window->api)->connection;
		xcb_window_t xcbWindow = ((pXCBWindow*) window->api)->window;
		error = sCreateXCBSurface(xcbConnection, xcbWindow);
		if (error < 0) {
			pLoggerError("Couldn't create VkSurfaceKHR for XCB\n");
			goto exit;
		}
	} break;
#endif
#ifdef pXLIB
	case pWINDOW_TYPE_XLIB: {
		Display* xlibDisplay = ((pXlibWindow*) window->api)->display;
		Window   xlibWindow  = ((pXlibWindow*) window->api)->window;
		error = sCreateXlibSurface(xlibDisplay, xlibWindow);
		if (error < 0) {
			pLoggerError("Couldn't create VkSurfaceKHR for Xlib\n");
			goto exit;
		}
	} break;
#endif
	default:
		assert(false && "unreachable");
	}

	rCHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(gVkCore.physicalDevice.physicalDevice, gVkCore.surface.surface, &gVkCore.surface.surfaceFormatsCount, NULL));
	gVkCore.surface.surfaceFormats = calloc(gVkCore.surface.surfaceFormatsCount, sizeof(VkSurfaceFormatKHR));
	rCHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(gVkCore.physicalDevice.physicalDevice, gVkCore.surface.surface, &gVkCore.surface.surfaceFormatsCount, gVkCore.surface.surfaceFormats));

	rCHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(gVkCore.physicalDevice.physicalDevice, gVkCore.surface.surface, &gVkCore.surface.presentModesCount, NULL));
	gVkCore.surface.presentModes = calloc(gVkCore.surface.presentModesCount, sizeof(VkPresentModeKHR));
	rCHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(gVkCore.physicalDevice.physicalDevice, gVkCore.surface.surface, &gVkCore.surface.presentModesCount, gVkCore.surface.presentModes));

	gVkCore.surface.pickedFormat     = sPickImageFormat();
	gVkCore.surface.pickedColorSpace = sPickColorSpace();
exit:
	return error;
}

void rVkDestroySurface(void)
{
	vkDestroySurfaceKHR(gVkCore.instance, gVkCore.surface.surface, NULL);
	free(gVkCore.surface.surfaceFormats);
	free(gVkCore.surface.presentModes);
}
