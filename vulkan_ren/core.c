/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023, Nikita Romanyuk
 */

#include "core.h"
#include "defines.h"
#include <engine/engine.h>
#include <pch/vulkan.h>

rVkCore gCore = { 0 };

i32 rVkInstanceCreate(void)
{
	uint32_t supportedApiVersion = 0;
	RVK_CHECK(vkEnumerateInstanceVersion(&supportedApiVersion));
	if (supportedApiVersion < VK_API_VERSION_1_2) {
		pLoggerError("Vulkan instance does not support 1.2\n");
		return -1;
	}

	const VkApplicationInfo applicationInfo = {
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pEngineName = "pogona",
		.engineVersion = VK_MAKE_VERSION(PVERSION_MAJOR, PVERSION_MINOR, PVERSION_PATCH),
		.apiVersion = VK_API_VERSION_1_2,
	};
	const VkInstanceCreateInfo instanceCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pApplicationInfo = &applicationInfo,
		.enabledLayerCount = 0,
		.ppEnabledLayerNames = NULL,
		.enabledExtensionCount = 0,
		.ppEnabledExtensionNames = NULL,
	};

	RVK_CHECK(vkCreateInstance(&instanceCreateInfo, NULL, &gCore.instance));
	volkLoadInstance(gCore.instance);
	return supportedApiVersion;
}

void rVkInstanceDestroy(void)
{
	vkDestroyInstance(gCore.instance, NULL);
}
