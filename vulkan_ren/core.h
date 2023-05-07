/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023, Nikita Romanyuk
 */

#pragma once

#include "vulkan.h"
#include "vulkan/vulkan_core.h"
#include <engine/defines.h>
#include <engine/renderer/mesh.h>

typedef struct pWindow pWindow;

typedef struct {
	VkPhysicalDevice physicalDevice;
	u32 queueFamilyIndex;

	VkPhysicalDeviceMemoryProperties memoryProperties;
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

		u32                 surfaceFormatsCount;
		VkSurfaceFormatKHR* surfaceFormats;
		u32                 presentModesCount;
		VkPresentModeKHR*   presentModes;

		VkFormat        pickedFormat;
		VkColorSpaceKHR pickedColorSpace;
	} surface;
	VkRenderPass renderPass;
	struct {
		VkPipeline pipeline;
		VkPipelineLayout layout;
		VkPipelineCache cache;
	} pipeline;
} rVkCore;

extern rVkCore gVkCore;

i32 rVkCreate(pWindow* window);
i32 rVkBeginFrame(void);
i32 rVkDrawMesh(const rMesh* mesh);
i32 rVkEndFrame(void);
void rVkDestroy(void);
