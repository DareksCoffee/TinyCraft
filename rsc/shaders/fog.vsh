#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texcoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out VS_OUTPUT 
{
  vec2 texcoord;
  float fog_distance;
} vs_out;

void main()
{
  vec4 world_pos = model * vec4(position, 1.0);
  vec4 view_pos = view * world_pos;
  gl_Position = projection * view_pos;
  
  vs_out.texcoord = texcoord;
  vs_out.fog_distance = length(view_pos.xyz);
}
