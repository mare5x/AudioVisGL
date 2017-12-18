#pragma once
#include "Shaders\Shader.h"
#include "glad\glad.h"


class WaveRenderer {
public:
	WaveRenderer(int wavedata_n) : wavedata_size(wavedata_n), render_volume(true), render_wave(true) { }
	~WaveRenderer();

	void init();

	void render();

	void set_wavedata(float wavedata[]);

	void set_volume(float volume);

	void toggle_render_wave() { render_wave = !render_wave; }
	void toggle_render_volume() { render_volume = !render_volume; }

	void set_render_wave(bool val) { render_wave = val; }
	void set_render_volume(bool val) { render_volume = val; }
private:
	const int wavedata_size;

	bool render_volume, render_wave;

	GLuint quad_vao, quad_vbo;
	Shader quad_shader;

	GLuint wavedata_vao, point_vbo, wavedata_vbo;
	Shader wavedata_shader;
};