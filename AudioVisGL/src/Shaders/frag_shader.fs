#version 330 core

in vec2 tex_pos;

out vec4 frag_color;

uniform sampler2D texture;

void main()
{
	frag_color = vec4(0.5, 0.5, 0.5, 1);
}