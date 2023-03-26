/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023, Nikita Romanyuk
 */

#pragma once

#include "defines.h"
#include "vulkan.h"

typedef struct {
	u8* data;
	u64 size;
} rVkShader;

i32 rVkReadShader(rVkShader* self, const char* path);
void rVkFreeShader(rVkShader* self);

i32 rVkCreateShaderModule(VkShaderModule* self, rVkShader* shader);
