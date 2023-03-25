/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023, Nikita Romanyuk
 */

#pragma once

#include <config.h>

#ifdef pOS_WINDOWS
# define VK_USE_PLATFORM_WIN32_KHR
#endif
#include <volk.h>
