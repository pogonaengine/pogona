/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023, Nikita Romanyuk
 */

#pragma once

#include "defines.h"

enum {
	pLOGGER_DEBUG,
	pLOGGER_INFO,
	pLOGGER_WARNING,
	pLOGGER_ERROR,

	pLOGGER_LEVEL_COUNT, /* do not use this as a level */
};

PAPI void _pLoggerLog(i8 level, const char* fmt, ...);

#ifndef PRENDERER
# define pLoggerDebug(...)   _pLoggerLog(pLOGGER_DEBUG,   __VA_ARGS__)
# define pLoggerInfo(...)    _pLoggerLog(pLOGGER_INFO,    __VA_ARGS__)
# define pLoggerWarning(...) _pLoggerLog(pLOGGER_WARNING, __VA_ARGS__)
# define pLoggerError(...)   _pLoggerLog(pLOGGER_ERROR,   __VA_ARGS__)
#else
# define pLoggerDebug(...)   gEngine->loggerLog(pLOGGER_DEBUG,   __VA_ARGS__)
# define pLoggerInfo(...)    gEngine->loggerLog(pLOGGER_INFO,    __VA_ARGS__)
# define pLoggerWarning(...) gEngine->loggerLog(pLOGGER_WARNING, __VA_ARGS__)
# define pLoggerError(...)   gEngine->loggerLog(pLOGGER_ERROR,   __VA_ARGS__)
#endif
