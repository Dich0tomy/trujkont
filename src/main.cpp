#include <cstdlib>
#include <numbers>
#include <random>

#include <trujkont/callbacks.hpp>

#include <fmt/format.h>

// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// clang-format on

auto enable_debug_info() {
  glfwSetErrorCallback(callbacks::glfw_error_callback);

  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(callbacks::gl_error_callback, nullptr);
}

auto main() -> int {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  auto constexpr window_width = 800;
  auto constexpr window_height = 600;

  auto* const window = glfwCreateWindow(window_width, window_height, "Szopu jest gejem", nullptr, nullptr);

  if(not window) {
    fmt::print(stderr, "Failed to initialize OpenGL window :(\n");
    glfwTerminate();

    return -1;
  }

  glfwMakeContextCurrent(window);

  if(not gladLoadGLLoader(
       reinterpret_cast<GLADloadproc>(glfwGetProcAddress)
     )) {
    fmt::print(stderr, "Failed to initialize GLAD\n");

    return -1;
  }

  glfwSetFramebufferSizeCallback(window, callbacks::framebuffer_size_callback);

  while(not glfwWindowShouldClose(window)) {
    for(auto i = 0.0f; i < std::numbers::pi * 2; i += 0.1f) {
      glClearColor(
        std::abs(std::sin(i)),
        std::abs(std::sin(i)),
        std::abs(std::sin(i)),
        1.0f
      );

      glClear(GL_COLOR_BUFFER_BIT);
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
}
