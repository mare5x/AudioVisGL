#pragma once
#include "glad/glad.h"
#include "SDL.h"
#include "Shaders/Shader.h"

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

	int width, height;

	bool quit_requested;

	unsigned int current_time;

	SDL_Event sdl_event;

	SDL_Window* window;
	SDL_GLContext context;
};