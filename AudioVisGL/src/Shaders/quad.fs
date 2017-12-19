#version 330 core

uniform float scale;

in vec2 v_pos;

out vec4 frag_color;

void main()
{
	frag_color = vec4(scale / 4, scale, scale, 1);
}