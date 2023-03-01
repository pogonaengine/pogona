project(
  'pogona',
  'c',
  version: '0.1.0',
  license: 'BSD-2-Clause',
  default_options: [
    'c_std=gnu11',
    'warning_level=2',
  ],
)

cc = meson.get_compiler('c')

subdir('engine')
subdir('vulkan')
subdir('runner')
subdir('tests')
