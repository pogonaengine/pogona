/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023, Nikita Romanyuk
 */

#pragma once


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
} rVkCore;

extern rVkCore gCore;

/* returns supported instance version on success, otherwise a negative value */
i32 rVkInstanceCreate(void);
void rVkInstanceDestroy(void);

i32 rVkPickPhysicalDevice(void);
