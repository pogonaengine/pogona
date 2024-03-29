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
#ifdef pWAYLAND
# define VK_USE_PLATFORM_WAYLAND_KHR
#endif
#ifdef pXCB
# define VK_USE_PLATFORM_XCB_KHR
#endif
#ifdef pXLIB
# define VK_USE_PLATFORM_XLIB_KHR
#endif
#include <volk.h>
