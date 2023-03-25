project(
  'pogona',
  'c',
  version: '0.1.0',
  license: 'BSD-2-Clause',
  default_options: [
    'c_std=gnu11',
    'warning_level=3',
    'b_lto_mode=thin',
    'b_thinlto_cache=true',
    'b_pie=true',
  ],
)

cc = meson.get_compiler('c')
libm = cc.find_library('m', required: target_machine.system() == 'linux')
libdl = cc.find_library('dl', required: target_machine.system() == 'linux')

config = configuration_data()

version_parts = meson.project_version().split('.')
config.set('pVERSION', '@0@'.format(meson.project_version()))
config.set('pVERSION_MAJOR', '@0@'.format(version_parts[0]))
config.set('pVERSION_MINOR', '@0@'.format(version_parts[1]))
config.set('pVERSION_PATCH', '@0@'.format(version_parts[2]))

# Endianness detection
if target_machine.endian() == 'big'
  config.set('pENDIANNESS_BIG', 1)
else
  config.set('pENDIANNESS_LITTLE', 1)
endif
config.set('pENDIANNESS_NAME', target_machine.endian())

# OS detection
if target_machine.system() == 'linux'
  config.set('pOS_LINUX', 1)
  config.set('pOS_UNIX', 1)
elif target_machine.system() == 'freebsd'
  config.set('pOS_FREEBSD', 1)
  config.set('pOS_BSD', 1)
  config.set('pOS_UNIX', 1)
elif target_machine.system() == 'netbsd'
  config.set('pOS_NETBSD', 1)
  config.set('pOS_BSD', 1)
  config.set('pOS_UNIX', 1)
elif target_machine.system() == 'openbsd'
  config.set('pOS_OPENBSD', 1)
  config.set('pOS_BSD', 1)
  config.set('pOS_UNIX', 1)
elif target_machine.system() == 'dragonfly'
  config.set('pOS_DRAGONFLY', 1)
  config.set('pOS_BSD', 1)
  config.set('pOS_UNIX', 1)
elif target_machine.system() == 'darwin'
  config.set('pOS_DARWIN', 1)
  config.set('pOS_BSD', 1)
  config.set('pOS_UNIX', 1)
elif target_machine.system() == 'windows'
  config.set('pOS_WINDOWS', 1)
else
  message('Your OS is not supported, be aware of compilation/runtime bugs!')
endif
config.set('pOS_NAME', target_machine.system())

# Compiler detection
if cc.get_id() == 'clang'
  config.set('pCC_CLANG', 1)
elif cc.get_id() == 'gcc'
  config.set('pCC_GCC', 1)
elif cc.get_id() == 'msvc'
  config.set('pCC_MSVC', 1)
else
  message('Your compiler is not supported, be aware of compilation bugs!')
endif
config.set('pCC_NAME', cc.get_id())

# Linker detection
if cc.get_linker_id() == 'ld.lld'
  config.set('pLD_LLD', 1)
elif cc.get_linker_id() == 'ld.bfd'
  config.set('pLD_BFD', 1)
elif cc.get_linker_id() == 'ld.mold'
  config.set('pLD_MOLD', 1)
elif cc.get_linker_id() == 'link'
  config.set('pLD_LINK', 1)
else
  message('Your linker is not supported, be aware of compilation bugs!')
endif
config.set('pLD_NAME', cc.get_linker_id())

if cc.get_id() == 'clang'
  add_project_arguments('-Wno-gnu', language: 'c')
endif

subdir('engine')
subdir('vulkan_ren')
subdir('runner')
subdir('tests')

configure_file(output: 'config.h', configuration: config)
