/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023, Nikita Romanyuk
 */

#include "core.h"
#include "swapchain.h"
#include <pch/pch.h>

rSwapchain gSwapchain = { 0 };

static VkFormat sPickImageFormat(void)
{
	u32 formatsCount = gCore.surface.surfaceFormatsCount;
	VkSurfaceFormatKHR* formats = gCore.surface.surfaceFormats;

	if (formatsCount == 1 && formats[0].format == VK_FORMAT_UNDEFINED)
		return VK_FORMAT_R8G8B8A8_SRGB;

	for (u32 i = 0; i < formatsCount; ++i) {
		if (formats[i].format == VK_FORMAT_R8G8B8A8_SRGB || formats[i].format == VK_FORMAT_B8G8R8A8_SRGB)
			return formats[i].format;
	}

	return formats[0].format;
}

static VkColorSpaceKHR sPickColorSpace(void)
{
	u32 formatsCount = gCore.surface.surfaceFormatsCount;
	VkSurfaceFormatKHR* formats = gCore.surface.surfaceFormats;

	for (u32 i = 0; i < formatsCount; ++i) {
		if (formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			return formats[i].colorSpace;
	}

	return formats[0].colorSpace;
}

i32 rVkCreateSwapchain(pWindow* window)
{
	gSwapchain.imageFormat = sPickImageFormat();
	gSwapchain.colorSpace  = sPickColorSpace();

	VkSurfaceCapabilitiesKHR surfaceCapabilities;
	rCHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gCore.physicalDevice.physicalDevice, gCore.surface.surface, &surfaceCapabilities));

	VkExtent2D swapchainExtent = surfaceCapabilities.currentExtent;
	if (swapchainExtent.width == 0xffffffff || swapchainExtent.height == 0xffffffff) {
		swapchainExtent.width  = pCLAMP(window->width, surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width);
		swapchainExtent.height = pCLAMP(window->height, surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height);
	}

	gSwapchain.width  = swapchainExtent.width;
	gSwapchain.height = swapchainExtent.height;

	VkSwapchainCreateInfoKHR swapchainCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		.surface = gCore.surface.surface,
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
		.pQueueFamilyIndices = &gCore.physicalDevice.queueFamilyIndex,
		.preTransform = surfaceCapabilities.currentTransform,
		.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
	};
	rCHECK(vkCreateSwapchainKHR(gCore.device, &swapchainCreateInfo, NULL, &gSwapchain.swapchain));

	rCHECK(vkGetSwapchainImagesKHR(gCore.device, gSwapchain.swapchain, &gSwapchain.imagesCount, NULL));
	gSwapchain.images = calloc(gSwapchain.imagesCount, sizeof(VkImage));
	rCHECK(vkGetSwapchainImagesKHR(gCore.device, gSwapchain.swapchain, &gSwapchain.imagesCount, gSwapchain.images));
	return 0;
}

i32 rVkAcquireNextImage(u32* imageIndex, VkSemaphore semaphore)
{
	rCHECK(vkAcquireNextImageKHR(gCore.device, gSwapchain.swapchain, UINT64_MAX, semaphore, VK_NULL_HANDLE, imageIndex));
	return 0;
}

void rVkDestroySwapchain(void)
{
	for (u32 i = 0; i < gSwapchain.imagesCount; ++i) {
		vkDestroyFramebuffer(gCore.device, gSwapchain.framebuffers[i], NULL);
		vkDestroyImageView(gCore.device, gSwapchain.imageViews[i], NULL);
	}
	vkDestroySwapchainKHR(gCore.device, gSwapchain.swapchain, NULL);
	free(gSwapchain.framebuffers);
	free(gSwapchain.imageViews);
	free(gSwapchain.images);
}

i32 rVkInitImageViews(void)
{
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
		rCHECK(vkCreateImageView(gCore.device, &imageViewCreateInfo, NULL, gSwapchain.imageViews + i));
	}
	return 0;
}

i32 rVkInitFramebuffers(void)
{
	gSwapchain.framebuffers = calloc(gSwapchain.imagesCount, sizeof(VkFramebuffer));
	for (u32 i = 0; i < gSwapchain.imagesCount; ++i) {
		VkFramebufferCreateInfo framebufferCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
			.renderPass = gCore.renderPass,
			.pAttachments = &gSwapchain.imageViews[i],
			.attachmentCount = 1,
			/* FIXME: don't hardcode */
			.width = 800,
			.height = 600,
			.layers = 1,
		};
		rCHECK(vkCreateFramebuffer(gCore.device, &framebufferCreateInfo, NULL, gSwapchain.framebuffers + i));
	}
	return 0;
}
