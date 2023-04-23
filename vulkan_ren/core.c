/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023, Nikita Romanyuk
 */

#include "core.h"
#include "defines.h"
#include "logger.h"
#include "pipeline.h"
#include "render.h"
#include "render_pass.h"
#include "shader.h"
#include "surface.h"
#include "swapchain.h"
#include "synchronization.h"
#include <engine/engine.h>
#include <pch/pch.h>

rVkCore gCore = { 0 };

static VkShaderModule sVertexShaderModule   = { 0 };
static VkShaderModule sFragmentShaderModule = { 0 };

static VkSemaphore    sImageAvailableSemaphore = NULL;
static VkSemaphore    sRenderFinishedSemaphore = NULL;
static VkFence        sInFlightFence           = NULL;

static VkBuffer       sVertexBuffer            = NULL;
static VkDeviceMemory sVertexBufferMemory = NULL;

static u32            sImageIndex              = 0;

static const rVkVertex sVertices[3] = {
	{ {{ 0.0f, -0.5f, 0.0f}}, {{1.0f, 0.0f, 0.0f, 1.0f}} },
	{ {{ 0.5f,  0.5f, 0.0f}}, {{0.0f, 1.0f, 0.0f, 1.0f}} },
	{ {{-0.5f,  0.5f, 0.0f}}, {{0.0f, 0.0f, 1.0f, 1.0f}} },
};

static i32 sCreateVertexBuffer(void)
{
	VkBufferCreateInfo bufferCreateInfo = {
	  .sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
	  .size        = sizeof(sVertices),
	  .usage       = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
	  .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
	};
	rCHECK(vkCreateBuffer(gCore.device, &bufferCreateInfo, NULL, &sVertexBuffer));

	VkMemoryRequirements memoryRequirements;
	vkGetBufferMemoryRequirements(gCore.device, sVertexBuffer, &memoryRequirements);

	VkMemoryAllocateInfo memoryAllocateInfo = {
	  .sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
	  .allocationSize  = memoryRequirements.size,
	  .memoryTypeIndex = 0,
	};
	rCHECK(vkAllocateMemory(gCore.device, &memoryAllocateInfo, NULL, &sVertexBufferMemory));
	vkBindBufferMemory(gCore.device, sVertexBuffer, sVertexBufferMemory, 0);
	return 0;
}

bool pVulkanSupport(void)
{
	/* This only tries to load the symbols from libvulkan.so and doesn't try to create an instance, check the devices, or anything like that.
	 * It should be replaced with a better algorithm in the future. */
	VkResult error = volkInitialize();
	if (error != VK_SUCCESS) {
	  pLoggerError("Vulkan isn't supported: %u\n", error);
	  return false;
	}
	return error == VK_SUCCESS;
}

#ifndef NDEBUG
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
#endif

i32 rVkInstanceCreate(void)
{
	uint32_t supportedApiVersion = 0;
	rCHECK(vkEnumerateInstanceVersion(&supportedApiVersion));
	if (supportedApiVersion < VK_API_VERSION_1_3) {
		pLoggerError("Vulkan instance does not support 1.3\n");
		return -1;
	}

	const VkApplicationInfo applicationInfo = {
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pEngineName = "pogona",
		.engineVersion = VK_MAKE_VERSION(pVERSION_MAJOR, pVERSION_MINOR, pVERSION_PATCH),
		.apiVersion = VK_API_VERSION_1_3,
	};

	pVector(const char*) layers;
	pVector(const char*) extensions;
	pVectorCreate(&layers);
	pVectorCreate(&extensions);

	pVectorPush(&extensions, VK_KHR_SURFACE_EXTENSION_NAME);
#ifdef pWAYLAND
	pVectorPush(&extensions, VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME);
#endif
#ifdef pX11
	pVectorPush(&extensions, VK_KHR_XLIB_SURFACE_EXTENSION_NAME);
#endif
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

	rCHECK(vkCreateInstance(&instanceCreateInfo, NULL, &gCore.instance));
	volkLoadInstance(gCore.instance);

#ifndef NDEBUG
	rCHECK(vkCreateDebugUtilsMessengerEXT(gCore.instance, &debugUtilsMessengerCreateInfo, NULL, &gCore.debugUtilsMessenger));
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
	rCHECK(vkEnumeratePhysicalDeviceGroups(gCore.instance, &groupCount, NULL));
	VkPhysicalDeviceGroupProperties* groupProperties = calloc(groupCount, sizeof(*groupProperties));
	rCHECK(vkEnumeratePhysicalDeviceGroups(gCore.instance, &groupCount, groupProperties));

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

i32 rVkCreateDevice(void)
{
	const char* extensions[] = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
	};

	const float queuePriorities[] = { 1.f };
	VkDeviceQueueCreateInfo queueCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
		.pQueuePriorities = queuePriorities,
		.queueCount = 1,
		.queueFamilyIndex = gCore.physicalDevice.queueFamilyIndex,
	};

	VkDeviceCreateInfo deviceCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		.queueCreateInfoCount = 1,
		.pQueueCreateInfos = &queueCreateInfo,
		.enabledExtensionCount = pARRAY_SIZE(extensions),
		.ppEnabledExtensionNames = extensions,
		.pEnabledFeatures = NULL,
	};
	rCHECK(vkCreateDevice(gCore.physicalDevice.physicalDevice, &deviceCreateInfo, NULL, &gCore.device));
	volkLoadDevice(gCore.device);
	vkGetDeviceQueue(gCore.device, gCore.physicalDevice.queueFamilyIndex, 0, &gCore.queue);
	return 0;
}

