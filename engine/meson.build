libpogona_src = [
  'pogona.c',
]

libpogona_include = [
  include_directories('../include'),
]

libpogona_deps = [
  cc.find_library('m', required: false),
  cc.find_library('dl', required: false),
]

libpogona = library(
  'pogona',
  libpogona_src,
  version: meson.project_version(),
  dependencies: libpogona_deps,
  c_pch: 'pch/pch.h',
  c_args: ['-DPEXPORT'],
  gnu_symbol_visibility: 'hidden',
  include_directories: libpogona_include,
  install: true,
)
libpogona_dep = declare_dependency(
  link_with: libpogona,
  include_directories: libpogona_include,
)
