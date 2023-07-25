project(
  'trujnoknt',
  [ 'cpp', 'c' ],
  version : '0.1.0',
  license: 'mit'
)

if(meson.get_compiler('cpp').get_id() == 'msvc') # have to do that to avoid MSVC breaking code with their min/max macros
  add_project_arguments('-DNOMINMAX', language: 'cpp')
endif

local_deps = [
  dependency('fmt', required: true),
  dependency('glfw3', required: true),
  dependency('glm', required: true),
]

compilation_options = [
  'cpp_std=c++20'
]

include_dirs = [
  include_directories('include')
]

sources = files(
  'src/glad.c',

  'src/main.cpp',
  'src/callbacks.cpp',
  'src/stb_image.cpp',
  'src/texture.cpp',
  'src/billboard.cpp'
)

executable(
  'turjnoknt',
  sources,
  dependencies: local_deps,
  include_directories: include_dirs,
  override_options: compilation_options,
)
