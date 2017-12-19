#include "AudioVis.h"
#include "AudioFFT.h"
#include <vector>


const int WIDTH = 800;
const int HEIGHT = 600;

const int FPS = 120;
const float FPS_ms = 1 / static_cast<float>(FPS) * 1000;

const int WAVEDATA_SAMPLES = 2048;
const int FREQUENCIES_SIZE = WAVEDATA_SAMPLES / 2 + 1;
const int FREQUENCY_BANDS = 128;

float wavedata[WAVEDATA_SAMPLES];
float fft_magnitudes[FREQUENCIES_SIZE];
float frequency_bands[FREQUENCY_BANDS];  // index is a frequency band, f[i] is the magnitude in range [0, 1.75]
float volume = 1;


void audio_callback(int channel, void* stream, int len, void* udata);
void update_frequencies();


AudioVis::AudioVis() :
	width(WIDTH), height(HEIGHT),
	wave_renderer(WAVEDATA_SAMPLES, FREQUENCY_BANDS)
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

	update_frequencies();
	wave_renderer.set_frequency_bands(frequency_bands);
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
	p_music = Mix_LoadMUS("D:/Mare5/Music/Soundtracks/Sid Meier's Civilization VI (2016) [MP3, V0]/Sogno di Volare (Civilization VI Main Theme) by Christopher Tin/01. Sogno di Volare ('The Dream of Flight').mp3");
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
		else if (e.key.keysym.sym == SDLK_e)
			wave_renderer.toggle_render_frequency_bands();
	}
}

void audio_callback(int channel, void * stream, int len, void * udata)
{
	Sint16* data = (Sint16*)stream;
	len /= 2;  // len is in bytes, 16 bits == 2 bytes
	float max = -1;
	float min = 1;
	int samples_per_wavedata = len / 2.0f / WAVEDATA_SAMPLES;
	float running_avg = 0;
	int wavedata_idx = WAVEDATA_SAMPLES - 1;  // this way the waves travel from left to right, since the sample stream is processed in chronological order
	int c = 0;
	for (int i = 0; i < len / 2; i++, c++) {
		float avg_sample = (data[2 * i] + data[2 * i + 1]) / 2.0f;  // average the left and right audio channel samples, (len is always a power of 2)
		float d = avg_sample / (1 << 15);  // scale to the range of [-1, 1]
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
	// SDL_Log("%d %f %f\n", len, min, max);
}

void update_frequencies()
{
	std::vector<float> re(audiofft::AudioFFT::ComplexSize(WAVEDATA_SAMPLES));
	std::vector<float> im(audiofft::AudioFFT::ComplexSize(WAVEDATA_SAMPLES));

	audiofft::AudioFFT fft;
	fft.init(WAVEDATA_SAMPLES);
	fft.fft(wavedata, re.data(), im.data());

	const int upper_bound_freq_idx = std::fmin(FREQUENCIES_SIZE, 5000.0f * WAVEDATA_SAMPLES / 44100.0f);
	const int steps_per_freq_band = upper_bound_freq_idx / (float)(FREQUENCY_BANDS);
	float avg_freq = 0;
	for (int i = 0; i < upper_bound_freq_idx; i++) {
		fft_magnitudes[i] = sqrt(re[i] * re[i] + im[i] * im[i]);

		// printf("%d Hz: %f\n", (int)(44100.0f / WAVEDATA_SAMPLES * i), fft_magnitudes[i]);

		avg_freq += fft_magnitudes[i] / steps_per_freq_band;
		if (i % steps_per_freq_band == 0) {
			frequency_bands[i / steps_per_freq_band] = std::fmin(1.75f, avg_freq / 69.0f);
			avg_freq = 0;
		}
	}
}