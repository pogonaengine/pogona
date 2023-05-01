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
	rCHECK(vkCreateWaylandSurfaceKHR(gCore.instance, &waylandSurfaceCreateInfo, NULL, &gCore.surface.surface));
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
	rCHECK(vkCreateXlibSurfaceKHR(gCore.instance, &xlibSurfaceCreateInfo, NULL, &gCore.surface.surface));
	return 0;
}

#endif

static VkFormat sPickImageFormat(void)
{
	u32 formatsCount = gCore.surface.surfaceFormatsCount;
	VkSurfaceFormatKHR* formats = gCore.surface.surfaceFormats;

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
	u32 formatsCount = gCore.surface.surfaceFormatsCount;
	VkSurfaceFormatKHR* formats = gCore.surface.surfaceFormats;

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
		struct wl_display* wlDisplay = pWaylandWindowGetDisplay(window->api);
		struct wl_surface* wlSurface = pWaylandWindowGetSurface(window->api);
		error = sCreateWaylandSurface(wlDisplay, wlSurface);
		if (error < 0) {
			pLoggerError("Couldn't create VkSurfaceKHR for Wayland\n");
			goto exit;
		}
	} break;
#endif
#ifdef pXLIB
	case pWINDOW_TYPE_XLIB: {
		Display* xlibDisplay = pXlibWindowGetDisplay(window->api);
		Window   xlibWindow  = pXlibWindowGetWindow(window->api);
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

	rCHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(gCore.physicalDevice.physicalDevice, gCore.surface.surface, &gCore.surface.surfaceFormatsCount, NULL));
	gCore.surface.surfaceFormats = calloc(gCore.surface.surfaceFormatsCount, sizeof(VkSurfaceFormatKHR));
	rCHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(gCore.physicalDevice.physicalDevice, gCore.surface.surface, &gCore.surface.surfaceFormatsCount, gCore.surface.surfaceFormats));

	rCHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(gCore.physicalDevice.physicalDevice, gCore.surface.surface, &gCore.surface.presentModesCount, NULL));
	gCore.surface.presentModes = calloc(gCore.surface.presentModesCount, sizeof(VkPresentModeKHR));
	rCHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(gCore.physicalDevice.physicalDevice, gCore.surface.surface, &gCore.surface.presentModesCount, gCore.surface.presentModes));

	gCore.surface.pickedFormat     = sPickImageFormat();
	gCore.surface.pickedColorSpace = sPickColorSpace();
exit:
	return error;
}

void rVkDestroySurface(void)
{
	vkDestroySurfaceKHR(gCore.instance, gCore.surface.surface, NULL);
	free(gCore.surface.surfaceFormats);
	free(gCore.surface.presentModes);
}
