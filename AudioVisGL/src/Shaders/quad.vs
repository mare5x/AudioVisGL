#version 330 core
layout (location = 0) in vec2 quad_pos;

uniform float scale;

out vec2 v_pos;

void main()
{
	v_pos = quad_pos;
	gl_Position = vec4(quad_pos * scale, 0.0, 1.0);
}