/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023, Nikita Romanyuk
 */

#include "core.h"
#include "swapchain.h"
#include <engine/event.h>
#include <pch/pch.h>

rVkSwapchain gSwapchain = { 0 };

static bool sRegisteredEvent = false;
static bool sOnResize(pEventData data)
{
	rVkCreateSwapchain((pWindow*) data);
	return true;
}

static i32 sDestroySwapchain(VkSwapchainKHR swapchain)
{
	rVK_CHECK(vkQueueWaitIdle(gVkCore.queue));
	for (u32 i = 0; i < gSwapchain.imagesCount; ++i) {
		vkDestroyImageView(gVkCore.device, gSwapchain.imageViews[i], NULL);
		vkDestroyFramebuffer(gVkCore.device, gSwapchain.framebuffers[i], NULL);
	}
	vkDestroySwapchainKHR(gVkCore.device, swapchain, NULL);
	return 0;
}

i32 rVkCreateSwapchain(pWindow* window)
{
	if (!sRegisteredEvent)
		pEventRegister(pEVENT_RESIZED, sOnResize);

	gSwapchain.imageFormat = gVkCore.surface.pickedFormat;
	gSwapchain.colorSpace  = gVkCore.surface.pickedColorSpace;

	VkSurfaceCapabilitiesKHR surfaceCapabilities;
	rVK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gVkCore.physicalDevice.physicalDevice, gVkCore.surface.surface, &surfaceCapabilities));
	VkExtent2D swapchainExtent = surfaceCapabilities.currentExtent;
	if (swapchainExtent.width == 0xffffffff || swapchainExtent.height == 0xffffffff) {
		swapchainExtent.width  = pCLAMP(window->width, surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width);
		swapchainExtent.height = pCLAMP(window->height, surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height);
	}

	gSwapchain.width  = swapchainExtent.width;
	gSwapchain.height = swapchainExtent.height;

	VkSwapchainCreateInfoKHR swapchainCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		.surface = gVkCore.surface.surface,
		.imageFormat = gSwapchain.imageFormat,
		.imageColorSpace = gSwapchain.colorSpace,
		.presentMode = VK_PRESENT_MODE_FIFO_KHR,
		.oldSwapchain = gSwapchain.swapchain,
		.imageExtent = swapchainExtent,
		.minImageCount = surfaceCapabilities.minImageCount + 1,
		.imageArrayLayers = 1,
		.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
		            | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
		.queueFamilyIndexCount = 1,
		.pQueueFamilyIndices = &gVkCore.physicalDevice.queueFamilyIndex,
		.preTransform = surfaceCapabilities.currentTransform,
		.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
	};
	rVK_CHECK(vkCreateSwapchainKHR(gVkCore.device, &swapchainCreateInfo, NULL, &gSwapchain.swapchain));

	if (swapchainCreateInfo.oldSwapchain)
		rVK_CHECK(sDestroySwapchain(swapchainCreateInfo.oldSwapchain));

	rVK_CHECK(vkGetSwapchainImagesKHR(gVkCore.device, gSwapchain.swapchain, &gSwapchain.imagesCount, NULL));
	gSwapchain.images = calloc(gSwapchain.imagesCount, sizeof(VkImage));
	rVK_CHECK(vkGetSwapchainImagesKHR(gVkCore.device, gSwapchain.swapchain, &gSwapchain.imagesCount, gSwapchain.images));

	gSwapchain.imageViews = calloc(gSwapchain.imagesCount, sizeof(VkImageView));
	for (u32 i = 0; i < gSwapchain.imagesCount; ++i) {
		VkImageViewCreateInfo imageViewCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.viewType = VK_IMAGE_VIEW_TYPE_2D,
			.format = gSwapchain.imageFormat,
			.image = gSwapchain.images[i],
			.subresourceRange = {
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.levelCount = 1,
				.layerCount = 1,
			},
		};
		rVK_CHECK(vkCreateImageView(gVkCore.device, &imageViewCreateInfo, NULL, gSwapchain.imageViews + i));
	}

	gSwapchain.framebuffers = calloc(gSwapchain.imagesCount, sizeof(VkFramebuffer));
	for (u32 i = 0; i < gSwapchain.imagesCount; ++i) {
		VkFramebufferCreateInfo framebufferCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
			.renderPass = gVkCore.renderPass,
			.pAttachments = &gSwapchain.imageViews[i],
			.attachmentCount = 1,
			.width = swapchainExtent.width,
			.height = swapchainExtent.height,
			.layers = 1,
		};
		rVK_CHECK(vkCreateFramebuffer(gVkCore.device, &framebufferCreateInfo, NULL, gSwapchain.framebuffers + i));
	}
	return 0;
}

i32 rVkAcquireNextImage(u32* imageIndex, VkSemaphore semaphore)
{
	rVK_CHECK(vkAcquireNextImageKHR(gVkCore.device, gSwapchain.swapchain, UINT64_MAX, semaphore, VK_NULL_HANDLE, imageIndex));
	return 0;
}

void rVkDestroySwapchain(void)
{
	sDestroySwapchain(gSwapchain.swapchain);
	free(gSwapchain.framebuffers);
	free(gSwapchain.imageViews);
	free(gSwapchain.images);
}
