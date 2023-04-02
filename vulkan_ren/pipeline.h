/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023, Nikita Romanyuk
 */

#pragma once

#include "defines.h"
#include "vulkan.h"

i32 rVkCreatePipelineLayoutAndCache(void);
i32 rVkCreateGraphicsPipeline(VkShaderModule vertexShader, VkShaderModule fragmentShader);