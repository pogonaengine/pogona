/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023, Nikita Romanyuk
 */

#pragma once

#include "defines.h"
#include "vulkan.h"

typedef struct {
	VkBuffer buffer;
	VkDeviceMemory memory;
	void* data;
	VkDeviceSize size;
} rVkBuffer;

i32 rVkCreateBuffer(rVkBuffer* self,
                    VkDeviceSize size,
                    VkBufferUsageFlags usage,
                    VkMemoryPropertyFlags flags);
void rVkDestroyBuffer(rVkBuffer* self);
