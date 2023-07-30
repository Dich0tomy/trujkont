#pragma once

#include <filesystem>

#include <glad/glad.h>

enum class TextureFormat : GLuint
{
  RGB = GL_RGB,
  RGBA = GL_RGBA,
};

struct TextureBasicInfo
{
  TextureFormat format = TextureFormat::RGB;

  int width = 0;
  int height = 0;
  int channels_number = 0;

  unsigned slot = 0;
  GLuint id = 0;
};

class Texture
{
public:
  Texture(TextureFormat const format = TextureFormat::RGB) noexcept;
  Texture(std::filesystem::path const& texture_path, TextureFormat const format = TextureFormat::RGB);

  auto get_slot() const noexcept -> unsigned;

private:
  TextureBasicInfo basic_info;
};
