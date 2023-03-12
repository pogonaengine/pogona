libpogona_src = [
  'file_api.c',
  'logger.c',
  'engine.c',
  'vector.c',
]

libpogona_include = [
  include_directories('../include'),
  include_directories('../pch'),
]

libpogona_deps = [
  libm,
  libdl,
]

libpogona = library(
  'pogona',
  libpogona_src,
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
