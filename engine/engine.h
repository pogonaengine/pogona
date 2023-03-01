/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023, Nikita Romanyuk
 */

#pragma once

#include <pogona/types.h>
#include <pogona/visibility.h>

PAPI int32_t pEngineInit(int argc, char** argv);
PAPI void    pEngineShutdown(void);
