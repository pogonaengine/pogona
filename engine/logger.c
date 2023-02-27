/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023, Nikita Romanyuk
 */

#include "pch/pch.h"
#include <pogona/logger.h>

static const char* sLevelNames[pLOGGER_LEVEL_COUNT] = {
	[pLOGGER_DEBUG]   = "DEBUG  ",
	[pLOGGER_INFO]    = "INFO   ",
	[pLOGGER_WARNING] = "WARNING",
	[pLOGGER_ERROR]   = "ERROR  ",
};

#define RESET_COLOUR "\x1b[0m"

static const char* sLevelColours[pLOGGER_LEVEL_COUNT] = {
	[pLOGGER_DEBUG]   = "\x1b[36m",
	[pLOGGER_INFO]    = "\x1b[32m",
	[pLOGGER_WARNING] = "\x1b[33m",
	[pLOGGER_ERROR]   = "\x1b[31m",
};

#ifndef NDEBUG
static i8 sLevel = pLOGGER_DEBUG;
#else
static i8 sLevel = pLOGGER_INFO;
#endif

void _pLoggerLog(i8 level, const char* fmt, ...)
{
	if (level < sLevel) return;

	va_list ap;
	va_start(ap, fmt);

	printf("%s[%-7s]%s ", sLevelColours[level], sLevelNames[level], RESET_COLOUR);
	vprintf(fmt, ap);

	va_end(ap);
}
