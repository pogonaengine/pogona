/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023, Nikita Romanyuk
 */

#include "engine.h"
#include "event.h"
#include "logger.h"
#include "renderer/renderer.h"
#include "window/window.h"
#include <config.h>
#include <pch/pch.h>

static rVertex sVertices[4] = {
	{ {{-0.5f, -0.5f, 0.0f}}, {{1.0f, 0.0f, 0.0f, 1.0f}} },
	{ {{ 0.5f, -0.5f, 0.0f}}, {{0.0f, 1.0f, 0.0f, 1.0f}} },
	{ {{ 0.5f,  0.5f, 0.0f}}, {{0.0f, 0.0f, 1.0f, 1.0f}} },
	{ {{-0.5f,  0.5f, 0.0f}}, {{1.0f, 1.0f, 1.0f, 1.0f}} },
};
static u16 sIndices[6] = {
	0, 1, 2, 2, 3, 0,
};
static const rMesh sMesh = {
	.vertices = sVertices,
	.verticesCount = 4,
	.indices = sIndices,
	.indicesCount = 6,
};

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
	/* FIXME: don't hardcode it
	 * FIXME: provide a way for the user to change it (envvar e.g.)
	 * FIXME: loop through an array of renderer paths, based on priority
	 */
	error = pRendererLoad(&renderer, "libpogona_vulkan_ren.so");
	if (error < 0) {
		pLoggerError("Couldn't load renderer\n");
		pWindowDestroy(&window);
		pEventSystemDestroy();
		goto exit;
	}
	pLoggerInfo("Renderer: %s\n", renderer.name);

	error = renderer.entry->create(&window);
	if (error < 0) {
		pLoggerError("Couldn't create renderer\n");
		pWindowDestroy(&window);
		pEventSystemDestroy();
		pRendererUnload(&renderer);
		goto exit;
	}

	while (window.isRunning) {
		error = renderer.entry->beginFrame();
		if (error < 0) {
			pLoggerError("Couldn't begin a frame. Dying\n");
			renderer.entry->destroy();
			pRendererUnload(&renderer);
			pWindowDestroy(&window);
			pEventSystemDestroy();
			goto exit;
		}

		renderer.entry->drawMesh(&sMesh);

		error = renderer.entry->endFrame();
		if (error < 0) {
			pLoggerWarning("Couldn't end frame. Dying\n");
			renderer.entry->destroy();
			pRendererUnload(&renderer);
			pWindowDestroy(&window);
			pEventSystemDestroy();
			goto exit;
		}
		window.pollEvents(&window);

		pEventSend(pEVENT_FRAME, NULL);
	}

	renderer.entry->destroy();
	pRendererUnload(&renderer);
	pWindowDestroy(&window);
	pEventSystemDestroy();

exit:
	return error;
}
