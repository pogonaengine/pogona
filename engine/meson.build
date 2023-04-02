libpogona_src = files(
	'window/window.c',
  'engine.c',
  'event.c',
  'logger.c',
  'renderer.c',
  'util.c',
  'vector.c',
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
