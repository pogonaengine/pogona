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

i32 rVkCreateGraphicsPipeline(VkShaderModule vertexShader, VkShaderModule fragmentShader)
{
	VkPipelineShaderStageCreateInfo pipelineShaderStageCreateInfo[2] = {
		[0] = {
			.sType	= VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			.stage	= VK_SHADER_STAGE_VERTEX_BIT,
			.module = vertexShader,
			.pName	= "main",
		},
		[1]	= {
			.sType	= VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			.stage	= VK_SHADER_STAGE_FRAGMENT_BIT,
			.module = fragmentShader,
			.pName	= "main",
		},
	};

	VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
	};
	VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo = {
		.sType    = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
		.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
	};
	VkPipelineViewportStateCreateInfo viewportStateCreateInfo = {
		.sType				 = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
		.viewportCount = 1,
		.scissorCount	 = 1,
	};
	VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
		.lineWidth = 1.f,
	};
	VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo = {
		.sType								= VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
		.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
	};
	VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
	};
	VkPipelineColorBlendAttachmentState colorBlendAttachmentState = {
		.colorWriteMask = VK_COLOR_COMPONENT_R_BIT
										| VK_COLOR_COMPONENT_G_BIT
										| VK_COLOR_COMPONENT_B_BIT
										| VK_COLOR_COMPONENT_A_BIT,
	};
	VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo = {
		.sType					 = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
		.attachmentCount = 1,
		.pAttachments		 = &colorBlendAttachmentState,
	};
	VkDynamicState dynamicStates[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
	VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = {
		.sType						 = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
		.dynamicStateCount = sizeof(dynamicStates) / sizeof(dynamicStates[0]),
		.pDynamicStates		 = dynamicStates,
	};
	VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo = {
		.sType							 = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		.layout							 = gCore.pipeline.layout,
		.renderPass					 = gCore.renderPass,
		.stageCount					 = sizeof(pipelineShaderStageCreateInfo) / sizeof(pipelineShaderStageCreateInfo[0]),
		.pStages						 = pipelineShaderStageCreateInfo,
		.pVertexInputState	 = &vertexInputStateCreateInfo,
		.pInputAssemblyState = &inputAssemblyStateCreateInfo,
		.pViewportState			 = &viewportStateCreateInfo,
		.pRasterizationState = &rasterizationStateCreateInfo,
		.pMultisampleState	 = &multisampleStateCreateInfo,
		.pDepthStencilState	 = &depthStencilStateCreateInfo,
		.pColorBlendState		 = &colorBlendStateCreateInfo,
		.pDynamicState			 = &dynamicStateCreateInfo,
	};
	rCHECK(vkCreateGraphicsPipelines(gCore.device, gCore.pipeline.cache, 1, &graphicsPipelineCreateInfo, NULL, &gCore.pipeline.pipeline));
	return 0;
}
