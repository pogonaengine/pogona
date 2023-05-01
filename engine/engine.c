/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023, Nikita Romanyuk
 */

#include "engine.h"
#include "event.h"
#include "logger.h"
#include "renderer.h"
#include <config.h>
#include <pch/pch.h>

extern i32 pEngineEntry(int argc, char** argv)
{
	(void) argc;
	(void) argv;

	i32 error = 0;

	error = pEventSystemCreate();
	if (error < 0) {
		pLoggerError("Couldn't create event system\n");
		goto exit;
	}

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

	while (window.isRunning) {
		error = renderer.beginFrame();
		if (error < 0) {
			pLoggerError("Couldn't begin a frame. Dying\n");
			goto exit;
		}
		error = renderer.endFrame();
		if (error < 0) {
			pLoggerWarning("Couldn't end frame\n");
			goto exit;
		}
		pWindowPollEvents(&window);

		pEventSend(pEVENT_FRAME, NULL);
	}

	renderer.destroy();
	pWindowDestroy(&window);
	pEventSystemDestroy();

exit:
	return error;
}
