#include "WaveRenderer.h"


const int INTERPOLATED_POINTS = 16;


WaveRenderer::WaveRenderer(int _wavedata, int _frequency_bands)
	: wavedata_size(_wavedata), frequency_bands(_frequency_bands),
	render_volume(true), render_wave(true), render_frequency_bands(true)
{

}

WaveRenderer::~WaveRenderer()
{
	glDeleteVertexArrays(1, &volume_quad_vao);
	glDeleteVertexArrays(1, &wavedata_vao);
	glDeleteVertexArrays(1, &frequencies_vao);
	const GLuint vbos[] = { volume_quad_vbo, wavedata_vbo, wavedata_indices_vbo, frequencies_bar_vbo, frequencies_heights_vbo };
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

	volume_quad_shader = Shader("Shaders/quad.vert", "Shaders/quad.frag");
	volume_quad_shader.use();
	volume_quad_shader.setFloat("scale", 1);
	volume_quad_shader.setFloat("aspect_ratio", 1);

	// Wavedata VAO layout:
	// attribute 0: float -> y offset
	// attribute 1: int   -> index of vertex
	glEnable(GL_LINE_SMOOTH);
	glLineWidth(2);

	glGenVertexArrays(1, &wavedata_vao);
	glBindVertexArray(wavedata_vao);
	
	glGenBuffers(1, &wavedata_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, wavedata_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * wavedata_size, NULL, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void *)(0));

	// Upload a simple array of integers, where a[i] = i. Use it in the shader to calculate
	// the x offset of a point.
	glGenBuffers(1, &wavedata_indices_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, wavedata_indices_vbo);
	int* indices = new int[wavedata_size];
	for (int i = 0; i < wavedata_size; ++i)
		indices[i] = i;
	glBufferData(GL_ARRAY_BUFFER, sizeof(int) * wavedata_size, indices, GL_STATIC_DRAW);
	delete[] indices;

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 1, GL_INT, GL_FALSE, sizeof(int), (void *)(0));

	glBindVertexArray(0);

	wavedata_shader = Shader("Shaders/wavedata.vert", "Shaders/wavedata.frag");
	wavedata_shader.use();
	wavedata_shader.setFloat("wavedata_size", wavedata_size);

	// Begin VAO for frequency bars
	glGenVertexArrays(1, &frequencies_vao);
	glBindVertexArray(frequencies_vao);

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

	glEnableVertexAttribArray(0);
	// Defines the layout of the bound VBO's data. 
	// Points to the currently bound GL_ARRAY_BUFFER, i.e. frequencies_bar_vbo, offset by 0.
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void *)(0));

	glGenBuffers(1, &frequencies_heights_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, frequencies_heights_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * frequency_bands, NULL, GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void *)(0));

	// glVertexAttribDivisor(0, 0);
	glVertexAttribDivisor(1, 1);

	glBindVertexArray(0);

	frequencies_shader = Shader("Shaders/freq.vert", "Shaders/freq.frag");
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
		glDrawArrays(GL_LINE_STRIP, 0, wavedata_size);
		glBindVertexArray(0);
	}
	if (render_frequency_bands) {
		frequencies_shader.use();
		glBindVertexArray(frequencies_vao);
		// Use the layout specified by the VAO and draw 4 vertices sequentially as specified by the layout, repeated frequncy_bands times.
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
	glBindBuffer(GL_ARRAY_BUFFER, wavedata_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * wavedata_size, wavedata, GL_DYNAMIC_DRAW);
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
