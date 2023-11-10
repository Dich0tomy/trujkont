#version 450 core

layout (location = 2) in vec2 texture_coords;

out vec4 out_frag_color;

uniform sampler2D billboard_texture;

void main()
{
  out_frag_color = texture(billboard_texture, texture_coords);
}

// vim: ft=glsl
