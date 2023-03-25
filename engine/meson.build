libpogona_src = [
	'window/window.c',
  'engine.c',
  'logger.c',
  'renderer.c',
  'util.c',
  'vector.c',
]

libpogona_include = [
  include_directories('..'),
]

libpogona_deps = [
  libm,
  libdl,
]

libpogona_custom_targets = []

libwayland_client = dependency('wayland-client', required: get_option('wayland'))
if libwayland_client.found()
  libpogona_deps += libwayland_client
  wayland_module = import('unstable-wayland')
  xdg_shell = wayland_module.scan_xml(wayland_module.find_protocol('xdg-shell'))
  libpogona_custom_targets += xdg_shell

  libpogona_src += [
    'window/wayland.c',
  ]
  config.set('pWAYLAND', 1)
endif

libpogona = library(
  'pogona',
  libpogona_src,
  libpogona_custom_targets,
  dependencies: libpogona_deps,
  c_pch: '../pch/pch.h',
  c_args: ['-DPEXPORT'],
  gnu_symbol_visibility: 'hidden',
  include_directories: libpogona_include,
  install: true,
  install_dir: '',
)
libpogona_dep = declare_dependency(
  link_with: libpogona,
  include_directories: libpogona_include,
)
