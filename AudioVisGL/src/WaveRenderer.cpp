#include "WaveRenderer.h"

WaveRenderer::~WaveRenderer()
{
	glDeleteVertexArrays(1, &volume_quad_vao);
	glDeleteBuffers(1, &volume_quad_vbo);
}

void WaveRenderer::init()
{
	float quad[] = {
		-0.5f, -0.5f,
		-0.5f, 0.5f,
		0.5f, -0.5f,
		0.5f, 0.5f
	};

	glGenBuffers(1, &volume_quad_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, volume_quad_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);

	glGenVertexArrays(1, &volume_quad_vao);
	glBindVertexArray(volume_quad_vao);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void *)(0));

	volume_quad_shader = Shader("src/Shaders/quad.vs", "src/Shaders/quad.fs");
	volume_quad_shader.use();
	volume_quad_shader.setFloat("scale", 1);

	float point[] = {
		0.0f, 0.0f
	};

	glGenBuffers(1, &point_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, point_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(point), point, GL_STATIC_DRAW);

	glGenVertexArrays(1, &wavedata_vao);
	glBindVertexArray(wavedata_vao);
	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void *)(0));
	
	glGenBuffers(1, &wavedata_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, wavedata_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * wavedata_size, NULL, GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void *)(0));

	glVertexAttribDivisor(0, 0);
	glVertexAttribDivisor(1, 1);

	glPointSize(3);

	wavedata_shader = Shader("src/Shaders/wavedata.vs", "src/Shaders/wavedata.fs");
	wavedata_shader.use();
	wavedata_shader.setFloat("wavedata_size", wavedata_size);

	float freq_bar_width = 2.0f / frequency_bands;
	float freq_bar_quad[] = {
		0.0f, 0.0f,
		0.0f, 1.0f,
		freq_bar_width, 0.0f,
		freq_bar_width, 1.0f
	};

	glGenBuffers(1, &frequencies_bar_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, frequencies_bar_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(freq_bar_quad), freq_bar_quad, GL_STATIC_DRAW);

	glGenVertexArrays(1, &frequencies_vao);
	glBindVertexArray(frequencies_vao);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void *)(0));

	glGenBuffers(1, &frequencies_heights_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, frequencies_heights_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * frequency_bands, NULL, GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void *)(0));

	glVertexAttribDivisor(0, 0);
	glVertexAttribDivisor(1, 1);

	frequencies_shader = Shader("src/Shaders/freq.vs", "src/Shaders/freq.fs");
	frequencies_shader.use();
	frequencies_shader.setFloat("frequency_bands", frequency_bands);
}

void WaveRenderer::render()
{
	if (render_volume) {
		volume_quad_shader.use();
		glBindVertexArray(volume_quad_vao);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
	}
	if (render_wave) {
		wavedata_shader.use();
		glBindVertexArray(wavedata_vao);
		glDrawArraysInstanced(GL_POINTS, 0, 1, wavedata_size);
		glBindVertexArray(0);
	}
	if (render_frequency_bands) {
		frequencies_shader.use();
		glBindVertexArray(frequencies_vao);
		glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, frequency_bands);
		glBindVertexArray(0);
	}
}

void WaveRenderer::set_wavedata(float wavedata[])
{
	glBindBuffer(GL_ARRAY_BUFFER, wavedata_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * wavedata_size, wavedata, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void WaveRenderer::set_volume(float volume)
{
	volume_quad_shader.use();
	volume_quad_shader.setFloat("scale", volume);
}

void WaveRenderer::set_frequency_bands(float frequencies[])
{
	glBindBuffer(GL_ARRAY_BUFFER, frequencies_heights_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * frequency_bands, frequencies, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
