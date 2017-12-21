#include "WaveRenderer.h"


const int INTERPOLATED_POINTS = 16;


WaveRenderer::WaveRenderer(int _wavedata, int _frequency_bands)
	: wavedata_size(_wavedata), frequency_bands(_frequency_bands),
	interpolated_wavedata_size(INTERPOLATED_POINTS * _wavedata - INTERPOLATED_POINTS + 1), interpolated_wavedata(interpolated_wavedata_size),
	render_volume(true), render_wave(true), render_frequency_bands(true)
{

}

WaveRenderer::~WaveRenderer()
{
	glDeleteVertexArrays(1, &volume_quad_vao);
	glDeleteVertexArrays(1, &wavedata_vao);
	glDeleteVertexArrays(1, &frequencies_vao);
	const GLuint vbos[] = { volume_quad_vbo, point_vbo, wavedata_vbo, frequencies_bar_vbo, frequencies_heights_vbo };
	glDeleteBuffers(5, vbos);
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

	glBindVertexArray(0);

	volume_quad_shader = Shader("Shaders/quad.vs", "Shaders/quad.fs");
	volume_quad_shader.use();
	volume_quad_shader.setFloat("scale", 1);
	volume_quad_shader.setFloat("aspect_ratio", 1);

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
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * interpolated_wavedata_size, NULL, GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void *)(0));

	glVertexAttribDivisor(0, 0);
	glVertexAttribDivisor(1, 1);

	glPointSize(2);

	glBindVertexArray(0);

	wavedata_shader = Shader("Shaders/wavedata.vs", "Shaders/wavedata.fs");
	wavedata_shader.use();
	wavedata_shader.setFloat("wavedata_size", interpolated_wavedata_size);

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

	glBindVertexArray(0);

	frequencies_shader = Shader("Shaders/freq.vs", "Shaders/freq.fs");
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
		glDrawArraysInstanced(GL_POINTS, 0, 1, interpolated_wavedata_size);
		glBindVertexArray(0);
	}
	if (render_frequency_bands) {
		frequencies_shader.use();
		glBindVertexArray(frequencies_vao);
		glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, frequency_bands);
		glBindVertexArray(0);
	}
}

void WaveRenderer::on_resize(int w, int h)
{
	float aspect_ratio = w / (float)(h);
	volume_quad_shader.use();
	volume_quad_shader.setFloat("aspect_ratio", aspect_ratio);
}

void WaveRenderer::set_wavedata(float wavedata[])
{
	for (size_t i = 0; i < wavedata_size - 1; i++) {
		float cur = wavedata[i];
		float diff = wavedata[i + 1] - cur;
		size_t idx = i * INTERPOLATED_POINTS;
		for (size_t j = 0; j < INTERPOLATED_POINTS; j++) {
			interpolated_wavedata[idx + j] = cur + (diff / INTERPOLATED_POINTS * j);
		}
	}
	interpolated_wavedata[(wavedata_size - 1) * INTERPOLATED_POINTS] = wavedata[wavedata_size - 1];

	glBindBuffer(GL_ARRAY_BUFFER, wavedata_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * interpolated_wavedata_size, interpolated_wavedata.data(), GL_STATIC_DRAW);
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
