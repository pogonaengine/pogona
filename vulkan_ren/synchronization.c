/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023, Nikita Romanyuk
 */

#include "core.h"
#include "synchronization.h"
#include <pch/pch.h>

i32 rVkCreateSemaphore(VkSemaphore* semaphore)
{
	VkSemaphoreCreateInfo semaphoreCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
	};
	rVK_CHECK(vkCreateSemaphore(gVkCore.device, &semaphoreCreateInfo, NULL, semaphore));
	return 0;
}

i32 rVkCreateFence(VkFence* fence, bool signaled)
{
	VkFenceCreateInfo fenceCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		.flags = signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0,
	};
	rVK_CHECK(vkCreateFence(gVkCore.device, &fenceCreateInfo, NULL, fence));
	return 0;
}
