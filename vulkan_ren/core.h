/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023, Nikita Romanyuk
 */

#pragma once

typedef struct {
	VkInstance instance;
#ifndef NDEBUG
	VkDebugUtilsMessengerEXT debugUtilsMessenger;
#endif
} rVkCore;

extern rVkCore gCore;

/* returns supported instance version on success, otherwise a negative value */
i32 rVkInstanceCreate(void);
void rVkInstanceDestroy(void);
