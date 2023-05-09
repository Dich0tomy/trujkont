#include <cstdlib>

#include <fmt/format.h>

// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// clang-format on

auto main() -> int {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  glfwSetErrorCallback([](int error_code, char const *desc) {
    fmt::print(stderr, "{}\n", desc);

    std::exit(error_code);
  });

  auto constexpr window_width = 800;
  auto constexpr window_height = 600;

  fmt::print("[[[[ Checkpoint 1. ]]]]\n");
  auto *const window = glfwCreateWindow(window_width, window_height,
                                        "Szopu jest gejem", nullptr, nullptr);

  if (not window) {
    fmt::print(stderr, "Failed to initialize OpenGL window :(\n");
    glfwTerminate();

    return -1;
  }

  fmt::print("[[[[ Checkpoint 2. ]]]]\n");
  glfwMakeContextCurrent(window);

  if (not gladLoadGLLoader(
          reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
    fmt::print(stderr, "Failed to initialize GLAD\n");

    return -1;
  }
}
