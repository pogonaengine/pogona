/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023, Nikita Romanyuk
 */

#include "core.h"
#include "memory.h"
#include <pch/pch.h>

static i32 sPickMemoryType(u32 memoryTypeBits,
                           VkMemoryPropertyFlags flags)
{
	const VkPhysicalDeviceMemoryProperties* memoryProperties = &gVkCore.physicalDevice.memoryProperties;

	/* memoryTypeBits is a bitmask of memory type indices, which we can use.
	 * We loop through all the memory types and check if its bit is set.
	 * Then, we check if it supports the flags we need. If that's the case, return the index.
	 *
	 * Detailed explanation: https://www.youtube.com/watch?v=nKCzD5iK71M&t=2790s
	 */
	for (u32 i = 0; i < memoryProperties->memoryTypeCount; ++i) {
		if (memoryTypeBits & (1 << i) && (memoryProperties->memoryTypes[i].propertyFlags & flags) == flags)
				return i;
	}

	pLoggerError("Couldn't pick a compatible memory type\n");
	return -1;
}

i32 rVkCreateBuffer(rVkBuffer* self,
                    VkDeviceSize size,
                    VkBufferUsageFlags usage,
                    VkMemoryPropertyFlags flags)
{
	VkBuffer buffer = NULL;
	VkDeviceMemory memory = NULL;
	void* data = NULL;

	VkBufferCreateInfo bufferCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.size = size,
		.usage = usage,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
	};
	rVK_CHECK(vkCreateBuffer(gVkCore.device, &bufferCreateInfo, NULL, &buffer));

	VkMemoryRequirements memoryRequirements;
	vkGetBufferMemoryRequirements(gVkCore.device, buffer, &memoryRequirements);

	u32 memoryTypeIndex = sPickMemoryType(memoryRequirements.memoryTypeBits, flags);
	assert(memoryTypeIndex >= 0);

	VkMemoryAllocateInfo memoryAllocateInfo = {
		.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.allocationSize = memoryRequirements.size,
		.memoryTypeIndex = memoryTypeIndex,
	};
	rVK_CHECK(vkAllocateMemory(gVkCore.device, &memoryAllocateInfo, NULL, &memory));
	rVK_CHECK(vkBindBufferMemory(gVkCore.device, buffer, memory, 0));

	if (flags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
		rVK_CHECK(vkMapMemory(gVkCore.device, memory, 0, size, 0, &data));

	self->buffer = buffer;
	self->memory = memory;
	self->data   = data;
	self->size   = size;
	return 0;
}

void rVkDestroyBuffer(rVkBuffer* self)
{
	vkDestroyBuffer(gVkCore.device, self->buffer, NULL);
	vkFreeMemory(gVkCore.device, self->memory, NULL);
}
