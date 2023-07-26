#include <algorithm>
#include <cstdlib>
#include <numbers>
#include <random>
#include <chrono>
#include <array>

#include <trujkont/shader_program.hpp>
#include <trujkont/callbacks.hpp>
#include <trujkont/billboard.hpp>
#include <trujkont/texture.hpp>
#include <trujkont/shader.hpp>
#include <trujkont/camera.hpp>

#include <fmt/format.h>

// clang-format off
#include <GLFW/glfw3.h>
#include <glad/glad.h>

// clang-format on
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>

#include "stb_image.h"

/*
Local space (or Object space)
World space
View space (or Eye space)
Clip space
Screen space
*/

auto enable_debug_info()
{
  glfwSetErrorCallback(callbacks::glfw_error_callback);

  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(callbacks::gl_error_callback, nullptr);
}

auto const vertex_shader_source = R"glsl(
#version 450 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 texture_coords;

layout (location = 2) out vec2 out_texture_coords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
  gl_Position = projection * view * model * vec4(pos, 1.0);
  out_texture_coords = texture_coords;
}
)glsl";

auto const frag_shader_source = R"glsl(
  #version 450 core

  layout(location = 2) in vec2 texture_coords;

  out vec4 frag_color;

  uniform sampler2D face_texture;

  void main()
  {
    frag_color = texture(face_texture, texture_coords);
  }
)glsl";

auto main() -> int
{
  fmt::print("GLFW version: '{}'\n", glfwGetVersionString());

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
  auto const attrs_per_vertex = 5;

  auto const vertices = std::array {
      -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
      0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
      0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
      0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
      -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
      -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

      -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
      0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
      0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
      0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
      -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
      -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

      -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
      -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
      -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
      -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
      -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
      -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

      0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
      0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
      0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
      0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
      0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
      0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

      -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
      0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
      0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
      0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
      -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
      -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

      -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
      0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
      0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
      0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
      -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
      -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
  };

  /* auto const vertices = std::vector {
    0.5f,  0.5f, 0.0f,    1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // NOTE: top right
    0.5f, -0.5f, 0.0f,    0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
    -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
    -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f, // bottom right
  }; */
  // clang-format on

  stbi_set_flip_vertically_on_load(true);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  auto VAO = 0u;
  glGenVertexArrays(1, &VAO);

  auto VBO = 0u;
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  glBindVertexArray(VAO);

  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, attrs_per_vertex * sizeof(float), reinterpret_cast<void*>(0));
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, attrs_per_vertex * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  // glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, attrs_per_vertex * sizeof(float), reinterpret_cast<void*>(6 * sizeof(float)));
  // glEnableVertexAttribArray(2);

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

  auto babushka_texture = Texture("assets/awesomeface.png", TextureFormat::RGBA);
  shader_program.set_uniform_1i("face_texture", babushka_texture.get_slot());
  // shader_program.set_uniform_1i("face_texture", 1);

  // fmt::print("X: {} Y: {} Z: {}", vec.x, vec.y, vec.z);

  using Clock = std::chrono::steady_clock;
  auto delta_time = Clock::duration::rep();
  auto last_frame_time = Clock::time_point();

  auto device = std::random_device();
  auto generator = std::mt19937();
  auto distribution = std::uniform_int_distribution(0, 1);

  auto const cube_positions = std::array {
    glm::vec3(1.0f, 3.0f, -5.5f),
    glm::vec3(2.0f, 5.0f, -15.0f),
    glm::vec3(-1.5f, -2.2f, -2.5f),
    glm::vec3(-3.8f, -2.0f, -12.3f),
    glm::vec3(2.4f, -0.4f, -3.5f),
    glm::vec3(-1.7f, 3.0f, -7.5f),
    glm::vec3(1.3f, -2.0f, -2.5f),
    glm::vec3(1.5f, 2.0f, -2.5f),
    glm::vec3(1.5f, 0.2f, -1.5f),
    glm::vec3(-1.3f, 1.0f, -1.5f)
  };

  auto camera = Camera(window);

  auto const awesomeface_texture = Texture("assets/babushka.png", TextureFormat::RGB);
  auto face_billboard = Billboard(awesomeface_texture, glm::vec3(0.));

  while(not glfwWindowShouldClose(window)) {
    shader_program.use();

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindVertexArray(VAO);
    // glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

    auto cubes = 10;
    for(auto i = 1; i <= cubes; ++i) {
      auto model = glm::mat4(1.0f);
      model = glm::translate(model, cube_positions[i - 1]);

      model = glm::rotate(model, i * (float)glfwGetTime() * glm::radians(25.0f), glm::vec3(0.5f, 1.0f, 0.0f));

      shader_program.set_uniform_4mat("model", model);

      glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    auto const now = Clock::now();
    delta_time = std::chrono::duration_cast<std::chrono::seconds>(now - last_frame_time).count();

    auto const [view, projection] = camera.update(delta_time, static_cast<float>(window_width) / window_height);

    face_billboard.update(camera.position, view, projection);

    shader_program.use();
    shader_program.set_uniform_4mat("view", view);
    shader_program.set_uniform_4mat("projection", projection);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
}
