shaders = files(
  '2d.vert.glsl',
  '2d.frag.glsl',
)

glslang_validator = find_program('glslangValidator', required: true)
foreach shader : shaders
  target_name = '@0@'.format(shader)
  target_name = target_name.split('/')[-1]
  vulkan_ren_custom_targets += custom_target(
    '@0@'.format(target_name),
    command: [glslang_validator, '-V', '@INPUT@', '-o', '@OUTPUT@'],
    input: shader,
    output: '@BASENAME@.spv',
    install: true,
    install_dir: 'shaders',
  )
endforeach
