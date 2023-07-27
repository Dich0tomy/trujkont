#pragma once

#include <filesystem>
#include <array>

#include <glm/glm.hpp>

#include "trujkont/shader_program.hpp"
#include "trujkont/texture.hpp"
#include "trujkont/quad.hpp"

class Billboard : Quad
{
public:
  Billboard();

  Billboard(Texture const texture, glm::vec3 position);

  auto update(glm::vec3 const& camera_position, glm::mat4 const& camera_view, glm::mat4 const& camera_projection) -> void;

  glm::vec3 position = glm::vec3(0.);

  glm::mat4 model = glm::mat4(1.0);

private:
  Texture texture;

  ShaderProgram billboard_shader_program;

  GLuint VAO;

  inline static auto constexpr attrs_per_vertex = 5;
};
