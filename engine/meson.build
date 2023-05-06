libpogona_src = files(
	'engine.c',
	'event.c',
	'logger.c',
	'renderer/renderer.c',
	'util.c',
	'vector.c',
	'window/window.c',
)

libpogona_include = [
	include_directories('..'),
]

libpogona_deps = [
	libm,
]

libpogona_custom_targets = []

libwayland_client = dependency('wayland-client', required: get_option('wayland'))
if libwayland_client.found()
	libpogona_deps += libwayland_client
	wayland_module = import('unstable-wayland')
	xdg_shell = wayland_module.scan_xml(wayland_module.find_protocol('xdg-shell'))
	libpogona_custom_targets += xdg_shell

	libpogona_src += files(
		'window/wayland.c',
	)
	config.set('pWAYLAND', 1)
endif

libXlib = dependency('X11', required: get_option('xlib'))
if libXlib.found()
	libpogona_deps += libXlib
	libpogona_src += files(
		'window/xlib.c',
	)
	config.set('pXLIB', 1)
endif

libXCB = dependency('xcb', required: get_option('xcb'))
if libXCB.found()
	libpogona_deps += libXCB
	libpogona_src += files(
		'window/xcb.c',
	)
	config.set('pXCB', 1)
endif

if get_option('vulkan').enabled()
	libpogona_src += vulkan_ren_src
	libpogona_deps += [
		subproject('vulkan-headers').get_variable('vulkan_headers_dep'),
		subproject('volk').get_variable('volk_dep'),
	]
	libpogona_custom_targets += vulkan_ren_custom_targets
	config.set('pVULKAN', 1)
endif

libpogona = library(
	'pogona',
	libpogona_src,
	libpogona_custom_targets,
	dependencies: libpogona_deps,
	c_pch: '../pch/pch.h',
	include_directories: libpogona_include,
	install: true,
	install_dir: '',
)
libpogona_dep = declare_dependency(
	link_with: libpogona,
	include_directories: libpogona_include,
)
