vulkan_ren_src = files(
	'core.c',
	'entry.c',
	'memory.c',
	'pipeline.c',
	'render_pass.c',
	'shader.c',
	'surface.c',
	'swapchain.c',
	'synchronization.c',
	'vulkan.c',
)

vulkan_ren_deps = [
	subproject('vulkan-headers').get_variable('vulkan_headers_dep'),
	subproject('volk').get_variable('volk_dep'),
	libpogona_dep,
]

vulkan_ren_custom_targets = []
subdir('shaders')

if libwayland_client.found()
	vulkan_ren_custom_targets += xdg_shell
endif

libpogona_vulkan_ren = shared_library(
	'pogona_vulkan_ren',
	vulkan_ren_src,
	vulkan_ren_custom_targets,
	dependencies: vulkan_ren_deps,
	include_directories: include_directories('..'),
	c_pch: '../pch/pch.h',
	install: true,
	install_dir: '',
)
