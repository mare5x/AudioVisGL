#version 330 core
layout (location = 0) in vec2 point_pos;  // (0, 0)
layout (location = 1) in float offset;  // y offset

uniform float wavedata_size;

void main()
{
	float x_offset = gl_InstanceID / wavedata_size * 2.0f - 1.0f;
	gl_Position = vec4(point_pos.x + x_offset, point_pos.y + offset, 0.0, 1.0);
}