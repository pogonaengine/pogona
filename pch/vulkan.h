/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023, Nikita Romanyuk
 */

#ifndef _VULKAN_PCH_H
#define _VULKAN_PCH_H

#include "pch.h"

#ifdef POS_WINDOWS
# define VK_USE_PLATFORM_WIN32_KHR
#endif
/* TODO: other defines */
#include <volk.h>

#endif /* _VULKAN_PCH_H */
