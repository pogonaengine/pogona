/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023, Nikita Romanyuk
 */

#include "core.h"
#include "defines.h"
#include "memory.h"
#include "pipeline.h"
#include "render_pass.h"
#include "shader.h"
#include "surface.h"
#include "swapchain.h"
#include "synchronization.h"
#include <engine/engine.h>
#include <engine/logger.h>
#include <pch/pch.h>

rVkCore gVkCore = { 0 };

static VkShaderModule sVertexShaderModule      = { 0 };
static VkShaderModule sFragmentShaderModule    = { 0 };

static VkSemaphore    sImageAvailableSemaphore = NULL;
static VkSemaphore    sRenderFinishedSemaphore = NULL;
static VkFence        sInFlightFence           = NULL;

static rVkBuffer      sVertexBuffer            = { 0 };
static rVkBuffer      sIndexBuffer             = { 0 };

static u32            sImageIndex              = 0;

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

i32 rVkInstanceCreate(i8 windowType)
{
	uint32_t supportedApiVersion = 0;
	rVK_CHECK(vkEnumerateInstanceVersion(&supportedApiVersion));
	if (supportedApiVersion < VK_API_VERSION_1_1) {
		pLoggerError("Vulkan instance does not support 1.1\n");
		return -1;
	}

	const VkApplicationInfo applicationInfo = {
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pEngineName = "pogona",
		.engineVersion = VK_MAKE_VERSION(pVERSION_MAJOR, pVERSION_MINOR, pVERSION_PATCH),
		.apiVersion = VK_API_VERSION_1_1,
	};

	pVector(const char*) layers;
	pVector(const char*) extensions;
	pVectorCreate(&layers);
	pVectorCreate(&extensions);

	pVectorPush(&extensions, VK_KHR_SURFACE_EXTENSION_NAME);
	char* windowExtension;
	switch (windowType) {
#ifdef pWAYLAND
	case pWINDOW_TYPE_WAYLAND:
		windowExtension = VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME; break;
#endif
#ifdef pXCB
	case pWINDOW_TYPE_XCB:
		windowExtension = VK_KHR_XCB_SURFACE_EXTENSION_NAME; break;
#endif
#ifdef pXLIB
	case pWINDOW_TYPE_XLIB:
		windowExtension = VK_KHR_XLIB_SURFACE_EXTENSION_NAME; break;
#endif
	default:
		assert(!"unreachable");
	}
	pVectorPush(&extensions, windowExtension);
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

	rVK_CHECK(vkCreateInstance(&instanceCreateInfo, NULL, &gVkCore.instance));
	volkLoadInstance(gVkCore.instance);

#ifndef NDEBUG
	rVK_CHECK(vkCreateDebugUtilsMessengerEXT(gVkCore.instance, &debugUtilsMessengerCreateInfo, NULL, &gVkCore.debugUtilsMessenger));
#endif

	pVectorDestroy(&layers);
	pVectorDestroy(&extensions);
	return supportedApiVersion;
}

void rVkInstanceDestroy(void)
{
#ifndef NDEBUG
	vkDestroyDebugUtilsMessengerEXT(gVkCore.instance, gVkCore.debugUtilsMessenger, NULL);
#endif
	vkDestroyInstance(gVkCore.instance, NULL);
}

i32 rVkPickPhysicalDevice(void)
{
	u32 groupCount = 0;
	rVK_CHECK(vkEnumeratePhysicalDeviceGroups(gVkCore.instance, &groupCount, NULL));
	VkPhysicalDeviceGroupProperties* groupProperties = calloc(groupCount, sizeof(*groupProperties));
	rVK_CHECK(vkEnumeratePhysicalDeviceGroups(gVkCore.instance, &groupCount, groupProperties));

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

			if (deviceProperties.apiVersion < VK_API_VERSION_1_1) {
				pLoggerWarning("%s does not support Vulkan 1.1\n", deviceProperties.deviceName);
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

	gVkCore.physicalDevice.physicalDevice = pickedPhysicalDevice;
	gVkCore.physicalDevice.queueFamilyIndex = pickedQueueFamily;
	vkGetPhysicalDeviceMemoryProperties(pickedPhysicalDevice, &gVkCore.physicalDevice.memoryProperties);
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
		.queueFamilyIndex = gVkCore.physicalDevice.queueFamilyIndex,
	};

	VkDeviceCreateInfo deviceCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		.queueCreateInfoCount = 1,
		.pQueueCreateInfos = &queueCreateInfo,
		.enabledExtensionCount = pARRAY_SIZE(extensions),
		.ppEnabledExtensionNames = extensions,
		.pEnabledFeatures = NULL,
	};
	rVK_CHECK(vkCreateDevice(gVkCore.physicalDevice.physicalDevice, &deviceCreateInfo, NULL, &gVkCore.device));
	volkLoadDevice(gVkCore.device);
	vkGetDeviceQueue(gVkCore.device, gVkCore.physicalDevice.queueFamilyIndex, 0, &gVkCore.queue);
	return 0;
}

