/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023, Nikita Romanyuk
 */

#include "core.h"
#include "pipeline.h"
#include <pch/pch.h>

i32 rVkCreatePipelineLayoutAndCache(void)
{
	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
	};
	rCHECK(vkCreatePipelineLayout(gCore.device, &pipelineLayoutCreateInfo, NULL, &gCore.pipeline.layout));

	VkPipelineCacheCreateInfo pipelineCacheCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,
	};
	rCHECK(vkCreatePipelineCache(gCore.device, &pipelineCacheCreateInfo, NULL, &gCore.pipeline.cache));
	return 0;
}

