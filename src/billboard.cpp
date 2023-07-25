#include <fstream>

#include <fmt/format.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

#include "trujkont/shader_program.hpp"
#include "trujkont/billboard.hpp"
#include "trujkont/shader.hpp"
#include "trujkont/texture.hpp"

namespace
{

auto read_shader_source(std::filesystem::path const path)
{
  if(not std::filesystem::exists(path)) {
    throw std::runtime_error(
      fmt::format("Cannot find shader @ path: \"{}\"", path.c_str())
    );
  }

  auto file = std::ifstream(path.c_str());

  auto source = std::string();
  source.reserve(100);

  auto line = std::string();
  while(std::getline(file, line)) {
    source.append(line);
    source.append("\n");
  }

  return source;
}

} // namespace

Billboard::Billboard()
  : Billboard(Texture(), glm::vec3(0.))
{}

Billboard::Billboard(Texture const texture, glm::vec3 position)
  : position(position),
    texture(texture)
{
  auto billboard_vertex_shader_source = read_shader_source("src/shaders/billboard.vert");
  auto const vertex_shader = Shader(ShaderType::Vertex, billboard_vertex_shader_source);
  if(vertex_shader.param<ShaderAttr::CompileStatus>() != GL_TRUE) {
    fmt::print(stderr, "Vertex shader compilation failed! Log:\n\n{}\n", vertex_shader.log());
    throw std::runtime_error("Cannot compile billboard vertex shader!");
  }

  auto billboard_fragment_shader_source = read_shader_source("src/shaders/billboard.frag");
  auto const frag_shader = Shader(ShaderType::Fragment, billboard_fragment_shader_source);
  if(frag_shader.param<ShaderAttr::CompileStatus>() != GL_TRUE) {
    fmt::print(stderr, "Fragment shader compilation failed! Log:\n\n{}\n", frag_shader.log());
    throw std::runtime_error("Cannot compile billboard fragment shader!");
  }

  billboard_shader_program = ShaderProgram(vertex_shader, frag_shader);

  if(billboard_shader_program.param<ProgramAttr::LinkStatus>() != GL_TRUE) {
    fmt::print(stderr, "Shader program linking failed! Log:\n\n{}\n", billboard_shader_program.log());
    throw std::runtime_error("Cannot compile billboard shader program!");
  }

  billboard_shader_program.use();
  billboard_shader_program.set_uniform_1i("billboard_texture", texture.get_slot());

  VAO = 0u;
  glGenVertexArrays(1, &VAO);

  auto VBO = 0u;
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  glBindVertexArray(VAO);

  glBufferData(GL_ARRAY_BUFFER, Billboard::vertices.size() * sizeof(float), Billboard::vertices.data(), GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, Billboard::attrs_per_vertex * sizeof(float), reinterpret_cast<void*>(0));
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, Billboard::attrs_per_vertex * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  auto EBO = 0u;
  glGenBuffers(1, &EBO);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), indices.data(), GL_STATIC_DRAW);
}

auto Billboard::update(glm::vec3 const& camera_position) -> void
{
  auto direction_from_camera = position - camera_position;

  if(direction_from_camera.length() <= 0) return;

  auto billboard_camera_angle = glm::atan(direction_from_camera.y, direction_from_camera.x);

  auto distance_2d = glm::sqrt(direction_from_camera.x * direction_from_camera.x + direction_from_camera.y + direction_from_camera.y);
  auto z_angle = glm::atan(direction_from_camera.z, distance_2d);

  model = glm::mat4(1.f);
  model = glm::translate(model, camera_position);
  model = model * glm::eulerAngleXYZ(0.f, z_angle, billboard_camera_angle);

  billboard_shader_program.use();
  billboard_shader_program.set_uniform_4mat("model", model);

  glBindVertexArray(VAO);
  // glDrawArrays(GL_TRIANGLES, 0, vertices.size());
  glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}
