#version 330 core

in float y_offset;

out vec4 frag;

void main() 
{
	frag = mix(vec4(1, 0, 0, 1), vec4(0, 0.3, 1, 1), abs(y_offset));
}