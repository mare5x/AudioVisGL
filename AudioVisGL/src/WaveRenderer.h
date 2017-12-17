#pragma once
#include "Shaders\Shader.h"
#include "glad\glad.h"


class WaveRenderer {
public:
	WaveRenderer(int wavedata_n) : wavedata_size(wavedata_n) { }
	~WaveRenderer();

	void init();

	void render();

	void set_wavedata(float wavedata[]);

	void set_volume(float volume);
private:
	const int wavedata_size;

	GLuint quad_vao, quad_vbo;
	Shader quad_shader;

	GLuint wavedata_vao, point_vbo, wavedata_vbo;
	Shader wavedata_shader;
};