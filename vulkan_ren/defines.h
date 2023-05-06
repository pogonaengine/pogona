/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023, Nikita Romanyuk
 */

#pragma once

#include <engine/logger.h>

#define rVK_CHECK(x) \
	do { \
		i32 result = (x); \
		if (result < 0) { \
			pLoggerError("Vulkan call '%s' failed: %d\n", #x, result); \
			return -1; \
		} \
	} while (false);
