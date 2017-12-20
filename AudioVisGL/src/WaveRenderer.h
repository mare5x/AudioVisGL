#pragma once
#include "Shaders\Shader.h"
#include "glad\glad.h"


class WaveRenderer {
public:
	WaveRenderer(int _wavedata, int _frequency_bands) : 
		wavedata_size(_wavedata), frequency_bands(_frequency_bands),
		render_volume(true), render_wave(true), render_frequency_bands(true) { }
	~WaveRenderer();

	void init();

	void render();

	void on_resize(int w, int h);

	void set_wavedata(float wavedata[]);
	void set_volume(float volume);
	void set_frequency_bands(float frequencies[]);

	void toggle_render_wave() { render_wave = !render_wave; }
	void toggle_render_volume() { render_volume = !render_volume; }
	void toggle_render_frequency_bands() { render_frequency_bands = !render_frequency_bands; }
private:
	const int wavedata_size, frequency_bands;

	bool render_volume, render_wave, render_frequency_bands;

	GLuint volume_quad_vao, volume_quad_vbo;
	Shader volume_quad_shader;

	GLuint wavedata_vao, point_vbo, wavedata_vbo;
	Shader wavedata_shader;

	GLuint frequencies_vao, frequencies_bar_vbo, frequencies_heights_vbo;
	Shader frequencies_shader;
};