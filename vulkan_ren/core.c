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
	if (supportedApiVersion < VK_API_VERSION_1_3) {
		pLoggerError("Vulkan instance does not support 1.3\n");
		return -1;
	}

	const VkApplicationInfo applicationInfo = {
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pEngineName = "pogona",
		.engineVersion = VK_MAKE_VERSION(PVERSION_MAJOR, PVERSION_MINOR, PVERSION_PATCH),
		.apiVersion = VK_API_VERSION_1_3,
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

i32 rVkPickPhysicalDevice(void)
{
	u32 groupCount = 0;
	RVK_CHECK(vkEnumeratePhysicalDeviceGroups(gCore.instance, &groupCount, NULL));
	VkPhysicalDeviceGroupProperties* groupProperties = calloc(groupCount, sizeof(*groupProperties));
	RVK_CHECK(vkEnumeratePhysicalDeviceGroups(gCore.instance, &groupCount, groupProperties));

	VkPhysicalDevice pickedPhysicalDevice = NULL;
	u32 pickedQueueFamily = VK_QUEUE_FAMILY_IGNORED;
	for (u32 i = 0; i < groupCount; ++i) {
		VkPhysicalDeviceGroupProperties properties = groupProperties[i];
		for (u32 j = 0; j < properties.physicalDeviceCount; ++j) {
			VkPhysicalDevice device = properties.physicalDevices[j];
			VkPhysicalDeviceProperties deviceProperties;
			vkGetPhysicalDeviceProperties(device, &deviceProperties);

#ifndef NDEBUG
			pLoggerDebug("%s:\n", deviceProperties.deviceName);
			pLoggerDebug(" API version: %u.%u.%u\n",
					VK_API_VERSION_MAJOR(deviceProperties.apiVersion),
					VK_API_VERSION_MINOR(deviceProperties.apiVersion),
					VK_API_VERSION_PATCH(deviceProperties.apiVersion));
			pLoggerDebug(" Driver version: %u.%u.%u\n",
					VK_API_VERSION_MAJOR(deviceProperties.driverVersion),
					VK_API_VERSION_MINOR(deviceProperties.driverVersion),
					VK_API_VERSION_PATCH(deviceProperties.driverVersion));
			pLoggerDebug(" Vendor ID: %#x\n", deviceProperties.vendorID);
			pLoggerDebug(" Device ID: %#x\n", deviceProperties.deviceID);

			const char* deviceType = NULL;
			/* TODO: move this switch out */
			switch (deviceProperties.deviceType) {
			case VK_PHYSICAL_DEVICE_TYPE_OTHER:
				deviceType = "Other"; break;
			case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
				deviceType = "iGPU"; break;
			case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
				deviceType = "dGPU"; break;
			case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
				deviceType = "vGPU"; break;
			case VK_PHYSICAL_DEVICE_TYPE_CPU:
				deviceType = "CPU"; break;
			default:
				assert(false && "Unreachable");
			}
			pLoggerDebug(" Device type: %s\n", deviceType);
#endif

			if (deviceProperties.apiVersion < VK_API_VERSION_1_3) {
				pLoggerWarning("%s does not support Vulkan 1.3\n", deviceProperties.deviceName);
				continue;
			}

			u32 queueFamilyCount = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, NULL);
			VkQueueFamilyProperties* queueFamilyProperties = calloc(queueFamilyCount, sizeof(*queueFamilyProperties));
			vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilyProperties);

			for (u32 k = 0; k < queueFamilyCount; ++k) {
				VkQueueFamilyProperties properties = queueFamilyProperties[k];
#ifndef NDEBUG
				pLoggerDebug(" Queue %u:\n", k);
				pLoggerDebug("  Queue flags:\n");
				pLoggerDebug("   Graphics bit: %s\n", properties.queueFlags & VK_QUEUE_GRAPHICS_BIT ? "true" : "false");
				pLoggerDebug("   Compute bit: %s\n", properties.queueFlags & VK_QUEUE_COMPUTE_BIT ? "true" : "false");
				pLoggerDebug("   Transfer bit: %s\n", properties.queueFlags & VK_QUEUE_TRANSFER_BIT ? "true" : "false");
				pLoggerDebug("   Sparse binding bit: %s\n", properties.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT ? "true" : "false");
				pLoggerDebug("   Protected bit: %s\n", properties.queueFlags & VK_QUEUE_PROTECTED_BIT ? "true" : "false");
				pLoggerDebug("   Video decode bit: %s\n", properties.queueFlags & VK_QUEUE_VIDEO_DECODE_BIT_KHR ? "true" : "false");
# ifdef VK_ENABLE_BETA_EXTENSIONS
				pLoggerDebug("   Video encode bit: %s\n", properties.queueFlags & VK_QUEUE_VIDEO_ENCODE_BIT_KHR ? "true" : "false");
# endif
				pLoggerDebug("   Optical flow bit: %s\n", properties.queueFlags & VK_QUEUE_OPTICAL_FLOW_BIT_NV ? "true" : "false");
				pLoggerDebug("  Queue count: %u\n", properties.queueCount);
#endif

				if (properties.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
					pickedQueueFamily = k;
					pickedPhysicalDevice = device;
					pLoggerDebug("Picked queue %u of %s\n", k, deviceProperties.deviceName);
					break;
				}
			}

			free(queueFamilyProperties);
		}
	}

	if (!pickedPhysicalDevice || pickedQueueFamily == VK_QUEUE_FAMILY_IGNORED) {
		free(groupProperties);
		return -1;
	}

	free(groupProperties);

	gCore.physicalDevice.physicalDevice = pickedPhysicalDevice;
	gCore.physicalDevice.queueFamilyIndex = pickedQueueFamily;
	return 0;
}
