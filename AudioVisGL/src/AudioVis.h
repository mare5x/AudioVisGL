#pragma once
#include "glad/glad.h"
#include "SDL.h"
#include "SDL_mixer.h"
#include "Shaders/Shader.h"
#include "WaveRenderer.h"


class AudioVis {
public:
	AudioVis();
	~AudioVis() { quit(); }

	void run();

	void input();
	void update();
	void render();
private:
	bool init();
	bool init_gl();
	void quit();

	void handle_input(SDL_Event& e);

	// Audio
	Mix_Music* p_music;

	WaveRenderer wave_renderer;

	// Window variables
	int width, height;
	bool quit_requested;
	unsigned int current_time;  // in milliseconds

	// SDL stuff
	SDL_Event sdl_event;
	SDL_Window* window;
	SDL_GLContext context;

	// OpenGL stuff
	// glm::mat4 projection;  // orthographic projection matrix
};