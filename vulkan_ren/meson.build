libpogona_vulkan_ren_src = [
  'core.c',
  'entry.c',
  'vulkan.c',
]

libpogona_vulkan_ren_include = [
  include_directories('..'),
]

libpogona_vulkan_ren_deps = [
  libm,
  subproject('vulkan-headers').get_variable('vulkan_headers_dep'),
  subproject('volk').get_variable('volk_dep'),
]

libpogona_vulkan = library(
  'pogona_vulkan_ren',
  libpogona_vulkan_ren_src,
  dependencies: libpogona_vulkan_ren_deps,
  c_args: '-DpRENDERER',
  c_pch: '../pch/vulkan.h',
  include_directories: libpogona_vulkan_ren_include,
  install: true,
  install_dir: '',
)
