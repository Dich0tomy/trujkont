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
}

auto prev = glm::vec3(0.0);

auto Billboard::update(glm::vec3 const& camera_position, glm::mat4 const& camera_view, glm::mat4 const& camera_projection) -> void
{
  auto const dfc = position - camera_position;

  if(prev != dfc)
    fmt::print("x {:8}\ty {:8}\tz {:8}\n", dfc.x, dfc.y, dfc.z);

  prev = dfc;

  // the angle is in a 2d space with z and x coordinates
  auto const angle_2d_z_x = glm::atan(dfc.x, dfc.z);

  auto const d = glm::sqrt(dfc.z * dfc.z + dfc.x * dfc.x);
  auto angle_2d_y_z = glm::atan(dfc.y, d);

  model = glm::mat4(1.f);
  model = glm::translate(model, position);
  model = model
          * glm::eulerAngleXYZ(0.0f, angle_2d_z_x, 0.0f)
          * glm::eulerAngleXYZ(-angle_2d_y_z, 0.0f, 0.0f);

  billboard_shader_program.set_uniform_4mat("model", model);
  billboard_shader_program.set_uniform_4mat("view", camera_view);
  billboard_shader_program.set_uniform_4mat("projection", camera_projection);

  Quad::update();
}
