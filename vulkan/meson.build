libpogona_vulkan_src = [
  'entry.c',
]

libpogona_vulkan_include = [
  include_directories('../include'),
  include_directories('../pch'),
]

libpogona_vulkan_deps = [
  libm,
]

libpogona_vulkan = library(
  'pogona_vulkan',
  libpogona_vulkan_src,
  dependencies: libpogona_vulkan_deps,
  c_pch: '../pch/pch.h',
  include_directories: libpogona_vulkan_include,
  install: true,
  install_dir: '',
)
