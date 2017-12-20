#version 330 core

in float height;
in vec2 v_pos;

out vec4 frag;

void main()
{
	frag = max(0.25, v_pos.y) * max(0.5, height) * vec4(1, 0.2, 0.5, 1);
}