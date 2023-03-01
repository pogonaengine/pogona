/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023, Nikita Romanyuk
 */

#include "engine.h"
#include <pch.h>
#include <pogona/logger.h>

int32_t pEngineInit(int argc, char** argv)
{
	(void) argc;
	(void) argv;

	pLoggerInfo("pEngineInit() was called\n");
	return 0;
}

void pEngineShutdown(void)
{
	pLoggerInfo("pEngineShutdown() was called\n");
}
