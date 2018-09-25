#version 330 core

in float height;
in vec2 v_pos;

out vec4 frag;

void main()
{
	float y = v_pos.y * height;
	frag = max(0.2, y) * vec4(1, 0.2, 0.5, 1);
}