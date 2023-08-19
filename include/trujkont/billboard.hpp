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

  Billboard(Texture texture, glm::vec3 position);

  auto update(glm::mat4 const& camera_view, glm::mat4 const& camera_projection) -> void;

private:
  glm::vec3 position = glm::vec3(0.);

  Texture texture;

  ShaderProgram billboard_shader_program;

  GLuint VAO = 0;

  auto inline static constexpr attrs_per_vertex = 5;
};
