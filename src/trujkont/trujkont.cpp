#include "trujkont/trujkont.hpp"

#include <algorithm>
#include <cstdlib>
#include <numbers>
#include <random>
#include <chrono>
#include <array>

#include <trujkont/shader_program/shader_program.hpp>
#include <trujkont/commandline/commandline.hpp>
#include <trujkont/delta_time/delta_time.hpp>
#include <trujkont/shader_program/shader.hpp>
#include <trujkont/callbacks/callbacks.hpp>
#include <trujkont/billboard/billboard.hpp>
#include <trujkont/texture/texture.hpp>
#include <trujkont/camera/camera.hpp>

#include <fmt/format.h>

// clang-format off
#include <GLFW/glfw3.h>
#include <glad/glad.h>

// clang-format on
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>

#include "stb/stb_image.h"

auto enable_debug_info()
{
  glfwSetErrorCallback(callbacks::glfw_error_callback);

  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(callbacks::gl_error_callback, nullptr);
}

namespace
{

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

} // namespace

auto Trujkont::run() -> int
{
  fmt::print("GLFW version: '{}'\n", glfwGetVersionString());

  if(glfwInit() == 0) {
    fmt::print(stderr, "GLFW init error\n");
    return -1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  auto constexpr window_width = 800;
  auto constexpr window_height = 800;

  auto* const window = glfwCreateWindow(window_width, window_height, "Creatix to kot", nullptr, nullptr);

  if(not window) {
    fmt::print(stderr, "Failed to initialize OpenGL window :(\n");
    glfwTerminate();

    return -1;
  }

  glfwSetWindowAttrib(window, GLFW_DECORATED, GLFW_FALSE);

  glfwMakeContextCurrent(window);

  if(gladLoadGL() == 0) {
    fmt::print(stderr, "Failed to initialize GLAD\n");

    return -1;
  }

  enable_debug_info();

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
      -0.5F, -0.5F, -0.5F,  0.0F, 0.0F,
      0.5F, -0.5F, -0.5F,  1.0F, 0.0F,
      0.5F,  0.5F, -0.5F,  1.0F, 1.0F,
      0.5F,  0.5F, -0.5F,  1.0F, 1.0F,
      -0.5F,  0.5F, -0.5F,  0.0F, 1.0F,
      -0.5F, -0.5F, -0.5F,  0.0F, 0.0F,

      -0.5F, -0.5F,  0.5F,  0.0F, 0.0F,
      0.5F, -0.5F,  0.5F,  1.0F, 0.0F,
      0.5F,  0.5F,  0.5F,  1.0F, 1.0F,
      0.5F,  0.5F,  0.5F,  1.0F, 1.0F,
      -0.5F,  0.5F,  0.5F,  0.0F, 1.0F,
      -0.5F, -0.5F,  0.5F,  0.0F, 0.0F,

      -0.5F,  0.5F,  0.5F,  1.0F, 0.0F,
      -0.5F,  0.5F, -0.5F,  1.0F, 1.0F,
      -0.5F, -0.5F, -0.5F,  0.0F, 1.0F,
      -0.5F, -0.5F, -0.5F,  0.0F, 1.0F,
      -0.5F, -0.5F,  0.5F,  0.0F, 0.0F,
      -0.5F,  0.5F,  0.5F,  1.0F, 0.0F,

      0.5F,  0.5F,  0.5F,  1.0F, 0.0F,
      0.5F,  0.5F, -0.5F,  1.0F, 1.0F,
      0.5F, -0.5F, -0.5F,  0.0F, 1.0F,
      0.5F, -0.5F, -0.5F,  0.0F, 1.0F,
      0.5F, -0.5F,  0.5F,  0.0F, 0.0F,
      0.5F,  0.5F,  0.5F,  1.0F, 0.0F,

      -0.5F, -0.5F, -0.5F,  0.0F, 1.0F,
      0.5F, -0.5F, -0.5F,  1.0F, 1.0F,
      0.5F, -0.5F,  0.5F,  1.0F, 0.0F,
      0.5F, -0.5F,  0.5F,  1.0F, 0.0F,
      -0.5F, -0.5F,  0.5F,  0.0F, 0.0F,
      -0.5F, -0.5F, -0.5F,  0.0F, 1.0F,

      -0.5F,  0.5F, -0.5F,  0.0F, 1.0F,
      0.5F,  0.5F, -0.5F,  1.0F, 1.0F,
      0.5F,  0.5F,  0.5F,  1.0F, 0.0F,
      0.5F,  0.5F,  0.5F,  1.0F, 0.0F,
      -0.5F,  0.5F,  0.5F,  0.0F, 0.0F,
      -0.5F,  0.5F, -0.5F,  0.0F, 1.0F
  };

  // clang-format on

  stbi_set_flip_vertically_on_load(static_cast<int>(true));
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  auto VAO = 0U;
  glGenVertexArrays(1, &VAO);

  auto VBO = 0U;
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  glBindVertexArray(VAO);

  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

  // NOLINTNEXTLINE
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, attrs_per_vertex * sizeof(float), static_cast<void*>(0));
  glEnableVertexAttribArray(0);

  // NOLINTNEXTLINE
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, attrs_per_vertex * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  // clang-format off
  auto const indices = std::array {
    0, 1, 3,
    1, 2, 3
  };
  // clang-format on

  auto EBO = 0U;
  glGenBuffers(1, &EBO);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), indices.data(), GL_STATIC_DRAW);

  auto face_texture = Texture("assets/babushka.png", TextureFormat::RGB);
  shader_program.set_uniform_1ui("face_texture", face_texture.get_slot());

  auto delta_time = DeltaTime();

  auto const cube_positions = std::array {
    glm::vec3(1.0F, 3.0F, -5.5F),
    glm::vec3(2.0F, 5.0F, -15.0F),
    glm::vec3(-1.5F, -2.2F, -2.5F),
    glm::vec3(-3.8F, -2.0F, -12.3F),
    glm::vec3(2.4F, -0.4F, -3.5F),
    glm::vec3(-1.7F, 3.0F, -7.5F),
    glm::vec3(1.3F, -2.0F, -2.5F),
    glm::vec3(1.5F, 2.0F, -2.5F),
    glm::vec3(1.5F, 0.2F, -1.5F),
    glm::vec3(-1.3F, 1.0F, -1.5F)
  };

  auto camera = Camera(window);

  auto const billboard_texture = Texture("assets/awesomeface.png", TextureFormat::RGBA);
  auto face_billboard = Billboard(billboard_texture.get_slot(), glm::vec3(1.0, 1.0, -5.0));

  auto commandline = Commandline();

  commandline.add_command(
    "help",
    []([[maybe_unused]] Commandline::CommandArgs args) -> Commandline::CommandResult {
      return "Twoja stara zrogowaciala siadala na butli od vanisha";
    }
  );

  commandline.add_command(
    "exit",
    [&commandline]([[maybe_unused]] Commandline::CommandArgs args) -> Commandline::CommandResult {
      commandline.stop();
      std::exit(0);
    }
  );

  auto commandline_thread = std::jthread(&Commandline::run, commandline);

  while(glfwWindowShouldClose(window) == 0) {
    shader_program.use();

    glClearColor(0.1F, 0.1F, 0.1F, 1.0F);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindVertexArray(VAO);

    auto constexpr cubes = 10;
    for(auto i = 1; i <= cubes; ++i) {
      auto model = glm::mat4(1.0F);
      model = glm::translate(model, cube_positions[i - 1]);

      model = glm::rotate(model, static_cast<float>(i) * (float)glfwGetTime() * glm::radians(25.0F), glm::vec3(0.5F, 1.0F, 0.0F));

      shader_program.set_uniform_4mat("model", model);

      glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    auto const [view, projection] = camera.update(delta_time.get(), static_cast<float>(window_width) / window_height);

    face_billboard.update(view, projection);

    shader_program.use();
    shader_program.set_uniform_4mat("view", view);
    shader_program.set_uniform_4mat("projection", projection);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
