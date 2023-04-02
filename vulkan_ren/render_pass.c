/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023, Nikita Romanyuk
 */

#include "core.h"
#include "render_pass.h"
#include "swapchain.h"
#include <pch/pch.h>

i32 rVkCreateRenderPass(void)
{
	VkAttachmentDescription attachmentDescriptions[1] = {
		[0] = {
			.format = gSwapchain.imageFormat,
			.samples = VK_SAMPLE_COUNT_1_BIT,
			.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
			.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
			.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
			.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
			.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
			.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
		},
	};
	VkAttachmentReference colourAttachments[1] = {
		[0] = { 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL },
	};
	VkSubpassDescription subpassDescriptions[1] = {
		[0] = {
			.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
			.colorAttachmentCount = pARRAY_SIZE(colourAttachments),
			.pColorAttachments = colourAttachments,
		},
	};
	VkRenderPassCreateInfo renderPassCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
		.attachmentCount = pARRAY_SIZE(attachmentDescriptions),
		.pAttachments = attachmentDescriptions,
		.subpassCount = pARRAY_SIZE(subpassDescriptions),
		.pSubpasses = subpassDescriptions,
	};
	rCHECK(vkCreateRenderPass(gCore.device, &renderPassCreateInfo, NULL, &gCore.renderPass));
	return 0;
}
