#include "AudioVis.h"


const int WIDTH = 800;
const int HEIGHT = 600;

const int FPS = 120;
const float FPS_ms = 1 / static_cast<float>(FPS) * 1000;


AudioVis::AudioVis() :
	width(WIDTH), height(HEIGHT)
{
	if (!init())
		quit();
}

void AudioVis::run()
{
	current_time = SDL_GetTicks();
	while (!quit_requested) {
		unsigned int start_time = SDL_GetTicks();

		input();

		update();

		render();

		unsigned int frame_time = SDL_GetTicks() - start_time;
		if (frame_time < FPS_ms)
			SDL_Delay(FPS_ms - frame_time);
	}
}

void AudioVis::input()
{
	while (SDL_PollEvent(&sdl_event)) {
		if (sdl_event.type == SDL_QUIT) {
			quit_requested = true;
			return;
		}
		handle_input(sdl_event);
	}
}

void AudioVis::update()
{
}

void AudioVis::render()
{
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	SDL_GL_SwapWindow(window);
}

bool AudioVis::init()
{
	if (SDL_Init(SDL_INIT_VIDEO) == 0) {
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

		window = SDL_CreateWindow("AudioVis", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL);
		if (!window) {
			SDL_Log("CREATE WINDOW ERROR: %s", SDL_GetError());
			return false;
		}

		context = SDL_GL_CreateContext(window);
		if (!context) {
			SDL_Log("CREATE CONTEXT ERROR: %s", SDL_GetError());
			return false;
		}

		if (!gladLoadGLLoader(static_cast<GLADloadproc>(SDL_GL_GetProcAddress)))
			return false;

		glViewport(0, 0, width, height);

		SDL_GL_SetSwapInterval(0);  // uncap FPS

		return init_gl();
	}
	else {
		SDL_Log("INIT ERROR: %s", SDL_GetError());
		return false;
	}
}

bool AudioVis::init_gl()
{
	return true;
}

void AudioVis::quit()
{
	SDL_GL_DeleteContext(context);
	context = nullptr;
	SDL_DestroyWindow(window);
	window = nullptr;

	SDL_Quit();
}

void AudioVis::handle_input(SDL_Event & e)
{
}
