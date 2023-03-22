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

static VkBool32 VKAPI_PTR sDebugCallback(
		VkDebugUtilsMessageSeverityFlagsEXT severity,
		VkDebugUtilsMessageTypeFlagsEXT type,
		const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
		void* userData)
{
	(void) severity;
	(void) type;
	(void) userData;

	pLoggerDebug("%s\n", callbackData->pMessage);
	return VK_FALSE;
}

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

	pVector(const char*) layers;
	pVector(const char*) extensions;
	pVectorCreate(&layers);
	pVectorCreate(&extensions);

	pVectorPush(&extensions, VK_KHR_SURFACE_EXTENSION_NAME);
#ifndef NDEBUG
	pVectorPush(&extensions, VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	pVectorPush(&layers, "VK_LAYER_KHRONOS_validation");
#endif

	VkInstanceCreateInfo instanceCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pApplicationInfo = &applicationInfo,
		.enabledLayerCount = layers.size,
		.ppEnabledLayerNames = layers.data,
		.enabledExtensionCount = extensions.size,
		.ppEnabledExtensionNames = extensions.data,
	};

#ifndef NDEBUG
	VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
		.pfnUserCallback = sDebugCallback,
		.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
		.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT,
	};
	instanceCreateInfo.pNext = &debugUtilsMessengerCreateInfo;
#endif

	RVK_CHECK(vkCreateInstance(&instanceCreateInfo, NULL, &gCore.instance));
	volkLoadInstance(gCore.instance);

#ifndef NDEBUG
	RVK_CHECK(vkCreateDebugUtilsMessengerEXT(gCore.instance, &debugUtilsMessengerCreateInfo, NULL, &gCore.debugUtilsMessenger));
#endif

	pVectorDestroy(&layers);
	pVectorDestroy(&extensions);
	return supportedApiVersion;
}

void rVkInstanceDestroy(void)
{
#ifndef NDEBUG
	vkDestroyDebugUtilsMessengerEXT(gCore.instance, gCore.debugUtilsMessenger, NULL);
#endif
	vkDestroyInstance(gCore.instance, NULL);
}
