#pragma once

#include <string_view>
#include <string>

#include <glad/glad.h>

enum class ShaderType
{
  Compute = GL_COMPUTE_SHADER,
  Vertex = GL_VERTEX_SHADER,
  TessControl = GL_TESS_CONTROL_SHADER,
  TessEvaluation = GL_TESS_EVALUATION_SHADER,
  Geometry = GL_GEOMETRY_SHADER,
  Fragment = GL_FRAGMENT_SHADER
};

enum class ShaderAttr
{
  Type = GL_SHADER_TYPE,
  DeleteStatus = GL_DELETE_STATUS,
  CompileStatus = GL_COMPILE_STATUS,
  InfoLogLength = GL_INFO_LOG_LENGTH,
  ShaderSourceLength = GL_SHADER_SOURCE_LENGTH
};

class Shader
{
public:
  Shader(ShaderType shader_type, std::string_view const shader_source)
    : id(glCreateShader(static_cast<GLenum>(shader_type)))
  {
    auto const source_ptr = shader_source.data();

    glShaderSource(id, 1, &source_ptr, nullptr);
    glCompileShader(id);
  }

  template<ShaderAttr ShaderAttr>
  [[nodiscard]] auto param() const -> GLuint
  {
    auto param = 0;
    glGetShaderiv(id, static_cast<GLenum>(ShaderAttr), &param);

    return param;
  }

  [[nodiscard]] auto log() const -> std::string
  {
    auto const log_size = param<ShaderAttr::InfoLogLength>();
    if(log_size == 0) {
      return {};
    }

    auto log = std::string();
    log.resize(log_size);

    glGetShaderInfoLog(id, log_size, nullptr, log.data());

    return log;
  }

  GLuint id;
};
