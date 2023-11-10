#version 450 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texture_coords;

layout (location = 2) out vec2 out_texture_coords;

uniform mat4 projection;
uniform mat4 no_rotation_model_view_mat;

void main()
{
  gl_Position = projection * no_rotation_model_view_mat * vec4(position, 1.0f);
  out_texture_coords = texture_coords;
}

// vim: ft=glsl
