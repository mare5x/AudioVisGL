#include "AudioVis.h"


const int WIDTH = 800;
const int HEIGHT = 600;

const int FPS = 120;
const float FPS_ms = 1 / static_cast<float>(FPS) * 1000;

void audio_callback(int channel, void* stream, int len, void* udata);

const int WAVEDATA_SIZE = 4096;
float wavedata[WAVEDATA_SIZE];
float volume = 1;


AudioVis::AudioVis() :
	width(WIDTH), height(HEIGHT),
	wave_renderer(WAVEDATA_SIZE)
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
	wave_renderer.set_volume(volume);
	wave_renderer.set_wavedata(wavedata);
}

void AudioVis::render()
{
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	wave_renderer.render();

	SDL_GL_SwapWindow(window);
}

bool AudioVis::init()
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) == 0) {
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

		if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
			SDL_Log("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
			return false;
		}

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
	wave_renderer.init();

	Mix_RegisterEffect(MIX_CHANNEL_POST, audio_callback, NULL, NULL);
	p_music = Mix_LoadMUS("D:/Mare5/Music/Songy stuff/Wham! - Last Christmas.mp3");
	if (p_music == NULL)
		printf("Mix_PlayMusic: %s\n", Mix_GetError());
	if (Mix_PlayMusic(p_music, -1) != 0)
		printf("Mix_PlayMusic: %s\n", Mix_GetError());

	return true;
}

void AudioVis::quit()
{
	Mix_FreeMusic(p_music);
	Mix_CloseAudio();
	Mix_Quit();

	SDL_GL_DeleteContext(context);
	context = nullptr;
	SDL_DestroyWindow(window);
	window = nullptr;

	SDL_Quit();
}

void AudioVis::handle_input(SDL_Event & e)
{
	if (e.type == SDL_KEYUP) {
		if (e.key.keysym.sym == SDLK_q)
			wave_renderer.toggle_render_volume();
		else if (e.key.keysym.sym == SDLK_w)
			wave_renderer.toggle_render_wave();
	}
}

void audio_callback(int channel, void * stream, int len, void * udata)
{
	Sint16* data = (Sint16*)stream;
	len /= 2;  // len is in bytes, 16 bits == 2 bytes
	float max = -1;
	float min = 1;
	int samples_per_wavedata = len / WAVEDATA_SIZE;
	float running_avg = 0;
	int wavedata_idx = WAVEDATA_SIZE - 1;  // this way the waves travel from left to right, since the sample stream is processed in chronological order
	int c = 0;
	for (int i = 0; i < len; i++, c++) {
		float d = (float)(data[i]) / (1 << 15);
		max = d > max ? d : max;
		min = d < min ? d : min;
		
		running_avg += d / samples_per_wavedata;
		if (c == samples_per_wavedata) {
			wavedata[wavedata_idx] = running_avg;
			wavedata_idx--;
			running_avg = 0;
			c = 0;
		}
	}
	wavedata[wavedata_idx] = running_avg;
	
	volume = (volume + std::fmin(1, std::fmax(0.3f, 0.3f + max * 0.7f))) / 2.0f;
	SDL_Log("%d %f %f\n", len, min, max);
}
