/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023, Nikita Romanyuk
 */

#pragma once

#include "defines.h"
#include "vulkan.h"
#include <engine/window/window.h>

typedef struct {
	u32 width, height;

	VkSwapchainKHR swapchain;
	VkFormat imageFormat;
	VkColorSpaceKHR colorSpace;

	u32 imagesCount;
	VkImage* images;
	VkImageView* imageViews;
	VkFramebuffer* framebuffers;
} rSwapchain;

extern rSwapchain gSwapchain;

i32 rVkCreateSwapchain(pWindow* window);
i32 rVkAcquireNextImage(u32* imageIndex, VkSemaphore semaphore);
void rVkDestroySwapchain(void);

i32 rVkInitImageViews(void);
i32 rVkInitFramebuffers(void);
