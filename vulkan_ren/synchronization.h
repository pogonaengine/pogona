/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023, Nikita Romanyuk
 */

#pragma once

#include "defines.h"
#include "vulkan.h"

i32 rVkCreateSemaphore(VkSemaphore* semaphore);
i32 rVkCreateFence(VkFence* fence, bool signaled);
