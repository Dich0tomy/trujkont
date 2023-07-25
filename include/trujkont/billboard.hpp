#pragma once

#include <filesystem>
#include <array>

#include <glm/glm.hpp>

#include "trujkont/shader_program.hpp"
#include "trujkont/texture.hpp"

class Billboard
{
public:
  Billboard();

  Billboard(Texture const texture, glm::vec3 position);

  auto update(glm::vec3 const& camera_position) -> void;

  glm::vec3 position = glm::vec3(0.);

private:
  Texture texture;

  glm::mat4 model = glm::mat4(1.f);

  ShaderProgram billboard_shader_program;

  GLuint VAO;

  inline static auto constexpr attrs_per_vertex = 5;

  // clang-format off
  inline static auto constexpr vertices = std::array {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
    0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
    0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
  };


  inline static auto constexpr indices = std::array {
    0, 1, 3,
    1, 2, 3
  };
  // clang-format on
};
