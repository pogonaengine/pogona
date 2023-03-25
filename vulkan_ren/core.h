/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023, Nikita Romanyuk
 */

#pragma once

#include <engine/defines.h>
#include <engine/window/window.h>
#include "vulkan.h"

typedef struct {
	VkPhysicalDevice physicalDevice;
	u32 queueFamilyIndex;
} rVkPhysicalDevice;

typedef struct {
	VkInstance instance;
#ifndef NDEBUG
	VkDebugUtilsMessengerEXT debugUtilsMessenger;
#endif
	rVkPhysicalDevice physicalDevice;
	VkDevice device;
	VkQueue queue;
	VkCommandPool commandPool;
	VkCommandBuffer commandBuffers[1];
	struct {
		VkSurfaceKHR surface;

		u32 surfaceFormatsCount;
		VkSurfaceFormatKHR* surfaceFormats;
		u32 presentModesCount;
		VkPresentModeKHR* presentModes;
	} surface;
} rVkCore;

extern rVkCore gCore;

bool pVulkanSupport(void);

i32 rVkCreate(pWindow* window);
i32 rVkBeginFrame(void);
i32 rVkEndFrame(void);
void rVkDestroy(void);

/* returns supported instance version on success, otherwise a negative value */
i32 rVkInstanceCreate(void);
void rVkInstanceDestroy(void);

i32 rVkPickPhysicalDevice(void);
i32 rVkCreateDevice(void);
i32 rVkCreateCommandPool(void);
