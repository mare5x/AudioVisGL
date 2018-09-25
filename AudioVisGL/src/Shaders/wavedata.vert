#version 330 core
layout (location = 0) in float offset;  // y offset
layout (location = 1) in float idx;

uniform float wavedata_size;

out float y_offset;

void main()
{
	y_offset = offset;
	float x_offset = idx / wavedata_size * 2.0f - 1.0f;
	gl_Position = vec4(x_offset, y_offset, 0.0, 1.0);
}