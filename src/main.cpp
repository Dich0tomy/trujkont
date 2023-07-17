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
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "stb_image.h"

auto enable_debug_info()
{
  glfwSetErrorCallback(callbacks::glfw_error_callback);

  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(callbacks::gl_error_callback, nullptr);
}

auto const vertex_shader_source = R"glsl(
#version 450 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 texture_coords;

layout (location = 3) out vec3 out_color;
layout (location = 4) out vec2 out_texture_coords;

void main()
{
  gl_Position = vec4(pos, 1.0);
  out_texture_coords = texture_coords;
  out_color = color;
}
)glsl";

auto const frag_shader_source = R"glsl(
  #version 450 core

  layout(location = 3) in vec3 color;
  layout(location = 4) in vec2 texture_coords;

  out vec4 frag_color;

  uniform sampler2D container_texture;
  uniform sampler2D face_texture;

  void main()
  {
    frag_color = mix(texture(container_texture, texture_coords), texture(face_texture, texture_coords), 0.5) * vec4(color, 1.0f);
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
  auto constexpr window_height = 800;

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

  // clang-format off
  auto const attrs_per_vertex = 8;

  auto const vertices = std::vector {
    0.5f,  0.5f, 0.0f,    1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // NOTE: top right
    0.5f, -0.5f, 0.0f,    0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
    -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
    -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f, // bottom right
  };
  // clang-format on

  auto triangleVAO = 0u;
  glGenVertexArrays(1, &triangleVAO);

  auto triangleVBO = 0u;
  glGenBuffers(1, &triangleVBO);
  glBindBuffer(GL_ARRAY_BUFFER, triangleVBO);

  glBindVertexArray(triangleVAO);

  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, attrs_per_vertex * sizeof(float), reinterpret_cast<void*>(0));
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, attrs_per_vertex * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, attrs_per_vertex * sizeof(float), reinterpret_cast<void*>(6 * sizeof(float)));
  glEnableVertexAttribArray(2);

  // clang-format off
  auto const indices = std::array {
    0, 1, 3,
    1, 2, 3
  };
  // clang-format on

  auto EBO = 0u;
  glGenBuffers(1, &EBO);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), indices.data(), GL_STATIC_DRAW);

  stbi_set_flip_vertically_on_load(true);
  {
    auto width = 0;
    auto height = 0;
    auto nr_of_channels = 0;
    auto const data = stbi_load("assets/container.jpg", &width, &height, &nr_of_channels, 0);

    if(not data) {
      fmt::print(stderr, "Whoops cannot load texture.");
      return -2;
    }

    auto texture = 0u;
    glGenTextures(1, &texture);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
  }

  {
    auto width = 0;
    auto height = 0;
    auto nr_of_channels = 0;

    auto const data = stbi_load("assets/awesomeface.png", &width, &height, &nr_of_channels, 0);

    if(not data) {
      fmt::print(stderr, "Whoops cannot load texture.");
      return -2;
    }

    auto texture = 0u;
    glGenTextures(1, &texture);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
  }

  shader_program.set_uniform_1i("container_texture", 0);
  shader_program.set_uniform_1i("face_texture", 1);

  while(not glfwWindowShouldClose(window)) {
    glClearColor(0.3f, 0.5f, 0.6f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindVertexArray(triangleVAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
}
