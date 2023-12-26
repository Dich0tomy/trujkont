#include <cstdlib>

#include "trujkont/callbacks/callbacks.hpp"

#include <fmt/format.h>
#include <fmt/color.h>

// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// clang-format on

namespace callbacks
{

auto glfw_error_callback(int error_code, char const* desc) -> void
{
  fmt::print(stderr, "GLFW error:\n\n{}\n", desc);

  std::exit(error_code);
}

auto gl_error_callback(
  [[maybe_unused]] GLenum source,
  GLenum type,
  [[maybe_unused]] GLuint id,
  GLenum severity,
  [[maybe_unused]] GLsizei length,
  GLchar const* message,
  [[maybe_unused]] void const* userParam
) -> void
{
  if(GL_DEBUG_SEVERITY_HIGH >= severity or severity >= GL_DEBUG_SEVERITY_LOW) return;

  fmt::print(
    stderr,
    fg(fmt::color::red),
    "GL error: [[\n\ttype: {}\n\tmessage: {}]]",
    (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
    message
  );
}

auto framebuffer_size_callback([[maybe_unused]] GLFWwindow* window, int width, int height) -> void
{
  glViewport(0, 0, width, height);
}

} // namespace callbacks