i32 rVkCreateCommandPool(void)
{
	VkCommandPoolCreateInfo commandPoolCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.queueFamilyIndex = gCore.physicalDevice.queueFamilyIndex,
		.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT
		       | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
	};
	rCHECK(vkCreateCommandPool(gCore.device, &commandPoolCreateInfo, NULL, &gCore.commandPool));

	VkCommandBufferAllocateInfo commandBufferAllocateInfo = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.commandBufferCount = 1,
		.commandPool = gCore.commandPool,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
	};
	rCHECK(vkAllocateCommandBuffers(gCore.device, &commandBufferAllocateInfo, gCore.commandBuffers));
	return 0;
}

i32 rVkCreate(pWindow* window)
{
	i32 error = 0;

	/* volkInitialize() is already called in pVulkanSupport,
	 * so there is no need to call it again. */

	error = rVkInstanceCreate();
	if (error < 0) {
		pLoggerError("Couldn't create Vulkan instance: %d\n", error);
		goto exit;
	}
	pLoggerInfo("Supported Vulkan version: %u.%u.%u\n",
			VK_API_VERSION_MAJOR(error),
			VK_API_VERSION_MINOR(error),
			VK_API_VERSION_PATCH(error));

	error = rVkPickPhysicalDevice();
	if (error < 0) {
		pLoggerError("Couldn't pick physical device or queue family\n");
		goto exit;
	}

	error = rVkCreateDevice();
	if (error < 0) {
		pLoggerError("Couldn't create device\n");
		goto exit;
	}

	error = sCreateVertexBuffer();
	if (error < 0) {
	  pLoggerError("Could not create vertex buffer\n");
	  goto exit;
	}

	error = rVkCreateCommandPool();
	if (error < 0) {
		pLoggerError("Couldn't create command pool\n");
		goto exit;
	}

	error = rVkCreateSurface(window);
	if (error < 0) {
		pLoggerError("Couldn't create surface\n");
		goto exit;
	}

	error = rVkCreateRenderPass();
	if (error < 0) {
		pLoggerError("Couldn't create render pass\n");
		goto exit;
	}

	error = rVkCreateSwapchain(window);
	if (error < 0) {
		pLoggerError("Couldn't create swapchain\n");
		goto exit;
	}

	error = rVkCreatePipelineLayoutAndCache();
	if (error < 0) {
		pLoggerError("Couldn't create pipeline layout and cache\n");
		goto exit;
	}

	error = rVkReadShader(&sVertexShaderModule, "shaders/2d.vert.spv");
	if (error < 0) {
		pLoggerError("Couldn't create vertex shader module\n");
		goto exit;
	}

	error = rVkReadShader(&sFragmentShaderModule, "shaders/2d.frag.spv");
	if (error < 0) {
		pLoggerError("Couldn't create fragment shader module\n");
		goto exit;
	}

	rVkShaderStage shaderStages[2] = {
		[0] = {
			.stage = VK_SHADER_STAGE_VERTEX_BIT,
			.module = sVertexShaderModule,
		},
		[1] = {
			.stage = VK_SHADER_STAGE_FRAGMENT_BIT,
			.module = sFragmentShaderModule,
		},
	};
	VkVertexInputAttributeDescription vertexAttributes[2] = {
		[0] = {
			.binding  = 0,
			.location = 0,
			.format   = VK_FORMAT_R32G32B32_SFLOAT,
			.offset   = offsetof(rVkVertex, pos),
		},
		[1] = {
			.binding  = 0,
			.location = 1,
			.format   = VK_FORMAT_R32G32B32A32_SFLOAT,
			.offset   = offsetof(rVkVertex, colour),
		},
	};
	error = rVkCreateGraphicsPipeline((rVkGraphicsPipelineCreateInfo) {
			.stages = shaderStages,
			.stagesCount = pARRAY_SIZE(shaderStages),

			.vertexAttributes = vertexAttributes,
			.vertexAttributesCount = pARRAY_SIZE(vertexAttributes),
			.vertexStride = sizeof(rVkVertex),
	});
	if (error < 0) {
		pLoggerError("Couldn't create graphics pipeline\n");
		goto exit;
	}

	assert(!rVkCreateSemaphore(&sImageAvailableSemaphore));
	assert(!rVkCreateSemaphore(&sRenderFinishedSemaphore));
	assert(!rVkCreateFence(&sInFlightFence, true));

exit:
	return error;
}

