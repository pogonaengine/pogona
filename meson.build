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

# Endianness detection
if target_machine.endian() == 'big'
  config.set('PENDIANNESS_BIG', 1)
else
  config.set('PENDIANNESS_LITTLE', 1)
endif
config.set('PENDIANNESS_NAME', target_machine.endian())

# OS detection
if target_machine.system() == 'linux'
  config.set('POS_LINUX', 1)
  config.set('POS_UNIX', 1)
elif target_machine.system() == 'freebsd'
  config.set('POS_FREEBSD', 1)
  config.set('POS_BSD', 1)
  config.set('POS_UNIX', 1)
elif target_machine.system() == 'netbsd'
  config.set('POS_NETBSD', 1)
  config.set('POS_BSD', 1)
  config.set('POS_UNIX', 1)
elif target_machine.system() == 'openbsd'
  config.set('POS_OPENBSD', 1)
  config.set('POS_BSD', 1)
  config.set('POS_UNIX', 1)
elif target_machine.system() == 'dragonfly'
  config.set('POS_DRAGONFLY', 1)
  config.set('POS_BSD', 1)
  config.set('POS_UNIX', 1)
elif target_machine.system() == 'darwin'
  config.set('POS_DARWIN', 1)
  config.set('POS_BSD', 1)
  config.set('POS_UNIX', 1)
elif target_machine.system() == 'windows'
  config.set('POS_WINDOWS', 1)
else
  message('Your OS is not supported, be aware of compilation/runtime bugs!')
endif
config.set('POS_NAME', target_machine.system())

# Compiler detection
if cc.get_id() == 'clang'
  config.set('PCC_CLANG', 1)
elif cc.get_id() == 'gcc'
  config.set('PCC_GCC', 1)
elif cc.get_id() == 'msvc'
  config.set('PCC_MSVC', 1)
else
  message('Your compiler is not supported, be aware of compilation bugs!')
endif
config.set('PCC_NAME', cc.get_id())

# Linker detection
if cc.get_linker_id() == 'ld.lld'
  config.set('PLD_LLD', 1)
elif cc.get_linker_id() == 'ld.bfd'
  config.set('PLD_BFD', 1)
elif cc.get_linker_id() == 'ld.mold'
  config.set('PLD_MOLD', 1)
elif cc.get_linker_id() == 'link'
  config.set('PLD_LINK', 1)
else
  message('Your linker is not supported, be aware of compilation bugs!')
endif
config.set('PLD_NAME', cc.get_linker_id())

if cc.get_id() == 'clang'
  add_project_arguments('-Wno-gnu', language: 'c')
endif

subdir('engine')
subdir('vulkan')
subdir('runner')
subdir('tests')

configure_file(output: 'config.h', configuration: config)
