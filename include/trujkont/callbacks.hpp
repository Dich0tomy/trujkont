#pragma once

#include <glad/glad.h>

struct GLFWwindow;

namespace callbacks {

auto gl_error_callback(
  GLenum source,
  GLenum type,
  GLuint id,
  GLenum severity,
  GLsizei length,
  GLchar const* message,
  void const* userParam
) -> void;

auto glfw_error_callback(int error_code, char const* desc) -> void;

auto framebuffer_size_callback(GLFWwindow* window, int width, int height) -> void;

} // namespace callbacks
