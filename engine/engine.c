/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023, Nikita Romanyuk
 */

#include "engine.h"
#include "logger.h"
#include "renderer.h"
#include <config.h>
#include <pch/pch.h>

extern PAPI i32 pEngineEntry(int argc, char** argv)
{
	(void) argc;
	(void) argv;

	i32 error = 0;

	pWindow window = { 0 };
	error = pWindowCreate(&window, "pogona", 800, 600);
	if (error < 0) {
		pLoggerError("Couldn't create window\n");
		goto exit;
	}

	pRenderer renderer = { 0 };
	error = pRendererPopulate(&renderer);
	if (error < 0) {
		pLoggerError("Couldn't populate renderer\n");
		pWindowDestroy(&window);
		goto exit;
	}

	error = renderer.create(&window);
	if (error < 0) {
		pLoggerDebug("Couldn't create renderer\n");
		pWindowDestroy(&window);
		goto exit;
	}

	/* while isRunning { ... } */
	renderer.destroy();

	pWindowDestroy(&window);

exit:
	return error;
}
