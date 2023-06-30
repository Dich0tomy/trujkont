#pragma once

#include <string_view>
#include <concepts>
#include <array>

#include "trujkont/shader.hpp"

#include <glad/glad.h>

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

  auto use() const
  {
    glUseProgram(id);
  }

  GLuint id;
};
