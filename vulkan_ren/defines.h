/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023, Nikita Romanyuk
 */

#pragma once

#include "engine.h"
#include <engine/logger.h>

#define RVK_CHECK(x)                                     \
	do {                                                   \
		if ((x) < 0) {                                       \
			pLoggerError("Vulkan call '%s' failed: %d\n", #x); \
			return -1;                                         \
		}                                                    \
	} while (false);
