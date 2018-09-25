#version 330 core

uniform float scale;

in vec2 v_pos;

out vec4 frag_color;

void main()
{
	float len = length(v_pos);
	if (len > 0.3 && len < 0.5)
		frag_color = vec4(scale / 2.0, scale / 8.0, scale / 2.0, 1);
}