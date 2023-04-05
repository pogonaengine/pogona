/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023, Nikita Romanyuk
 */

#pragma once

#include "defines.h"
#include "vulkan.h"

typedef struct {
	/* the shader should be already loaded */
	VkShaderModule module;
	VkShaderStageFlagBits stage;
} rVkShaderStage;

typedef struct {
	const rVkShaderStage* stages;
	u16 stagesCount;
} rVkGraphicsPipelineCreateInfo;

i32 rVkCreatePipelineLayoutAndCache(void);
i32 rVkCreateGraphicsPipeline(const rVkGraphicsPipelineCreateInfo createInfo);
