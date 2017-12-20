#version 330 core
layout (location = 0) in vec2 bar_pos;
layout (location = 1) in float bar_height;

uniform float frequency_bands;

out float height;
out vec2 v_pos;

void main()
{
	v_pos = bar_pos;
	height = bar_height;
	float x_offset = gl_InstanceID / frequency_bands * 2.0f - 1.0f;
	gl_Position = vec4(bar_pos.x + x_offset, bar_pos.y * bar_height - 1.0, 0.0, 1.0);
}