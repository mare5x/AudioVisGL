#version 330 core
layout (location = 0) in vec2 quad_pos;

uniform float scale;

void main()
{
	gl_Position = vec4(quad_pos * scale, 0.0, 1.0);
}