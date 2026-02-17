#version 330 core

in VS_OUTPUT 
{
  vec3 color;
} fs_in;

out vec4 fragment_color;

void main()
{
  fragment_color = vec4(fs_in.color, 1.0);
}
