project(
  'trujnoknt',
  [ 'cpp', 'c' ],
  version : '0.1.0',
  license: 'mit'
)

if(meson.get_compiler('cpp').get_id() == 'msvc') # have to do that to avoid MSVC breaking code with their min/max macros
  add_project_arguments('-DNOMINMAX', language: 'cpp')
endif

if(meson.get_compiler('cpp').get_id() == 'clang')
  add_project_arguments(['-stdlib=libc++'], language: 'cpp')
endif

cmake = import('cmake')

build_type_opts = cmake.subproject_options()
build_type_opts.add_cmake_defines( { 'CMAKE_BUILD_TYPE': get_option('buildtype').to_upper() } )

libassert_sub = cmake.subproject('libassert', options: build_type_opts, required: true)
optional_sub = cmake.subproject('optional', options: build_type_opts, required: true)
expected_sub = cmake.subproject('expected', options: build_type_opts, required: true)

local_deps = [
  dependency('freetype2', required: true),
  dependency('glfw3', required: true),
  dependency('fmt', required: true),
  dependency('glm', required: true),

  dependency('range-v3', required: true),

  libassert_sub.dependency('assert'),
  optional_sub.dependency('optional'),
  expected_sub.dependency('expected'),
]

compilation_options = [
  'cpp_std=c++2b'
]

include_dirs = [
  include_directories('include')
]

sources = files(
  'src/glad.c',

  'src/main.cpp',

  'src/commandline.cpp',
  'src/delta_time.cpp',
  'src/callbacks.cpp',
  'src/stb_image.cpp',
  'src/billboard.cpp',
  'src/trujkont.cpp',
  'src/texture.cpp',
  'src/camera.cpp',
  'src/quad.cpp'
)

executable(
  'trujkont',
  sources,
  dependencies: local_deps,
  include_directories: include_dirs,
  override_options: compilation_options,
)
