#include <cstdlib>
#include <numbers>
#include <random>
#include <array>

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

auto const vertex_shader_source = R"glsl(
  #version 330 core
  layout (location = 0) in vec3 aPos;

  void main()
  {
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
  }
)glsl";

auto const frag_shader_source = R"glsl(
  #version 330 core
  out vec4 FragColor;

  void main() {
    FragColor = vec4(0.3f, 0.9f, 0.2f, 1.0f);
  }
)glsl";

auto main() -> int {
  if(not glfwInit()) {
    fmt::print(stderr, "GLFW init error\n");
    return -1;
  }

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

  if(not gladLoadGL()) {
    fmt::print(stderr, "Failed to initialize GLAD\n");
    return -1;
  }

  glfwSetFramebufferSizeCallback(window, callbacks::framebuffer_size_callback);

  // clang-format off
  auto const triangle_vertices = std::array {
    -0.5f, -0.5f, 0.0f,
    0.5f, -0.5f, 0.0f,
    0.0f, 0.5f, 0.0f
  };

  auto const vertices = std::array {
    0.5f,  0.5f, 0.0f,
    0.5f, -0.5f, 0.0f,
    -0.5f, -0.5f, 0.0f,
    -0.5f,  0.5f, 0.0f
  };

  auto const indices = std::array {
    0, 1, 3,
    1, 2, 3
  };
  // clang-format on

  auto vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, &vertex_shader_source, nullptr);
  glCompileShader(vertex_shader);

  auto success = 0;
  glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
  if(not success) {
    std::array<char, 512> log;
    glGetShaderInfoLog(vertex_shader, log.size(), nullptr, log.data());
    fmt::print(stderr, "Vertex shader compilation failed! Log:\n\n{}\n", log.data());
  }

  auto fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &frag_shader_source, nullptr);
  glCompileShader(fragment_shader);

  success = 0;
  glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
  if(not success) {
    std::array<char, 512> log;
    glGetShaderInfoLog(fragment_shader, log.size(), nullptr, log.data());
    fmt::print(stderr, "Fragment shader compilation failed! Log:\n\n{}\n", log.data());
  }

  auto shader_program = glCreateProgram();
  glAttachShader(shader_program, vertex_shader);
  glAttachShader(shader_program, fragment_shader);
  glLinkProgram(shader_program);

  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);

  auto VBO = 0u;
  auto VAO = 0u;
  auto EBO = 0u;

  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(float), indices.data(), GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(0));
  glEnableVertexAttribArray(0);

  success = 0;
  glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
  if(not success) {
    std::array<char, 512> log;
    glGetProgramInfoLog(shader_program, log.size(), nullptr, log.data());
    fmt::print(stderr, "Fragment shader compilation failed! Log:\n\n{}\n", log.data());
  }

  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  while(not glfwWindowShouldClose(window)) {
    for(auto i = 0.0f; i < std::numbers::pi * 2; i += 0.1f) {
      glClearColor(0.2f, 0.3f, 0.5f, 1.f);
      glClear(GL_COLOR_BUFFER_BIT);

      glUseProgram(shader_program);
      glBindVertexArray(VAO);
      glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
      glBindVertexArray(0);

      glfwSwapBuffers(window);
      glfwPollEvents();
    }
  }

  glfwTerminate();
}
