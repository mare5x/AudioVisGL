#pragma once
#include "glad/glad.h"
#include "SDL.h"
#include "SDL_mixer.h"
#include "Shader.h"
#include "WaveRenderer.h"


class AudioVis {
public:
	AudioVis(const char* path);
	~AudioVis() { quit(); }

	void run();

	void input();
	void update();
	void render();
private:
	bool init();
	bool init_gl();
	void quit();

	void on_resize(int w, int h);

	void handle_input(SDL_Event& e);

	// Audio
	const char* music_path;

	Mix_Music* p_music;

	WaveRenderer wave_renderer;

	// Window variables
	int width, height;
	bool quit_requested;

	// SDL stuff
	SDL_Event sdl_event;
	SDL_Window* window;
	SDL_GLContext context;

	// OpenGL stuff
	// glm::mat4 projection;  // orthographic projection matrix
};