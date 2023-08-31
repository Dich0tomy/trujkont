#include "trujkont/texture.hpp"

#include <fmt/format.h>

#include <glad/glad.h>

#include "stb_image.h"

Texture::Texture(TextureFormat const format) noexcept
{
  basic_info.format = format;

  auto static current_slot_nr = 0;
  basic_info.slot = current_slot_nr;

  current_slot_nr++;
}

Texture::Texture(std::filesystem::path const& texture_path, TextureFormat const format)
  : Texture(format)
{
  auto const data = stbi_load(
    texture_path.c_str(),
    &basic_info.width,
    &basic_info.height,
    &basic_info.channels_number,
    0
  );

  if(not data) {
    throw std::runtime_error(
      fmt::format("Cannot find texture @ \"{}\".", texture_path.c_str())
    );
  }

  glGenTextures(1, &basic_info.id);

  glActiveTexture(GL_TEXTURE0 + basic_info.slot);
  glBindTexture(GL_TEXTURE_2D, basic_info.id);

  auto const gl_format = static_cast<GLuint>(format);
  glTexImage2D(GL_TEXTURE_2D, 0, gl_format, basic_info.width, basic_info.height, 0, gl_format, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);

  stbi_image_free(data);
}

auto Texture::get_slot() const noexcept -> TextureSlot
{
  return basic_info.slot;
}