i32 rVkCreateCommandPool(void)
{
	VkCommandPoolCreateInfo commandPoolCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.queueFamilyIndex = gVkCore.physicalDevice.queueFamilyIndex,
		.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT
		       | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
	};
	rVK_CHECK(vkCreateCommandPool(gVkCore.device, &commandPoolCreateInfo, NULL, &gVkCore.commandPool));

	VkCommandBufferAllocateInfo commandBufferAllocateInfo = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.commandBufferCount = 1,
		.commandPool = gVkCore.commandPool,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
	};
	rVK_CHECK(vkAllocateCommandBuffers(gVkCore.device, &commandBufferAllocateInfo, gVkCore.commandBuffers));
	return 0;
}

i32 rVkCreate(pWindow* window)
{
	i32 error = 0;

	rVK_CHECK(volkInitialize());

	error = rVkInstanceCreate(window->type);
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

	error = rVkCreateBuffer(&sVertexBuffer,
	                        2 * 1024 * 1024, /* FIXME: this */
	                        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
	                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	if (error < 0) {
		pLoggerError("Couldn't create vertex buffer\n");
		goto exit;
	}

	error = rVkCreateBuffer(&sIndexBuffer,
	                        2 * 1024 * 1024, /* FIXME: this */
	                        VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
	                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	if (error < 0) {
		pLoggerError("Couldn't create index buffer\n");
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

	error = rVkReadShader(&sVertexShaderModule, "2d.vert.spv");
	if (error < 0) {
		pLoggerError("Couldn't create vertex shader module\n");
		goto exit;
	}

	error = rVkReadShader(&sFragmentShaderModule, "2d.frag.spv");
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
			.offset   = offsetof(rVertex, pos),
		},
		[1] = {
			.binding  = 0,
			.location = 1,
			.format   = VK_FORMAT_R32G32B32A32_SFLOAT,
			.offset   = offsetof(rVertex, colour),
		},
	};
	error = rVkCreateGraphicsPipeline((rVkGraphicsPipelineCreateInfo) {
			.stages = shaderStages,
			.stagesCount = pARRAY_SIZE(shaderStages),

			.vertexAttributes = vertexAttributes,
			.vertexAttributesCount = pARRAY_SIZE(vertexAttributes),
			.vertexStride = sizeof(rVertex),
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
	rVK_CHECK(vkWaitForFences(gVkCore.device, 1, &sInFlightFence, VK_TRUE, UINT64_MAX));
	rVK_CHECK(vkResetFences(gVkCore.device, 1, &sInFlightFence));

	rVK_CHECK(rVkAcquireNextImage(&sImageIndex, sImageAvailableSemaphore));

	vkResetCommandBuffer(gVkCore.commandBuffers[0], 0);
	VkCommandBufferBeginInfo commandBufferBeginInfo = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
	};
	rVK_CHECK(vkBeginCommandBuffer(gVkCore.commandBuffers[0], &commandBufferBeginInfo));

	VkClearValue clearValue = { { { 0.f, 0.f, 0.f, 1.f } } };
	VkRenderPassBeginInfo renderPassBeginInfo = {
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		.renderPass = gVkCore.renderPass,
		.framebuffer = gSwapchain.framebuffers[sImageIndex],
		.renderArea = {
			.offset = { 0, 0 },
			.extent = { gSwapchain.width, gSwapchain.height },
		},
		.clearValueCount = 1,
		.pClearValues = &clearValue,
	};
	vkCmdBeginRenderPass(gVkCore.commandBuffers[0], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline(gVkCore.commandBuffers[0], VK_PIPELINE_BIND_POINT_GRAPHICS, gVkCore.pipeline.pipeline);
	VkViewport viewport = {
		.x        = 0.f,
		.y        = 0.f,
		.width    = (f32) gSwapchain.width,
		.height   = (f32) gSwapchain.height,
		.minDepth = 0.f,
		.maxDepth = 1.f,
	};
	vkCmdSetViewport(gVkCore.commandBuffers[0], 0, 1, &viewport);

	VkRect2D scissor = {
		.offset = { 0, 0 },
		.extent = {
			.width  = gSwapchain.width,
			.height = gSwapchain.height,
		},
	};
	vkCmdSetScissor(gVkCore.commandBuffers[0], 0, 1, &scissor);
	return 0;
}

i32 rVkDrawMesh(const rMesh* mesh)
{
	/* FIXME: allocate buffers for each mesh!!! */
	memcpy(sVertexBuffer.data, mesh->vertices, sizeof(rVertex) * mesh->verticesCount);
	memcpy(sIndexBuffer.data, mesh->indices, sizeof(u16) * mesh->indicesCount);

	VkDeviceSize dummyOffset = 0;
	vkCmdBindVertexBuffers(gVkCore.commandBuffers[0], 0, 1, &sVertexBuffer.buffer, &dummyOffset);
	vkCmdBindIndexBuffer(gVkCore.commandBuffers[0], sIndexBuffer.buffer, 0, VK_INDEX_TYPE_UINT16);
	vkCmdDrawIndexed(gVkCore.commandBuffers[0], mesh->indicesCount, 1, 0, 0, 0);
	return 0;
}

i32 rVkEndFrame(void)
{
	vkCmdEndRenderPass(gVkCore.commandBuffers[0]);
	rVK_CHECK(vkEndCommandBuffer(gVkCore.commandBuffers[0]));

	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	VkSubmitInfo submitInfo = {
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &sImageAvailableSemaphore,
		.pWaitDstStageMask = waitStages,
		.commandBufferCount = 1,
		.pCommandBuffers = gVkCore.commandBuffers,
		.signalSemaphoreCount = 1,
		.pSignalSemaphores = &sRenderFinishedSemaphore,
	};
	rVK_CHECK(vkQueueSubmit(gVkCore.queue, 1, &submitInfo, sInFlightFence));

	VkPresentInfoKHR presentInfo = {
		.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		.swapchainCount = 1,
		.pSwapchains = &gSwapchain.swapchain,
		.pImageIndices = &sImageIndex,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &sRenderFinishedSemaphore,
	};
	rVK_CHECK(vkQueuePresentKHR(gVkCore.queue, &presentInfo));

	rVK_CHECK(vkQueueWaitIdle(gVkCore.queue));
	return 0;
}

void rVkDestroy(void)
{
	vkDestroyFence(gVkCore.device, sInFlightFence, NULL);
	vkDestroySemaphore(gVkCore.device, sRenderFinishedSemaphore, NULL);
	vkDestroySemaphore(gVkCore.device, sImageAvailableSemaphore, NULL);
	vkDestroyPipeline(gVkCore.device, gVkCore.pipeline.pipeline, NULL);
	vkDestroyShaderModule(gVkCore.device, sFragmentShaderModule, NULL);
	vkDestroyShaderModule(gVkCore.device, sVertexShaderModule, NULL);
	vkDestroyPipelineCache(gVkCore.device, gVkCore.pipeline.cache, NULL);
	vkDestroyPipelineLayout(gVkCore.device, gVkCore.pipeline.layout, NULL);
	vkDestroyRenderPass(gVkCore.device, gVkCore.renderPass, NULL);
	rVkDestroySwapchain();
	rVkDestroyBuffer(&sIndexBuffer);
	rVkDestroyBuffer(&sVertexBuffer);
	rVkDestroySurface();
	vkDestroyCommandPool(gVkCore.device, gVkCore.commandPool, NULL);
	vkDestroyDevice(gVkCore.device, NULL);
	rVkInstanceDestroy();
}
