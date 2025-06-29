//disable _CRT_SECURE_NO_WARNINGS message to build this in VC++
#pragma warning(disable:4996)

#ifdef _MSC_VER
#include <windows.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>

#if defined(WIN32)
#include "TGDSTypes.h"
#include <time.h>
#include <profileapi.h>
#include "SDL.h"
#include "SDL_image.h"
#endif


#if defined(WIN32)
#elif defined(__APPLE__)
#include <CoreFoundation/CoreFoundation.h>
#include <getopt.h>
#endif


#ifdef WIN32

static SDL_Window *window = NULL;
static SDL_Renderer* renderer = NULL;

static SDL_Texture *bg;
static SDL_Texture *shell;
static SDL_Texture *icons;
static SDL_Rect shell_rect;
static SDL_Rect bg_rect;

static SDL_AudioSpec audio_spec;
static SDL_AudioDeviceID audio_dev;
#endif

#if defined(__WIN32__)
static LARGE_INTEGER counter_freq;
#endif

void sdl_release(void);
bool sdl_init(void);

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif

#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
int get_timestamp(void)
{
#if defined(__WIN32__) //VS2012
	LARGE_INTEGER count;

	QueryPerformanceCounter(&count);
	return (count.QuadPart * 1000000)/counter_freq.QuadPart;
#endif
#if !defined(__WIN32__) && !defined(ARM9) //linux 
	struct timespec time;

	clock_gettime(CLOCK_REALTIME, &time);
	return (time.tv_sec * 1000000 + time.tv_nsec/1000);
#endif
#if !defined(__WIN32__) && defined(ARM9) //NDS
	return (timestamp_t)0;
#endif
}


#ifdef WIN32
static int handle_sdl_events(SDL_Event *event)
{
	char save_path[256];

	switch(event->type) {
		case SDL_QUIT:
			return 1;

		case SDL_WINDOWEVENT:
			switch (event->window.event) {
				case SDL_WINDOWEVENT_SIZE_CHANGED:
					break;
			}
			break;

		case SDL_MOUSEBUTTONDOWN:
			switch (event->button.button) {
				case SDL_BUTTON_LEFT:
					//handle_click(event->button.x, event->button.y, 1);
					break;

				case SDL_BUTTON_RIGHT:
					break;

				case SDL_BUTTON_MIDDLE:
					break;
			}
			break;

		case SDL_MOUSEBUTTONUP:
			switch (event->button.button) {
				case SDL_BUTTON_LEFT:
					//handle_click(event->button.x, event->button.y, 0);
					break;

				case SDL_BUTTON_RIGHT:
					break;

				case SDL_BUTTON_MIDDLE:
					break;
			}
			break;

		case SDL_MOUSEMOTION:
			break;

		case SDL_MOUSEWHEEL:
			break;

		case SDL_KEYDOWN:
			switch (event->key.keysym.sym) {
				case SDLK_AC_BACK:
				case SDLK_ESCAPE:
				case SDLK_q:
					return 1;

				case SDLK_r:
					//tamalib_set_exec_mode(EXEC_MODE_RUN);
					break;

				case SDLK_s:
					//tamalib_set_exec_mode(EXEC_MODE_STEP);
					break;

				case SDLK_w:
					//tamalib_set_exec_mode(EXEC_MODE_NEXT);
					break;

				case SDLK_x:
					//tamalib_set_exec_mode(EXEC_MODE_TO_CALL);
					break;

				case SDLK_c:
					//tamalib_set_exec_mode(EXEC_MODE_TO_RET);
					break;

				case SDLK_f:
					
					break;

				case SDLK_b:
					
					break;

				case SDLK_n:
					
					break;

				case SDLK_i:
					sdl_release();
					sdl_init();
					break;

				case SDLK_d:
					sdl_release();
					sdl_init();
					break;

				case SDLK_t:
					sdl_release();
					sdl_init();
					break;

				case SDLK_LEFT:
					
					break;

				case SDLK_DOWN:
					
					break;

				case SDLK_RIGHT:
					
					break;
			}
			break;

		case SDL_KEYUP:
			switch (event->key.keysym.sym) {
				case SDLK_LEFT:
					
					break;

				case SDLK_DOWN:
					
					break;

				case SDLK_RIGHT:
					
					break;
			}
			break;
	}

	return 0;
}
#endif

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif

#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
int hal_handler(void)
{
	#ifdef WIN32
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (handle_sdl_events(&event)) {
			return 1;
		}
	}
	#endif
	return 0;
}

#ifdef WIN32
static void audio_callback(void *userdata, Uint8 *stream, int len)
{
	/*
	unsigned int i;
	int samples = len / sizeof(float);

	if (is_audio_playing) {
		//Generate the required frequency
		for (i = 0; i < samples; i++) {
			((float *) stream)[i] = AUDIO_VOLUME * SDL_sinf(2 * M_PI * (i + sin_pos) * current_freq / (AUDIO_FREQUENCY * 10));
		}

		sin_pos = (sin_pos + samples) % (AUDIO_FREQUENCY * 10);
	} else {
		//No sound
		SDL_memset(stream, 0, len);
		sin_pos = 0;
	}
	*/
}

static void sdl_release(void)
{
	SDL_DestroyTexture(icons);
	SDL_DestroyTexture(bg);

	IMG_Quit();

	SDL_DestroyWindow(window);
	SDL_Quit();
}

#define APP_NAME "templateproject"

static bool sdl_init(void)
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_AUDIO) != 0) {
		printf("Failed to initialize SDL: %s\n", SDL_GetError());
		return 1;
	}

	if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
		printf("Failed to initialize SDL_image: %s\n", SDL_GetError());
		SDL_Quit();
		return 1;
	}

	window = SDL_CreateWindow(APP_NAME, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_SHOWN);

	renderer =  SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	#define BACKGROUND_PATH	"0:/someimagedir/image.png"
	bg = IMG_LoadTexture(renderer, BACKGROUND_PATH);
	if(!bg) {
		printf("Failed to load the background image: %s\n", SDL_GetError());
		sdl_release();
		return 1;
	}

	#define AUDIO_FREQUENCY			48000
	#define AUDIO_SAMPLES			480 // 10 ms @ 48000 Hz
	#define AUDIO_VOLUME			0.2f

	SDL_memset(&audio_spec, 0, sizeof(audio_spec));
	audio_spec.freq = AUDIO_FREQUENCY;
	audio_spec.format = AUDIO_F32SYS;
	audio_spec.channels = 1;
	audio_spec.samples = AUDIO_SAMPLES;
	audio_spec.callback = &audio_callback;

	audio_dev = SDL_OpenAudioDevice(NULL, 0, &audio_spec, &audio_spec, SDL_AUDIO_ALLOW_FREQUENCY_CHANGE);
	if(!audio_dev) {
		printf("Failed to open the audio device: %s\n", SDL_GetError());
		sdl_release();
		return 1;
	}

	SDL_PauseAudioDevice(audio_dev, SDL_FALSE);

	return 0;
}
#endif

#ifdef WIN32
//entrypoint
int main(int argc, char **argv)
{
	char rom_path[256] = "";
	bool gen_header = 0;
	bool extract_sprites = 0;
	bool modify_sprites = 0;

#if defined(__WIN32__)
	QueryPerformanceFrequency(&counter_freq);
#endif

	if (sdl_init()) {
		printf("FATAL: Error while initializing application !\n");
		//SDL_free(g_program);
		return -1;
	}

	sdl_release();

	return 0;
}
#endif