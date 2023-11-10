#pragma once

#include <array>

#include "glad/glad.h"

class Quad
{
public:
  Quad();

  auto update() -> void;

private:
  GLuint VAO;

  auto inline static constexpr attrs_per_vertex = 5;

  // clang-format off
  inline static auto constexpr vertices = std::array {
    0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
    0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
    -0.5f,  -0.5f, 0.0f, 0.0f, 0.0f,
    -0.5f,  0.5f, 0.0f, 0.0f, 1.0f
  };

  inline static auto constexpr indices = std::array {
    0, 1, 3,
    1, 2, 3
  };
  // clang-format on
};
