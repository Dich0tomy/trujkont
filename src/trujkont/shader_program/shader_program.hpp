#pragma once

#include <string_view>
#include <concepts>
#include <array>

#include "trujkont/shader_program/shader.hpp"

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

enum class ProgramAttr
{
  DeleteStatus = GL_DELETE_STATUS,
  LinkStatus = GL_LINK_STATUS,
  ValidateStatus = GL_VALIDATE_STATUS,
  InfoLogLength = GL_INFO_LOG_LENGTH,
  AttachedShaders = GL_ATTACHED_SHADERS,
  ActiveAttributes = GL_ACTIVE_ATTRIBUTES,
  ActiveAttributesMaxLength = GL_ACTIVE_ATTRIBUTE_MAX_LENGTH,
  ActiveUniforms = GL_ACTIVE_UNIFORMS,
  ActiveUniformMaxLength = GL_ACTIVE_UNIFORM_MAX_LENGTH
};

class ShaderProgram
{
public:
  ShaderProgram()
    : id(glCreateProgram()) {};

  template<std::convertible_to<Shader>... Shaders>
  ShaderProgram(Shaders const... shaders)
    : ShaderProgram()
  {
    (glAttachShader(id, shaders.id), ...);

    glLinkProgram(id);

    (glDeleteShader(shaders.id), ...);
  }

  template<ProgramAttr ProgramAttr>
  [[nodiscard]] auto param() const -> GLuint
  {
    auto param = 0;
    glGetProgramiv(id, static_cast<GLenum>(ProgramAttr), &param);

    return param;
  }

  [[nodiscard]] auto log() const -> std::string
  {
    auto const log_size = param<ProgramAttr::InfoLogLength>();
    if(log_size == 0) {
      return {};
    }

    auto str = std::string();
    str.resize(log_size);

    glGetProgramInfoLog(id, log_size, nullptr, str.data());

    return str;
  }

  auto set_uniform_2f(std::string_view const name, float const first, float const second) const
  {
    auto const uniform_loc = glGetUniformLocation(id, name.data());

    use();
    glUniform2f(uniform_loc, first, second);
  }

  auto set_uniform_1i(std::string_view const name, int const value) const
  {
    auto const uniform_loc = glGetUniformLocation(id, name.data());

    use();
    glUniform1i(uniform_loc, value);
  }

  auto set_uniform_1ui(std::string_view const name, unsigned int const value) const
  {
    auto const uniform_loc = glGetUniformLocation(id, name.data());

    use();
    glUniform1ui(uniform_loc, value);
  }

  auto set_uniform_4mat(std::string_view const name, glm::mat4 mat) const
  {
    auto const uniform_loc = glGetUniformLocation(id, name.data());

    use();
    glUniformMatrix4fv(uniform_loc, 1, GL_FALSE, glm::value_ptr(mat));
  }

  auto use() const -> void
  {
    glUseProgram(id);
  }

  GLuint id;
};