i32 rVkBeginFrame(void)
{
	rCHECK(vkWaitForFences(gCore.device, 1, &sInFlightFence, VK_TRUE, UINT64_MAX));
	rCHECK(vkResetFences(gCore.device, 1, &sInFlightFence));

	rCHECK(rVkAcquireNextImage(&sImageIndex, sImageAvailableSemaphore));

	void* vertexData;
	vkMapMemory(gCore.device, sVertexBufferMemory, 0, sizeof(sVertices), 0, &vertexData);
	memcpy(vertexData, sVertices, sizeof(sVertices));
	vkUnmapMemory(gCore.device, sVertexBufferMemory);

	vkResetCommandBuffer(gCore.commandBuffers[0], 0);
	VkCommandBufferBeginInfo commandBufferBeginInfo = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
	};
	rCHECK(vkBeginCommandBuffer(gCore.commandBuffers[0], &commandBufferBeginInfo));

	VkClearValue clearValue = { { { 0.f, 0.f, 0.f, 1.f } } };
	VkRenderPassBeginInfo renderPassBeginInfo = {
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		.renderPass = gCore.renderPass,
		.framebuffer = gSwapchain.framebuffers[sImageIndex],
		.renderArea = {
			.offset = { 0, 0 },
			.extent = { gSwapchain.width, gSwapchain.height },
		},
		.clearValueCount = 1,
		.pClearValues = &clearValue,
	};
	vkCmdBeginRenderPass(gCore.commandBuffers[0], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline(gCore.commandBuffers[0], VK_PIPELINE_BIND_POINT_GRAPHICS, gCore.pipeline.pipeline);
	VkViewport viewport = {
		.x        = 0.f,
		.y        = 0.f,
		.width    = (f32) gSwapchain.width,
		.height   = (f32) gSwapchain.height,
		.minDepth = 0.f,
		.maxDepth = 1.f,
	};
	vkCmdSetViewport(gCore.commandBuffers[0], 0, 1, &viewport);

	VkRect2D scissor = {
		.offset = { 0, 0 },
		.extent = {
			.width  = gSwapchain.width,
			.height = gSwapchain.height,
		},
	};
	vkCmdSetScissor(gCore.commandBuffers[0], 0, 1, &scissor);
	return 0;
}

i32 rVkEndFrame(void)
{
	VkBuffer vertexBuffers[] = { sVertexBuffer };
	VkDeviceSize vertexOffsets[] = { 0 };
	vkCmdBindVertexBuffers(gCore.commandBuffers[0], 0, 1, vertexBuffers, vertexOffsets);
	vkCmdDraw(gCore.commandBuffers[0], 3, 1, 0, 0);

	vkCmdEndRenderPass(gCore.commandBuffers[0]);
	rCHECK(vkEndCommandBuffer(gCore.commandBuffers[0]));

	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	VkSubmitInfo submitInfo = {
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &sImageAvailableSemaphore,
		.pWaitDstStageMask = waitStages,
		.commandBufferCount = 1,
		.pCommandBuffers = gCore.commandBuffers,
		.signalSemaphoreCount = 1,
		.pSignalSemaphores = &sRenderFinishedSemaphore,
	};
	rCHECK(vkQueueSubmit(gCore.queue, 1, &submitInfo, sInFlightFence));

	VkPresentInfoKHR presentInfo = {
		.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		.swapchainCount = 1,
		.pSwapchains = &gSwapchain.swapchain,
		.pImageIndices = &sImageIndex,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &sRenderFinishedSemaphore,
	};
	rCHECK(vkQueuePresentKHR(gCore.queue, &presentInfo));

	rCHECK(vkQueueWaitIdle(gCore.queue));
	return 0;
}

void rVkDestroy(void)
{
	vkDestroyFence(gCore.device, sInFlightFence, NULL);
	vkDestroySemaphore(gCore.device, sRenderFinishedSemaphore, NULL);
	vkDestroySemaphore(gCore.device, sImageAvailableSemaphore, NULL);
	vkDestroyPipeline(gCore.device, gCore.pipeline.pipeline, NULL);
	vkDestroyShaderModule(gCore.device, sFragmentShaderModule, NULL);
	vkDestroyShaderModule(gCore.device, sVertexShaderModule, NULL);
	vkDestroyPipelineCache(gCore.device, gCore.pipeline.cache, NULL);
	vkDestroyPipelineLayout(gCore.device, gCore.pipeline.layout, NULL);
	vkDestroyRenderPass(gCore.device, gCore.renderPass, NULL);
	rVkDestroySwapchain();
	vkDestroyBuffer(gCore.device, sVertexBuffer, NULL);
	vkFreeMemory(gCore.device, sVertexBufferMemory, NULL);
	rVkDestroySurface();
	vkDestroyCommandPool(gCore.device, gCore.commandPool, NULL);
	vkDestroyDevice(gCore.device, NULL);
	rVkInstanceDestroy();
}
