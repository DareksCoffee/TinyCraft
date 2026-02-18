#version 330 core

in VS_OUTPUT 
{
  vec2 texcoord;
} fs_in;

uniform sampler2D texture_sampler;

out vec4 fragment_color;

void main()
{
  fragment_color = texture(texture_sampler, fs_in.texcoord);
}
