runner_include = [
  include_directories('../include'),
  include_directories('../pch'),
]

runner_deps = [
  # it's only required on linux
  # TODO: check if running on linux
  cc.find_library('dl', required: false),
]

runner = executable(
  'runner',
  'main.c',
  dependencies: runner_deps,
  c_pch: '../pch/pch.h',
  include_directories: runner_include,
  install: true,
  install_dir: '',
)
