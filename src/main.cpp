#include <cstdlib>
#include <numbers>
#include <random>
#include <array>

#include <trujkont/callbacks.hpp>
#include <trujkont/shader.hpp>
#include <trujkont/shader_program.hpp>

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

  in vec2 position;
  in vec3 color;

  out vec3 Color;

  void main()
  {
    Color = color;
    gl_Position = vec4(position, 0.0, 1.0);
  }
)glsl";

auto const frag_shader_source = R"glsl(
  #version 330 core

  in vec3 Color;

  out vec4 frag_color;

  void main() {
    frag_color = vec4(Color, 0.0f);
  }
)glsl";

auto main() -> int {
  fmt::print("GLFW version: '{}'", glfwGetVersionString());

  if(not glfwInit()) {
    fmt::print(stderr, "GLFW init error\n");
    return -1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);

  auto constexpr window_width = 800;
  auto constexpr window_height = 600;

  auto* const window = glfwCreateWindow(window_width, window_height, "Creatix to kot", nullptr, nullptr);

  glfwSetWindowAttrib(window, GLFW_DECORATED, GLFW_FALSE);

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

  auto const vertex_shader = Shader(ShaderType::Vertex, vertex_shader_source);
  if(vertex_shader.param<ShaderAttr::CompileStatus>() != GL_TRUE) {
    fmt::print(stderr, "Vertex shader compilation failed! Log:\n\n{}\n", vertex_shader.log());
  }

  auto const frag_shader = Shader(ShaderType::Fragment, frag_shader_source);
  if(frag_shader.param<ShaderAttr::CompileStatus>() != GL_TRUE) {
    fmt::print(stderr, "Fragment shader compilation failed! Log:\n\n{}\n", frag_shader.log());
  }

  auto const shader_program = ShaderProgram(vertex_shader, frag_shader);
  if(shader_program.param<ProgramAttr::LinkStatus>() != GL_TRUE) {
    fmt::print(stderr, "Shader program linking failed! Log:\n\n{}\n", shader_program.log());
  }
  shader_program.use();

  /*
  auto const indices = std::array {
    0, 1, 2,
  };

  auto EBO = 0u;
  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(float), indices.data(), GL_STATIC_DRAW); */

  auto VAO = 0u;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  auto const attrs_per_vertex = 5;
  // clang-format off
  auto const vertices = std::array {
    0.0f,  0.5f, 1.0f, 0.0f, 0.0f,
    0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
    -0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
  };
  // clang-format on

  auto VBO = 0u;
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

  auto const position_loc = glGetAttribLocation(shader_program.id, "position");
  glVertexAttribPointer(position_loc, 2, GL_FLOAT, GL_FALSE, attrs_per_vertex * sizeof(float), 0);
  glEnableVertexAttribArray(position_loc);

  auto const color_loc = glGetAttribLocation(shader_program.id, "color");
  glVertexAttribPointer(color_loc, 3, GL_FLOAT, GL_FALSE, attrs_per_vertex * sizeof(float), reinterpret_cast<void*>(2 * sizeof(float)));
  glEnableVertexAttribArray(color_loc);

  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  while(not glfwWindowShouldClose(window)) {
    // glUniform3f(triangle_color, (std::sin(i) + 1) / 2.0f, 0.0f, 0.0f);
    // glUniform3f(triangle_color, 1.0f, std::sin(i) + 1, std::cos(i));

    glClearColor(0.3f, 0.5f, 0.6f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glDrawArrays(GL_TRIANGLES, 0, 3);
    // glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
}
