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

#include "stb_image.h"

auto enable_debug_info()
{
  glfwSetErrorCallback(callbacks::glfw_error_callback);

  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(callbacks::gl_error_callback, nullptr);
}

auto const vertex_shader_source = R"glsl(
  #version 450 core

  in vec2 position;
  in vec3 color;

  layout(location = 0) out vec3 out_color;

  void main()
  {
    gl_Position = vec4(position, 0.0, 1.0);
    out_color = color;
  }
)glsl";

auto const frag_shader_source = R"glsl(
  #version 450 core

  layout(location = 0) in vec3 color;

  out vec4 frag_color;

  void main()
  {
    frag_color = vec4(color, 1.0f);
  }
)glsl";

auto main() -> int
{
  fmt::print("GLFW version: '{}'", glfwGetVersionString());

  if(not glfwInit()) {
    fmt::print(stderr, "GLFW init error\n");
    return -1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

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
    return -1;
  }

  auto const frag_shader = Shader(ShaderType::Fragment, frag_shader_source);
  if(frag_shader.param<ShaderAttr::CompileStatus>() != GL_TRUE) {
    fmt::print(stderr, "Fragment shader compilation failed! Log:\n\n{}\n", frag_shader.log());
    return -1;
  }

  auto shader_program = ShaderProgram(vertex_shader, frag_shader);
  if(shader_program.param<ProgramAttr::LinkStatus>() != GL_TRUE) {
    fmt::print(stderr, "Shader program linking failed! Log:\n\n{}\n", shader_program.log());
    return -1;
  }

  shader_program.use();

  auto const attrs_per_vertex
    = 5;
  // clang-format off
  auto const triangle_vertices = std::vector {
    0.0f,  0.5f, 1.0f,  0.0f, 0.0f,
    0.5f, -0.5f, 0.0f,  1.0f, 0.0f,
    -0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
  };

  auto const rectangle_vertices = std::vector {
    -0.5f,  0.5f, 1.0f, // 0.0f, 0.0f, // Top-left
     0.5f,  0.5f, 0.0f, // 1.0f, 0.0f, // Top-right
     0.5f, -0.5f, 0.0f, // 0.0f, 1.0f, // Bottom-right
    -0.5f, -0.5f, 1.0f, // 1.0f, 1.0f  // Bottom-left
  };
  // clang-format on
  auto triangleVAO = 0u;
  glGenVertexArrays(1, &triangleVAO);

  auto rectangleVAO = 0u;
  glGenVertexArrays(1, &rectangleVAO);

  auto constexpr thing_to_draw = std::string_view("triangle");

  auto triangleVBO = 0u;
  glGenBuffers(1, &triangleVBO);

  auto rectangleVBO = 0u;
  glGenBuffers(1, &rectangleVBO);

  auto const& active_vertices = [thing_to_draw, triangleVBO, triangleVAO, &rectangle_vertices, rectangleVAO, rectangleVBO, &triangle_vertices] {
    if constexpr(thing_to_draw == "triangle") {
      glBindBuffer(GL_ARRAY_BUFFER, triangleVBO);
      glBindVertexArray(triangleVAO);
      return triangle_vertices;
    }

    glBindBuffer(GL_ARRAY_BUFFER, rectangleVBO);
    glBindVertexArray(rectangleVAO);
    return rectangle_vertices;
  }();

  glBufferData(GL_ARRAY_BUFFER, active_vertices.size() * sizeof(float), active_vertices.data(), GL_STATIC_DRAW);

  auto const position_loc = glGetAttribLocation(shader_program.id, "position");
  glVertexAttribPointer(position_loc, 2, GL_FLOAT, GL_FALSE, attrs_per_vertex * sizeof(float), 0);
  glEnableVertexAttribArray(position_loc);

  auto const color_loc = glGetAttribLocation(shader_program.id, "color");
  glVertexAttribPointer(color_loc, 3, GL_FLOAT, GL_FALSE, attrs_per_vertex * sizeof(float), reinterpret_cast<void*>(2 * sizeof(float)));
  glEnableVertexAttribArray(color_loc);

  auto const triangle_indices = std::array {
    0,
    1,
    2,
  };

  auto const rectangle_indices = std::array {
    0,
    1,
    2,
    2,
    3,
    0
  };

  auto const& active_indices = [thing_to_draw, &triangle_indices, &rectangle_indices] {
    if constexpr(thing_to_draw == "triangle")
      return triangle_indices;
    else
      return rectangle_indices;
  }();

  auto EBO = 0u;
  glGenBuffers(1, &EBO);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, active_indices.size() * sizeof(int), active_indices.data(), GL_STATIC_DRAW);

  while(not glfwWindowShouldClose(window)) {
    glClearColor(0.3f, 0.5f, 0.6f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glDrawElements(GL_TRIANGLES, active_indices.size(), GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
}
