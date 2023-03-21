runner_include = [
  include_directories('../include'),
  include_directories('../engine'),
  include_directories('../pch'),
]

runner_deps = [
  libdl,
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
