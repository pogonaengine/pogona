/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023, Nikita Romanyuk
 */

#pragma once

#include <engine/window/window.h>
#include "vulkan.h"

i32 rVkCreateSurface(pWindow* window);
void rVkDestroySurface(void);
