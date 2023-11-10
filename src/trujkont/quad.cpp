#include "trujkont/quad.hpp"

Quad::Quad()
{
  VAO = 0u;
  glGenVertexArrays(1, &VAO);

  auto VBO = 0u;
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  glBindVertexArray(VAO);

  glBufferData(GL_ARRAY_BUFFER, Quad::vertices.size() * sizeof(float), Quad::vertices.data(), GL_STATIC_DRAW);

  glVertexAttribPointer(
    0,
    3,
    GL_FLOAT,
    GL_FALSE,
    Quad::attrs_per_vertex * sizeof(float),
    reinterpret_cast<void*>(0)
  );
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(
    1,
    2,
    GL_FLOAT,
    GL_FALSE,
    Quad::attrs_per_vertex * sizeof(float),
    reinterpret_cast<void*>(3 * sizeof(float))
  );
  glEnableVertexAttribArray(1);

  auto EBO = 0u;
  glGenBuffers(1, &EBO);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), indices.data(), GL_STATIC_DRAW);
}

auto Quad::update() -> void
{
  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}
