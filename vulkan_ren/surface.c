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
	RVK_CHECK(vkCreateWaylandSurfaceKHR(gCore.instance, &waylandSurfaceCreateInfo, NULL, &gCore.surface.surface));

	RVK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(gCore.physicalDevice.physicalDevice, gCore.surface.surface, &gCore.surface.surfaceFormatsCount, NULL));
	gCore.surface.surfaceFormats = calloc(gCore.surface.surfaceFormatsCount, sizeof(VkSurfaceFormatKHR));
	RVK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(gCore.physicalDevice.physicalDevice, gCore.surface.surface, &gCore.surface.surfaceFormatsCount, gCore.surface.surfaceFormats));

	RVK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(gCore.physicalDevice.physicalDevice, gCore.surface.surface, &gCore.surface.presentModesCount, NULL));
	gCore.surface.presentModes = calloc(gCore.surface.presentModesCount, sizeof(VkPresentModeKHR));
	RVK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(gCore.physicalDevice.physicalDevice, gCore.surface.surface, &gCore.surface.presentModesCount, gCore.surface.presentModes));
	return 0;
}

#endif

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
	default:
		assert(false && "unreachable");
	}
exit:
	return error;
}

void rVkDestroySurface(void)
{
	vkDestroySurfaceKHR(gCore.instance, gCore.surface.surface, NULL);
	free(gCore.surface.surfaceFormats);
	free(gCore.surface.presentModes);
}
