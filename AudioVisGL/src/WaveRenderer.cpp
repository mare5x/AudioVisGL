#include "WaveRenderer.h"

WaveRenderer::~WaveRenderer()
{
	glDeleteVertexArrays(1, &quad_vao);
	glDeleteBuffers(1, &quad_vbo);
}

void WaveRenderer::init()
{
	float quad[] = {
		-0.5f, -0.5f,
		-0.5f, 0.5f,
		0.5f, -0.5f,
		0.5f, 0.5f
	};

	glGenBuffers(1, &quad_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);

	glGenVertexArrays(1, &quad_vao);
	glBindVertexArray(quad_vao);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void *)(0));

	quad_shader = Shader("src/Shaders/vert_shader.vs", "src/Shaders/frag_shader.fs");
	quad_shader.use();
	quad_shader.setFloat("scale", 1);

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
}

void WaveRenderer::render()
{
	quad_shader.use();

	glBindVertexArray(quad_vao);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);

	wavedata_shader.use();
	glBindVertexArray(wavedata_vao);
	glDrawArraysInstanced(GL_POINTS, 0, 1, wavedata_size);
	glBindVertexArray(0);
}

void WaveRenderer::set_wavedata(float wavedata[])
{
	glBindBuffer(GL_ARRAY_BUFFER, wavedata_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * wavedata_size, wavedata, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void WaveRenderer::set_volume(float volume)
{
	quad_shader.use();
	quad_shader.setFloat("scale", volume);
}
