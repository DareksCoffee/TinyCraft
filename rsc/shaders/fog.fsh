#version 330 core

in VS_OUTPUT 
{
  vec2 texcoord;
  float fog_distance;
} fs_in;

uniform sampler2D texture_sampler;
uniform float fog_start = 5.0;
uniform float fog_end = 100.0;
uniform vec3 fog_color = vec3(0.55294, 0.74510, 1.0);

out vec4 fragment_color;

void main()
{
  vec4 tex_color = texture(texture_sampler, fs_in.texcoord);
  
  float fog_factor = (fog_end - fs_in.fog_distance) / (fog_end - fog_start);
  fog_factor = clamp(fog_factor, 0.0, 1.0);
  
  vec3 final_color = mix(fog_color, tex_color.rgb, fog_factor);
  fragment_color = vec4(final_color, tex_color.a);
}
