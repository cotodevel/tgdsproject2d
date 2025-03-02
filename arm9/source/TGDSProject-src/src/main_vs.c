//disable _CRT_SECURE_NO_WARNINGS message to build this in VC++
#pragma warning(disable:4996)

#ifdef _MSC_VER

#ifdef _MSC_VER
#include <windows.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>

#if defined(_MSC_VER)
#include <time.h>
#include <profileapi.h>
#include "SDL.h"
#include "SDL_image.h"
#endif

#include "main_vs.h"

#if defined(_MSC_VER)

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

#if defined(_MSC_VER)
static LARGE_INTEGER counter_freq;
#endif



#if defined(_MSC_VER)
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
					
					break;

				case SDLK_s:
					
					break;

				case SDLK_w:
					
					break;

				case SDLK_x:
					
					break;

				case SDLK_c:
					
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

void sdl_release()
{
	SDL_DestroyTexture(icons);
	SDL_DestroyTexture(bg);

	IMG_Quit();

	SDL_DestroyWindow(window);
	SDL_Quit();
}

bool sdl_init(void)
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_AUDIO) != 0) {
		
		return 1;
	}

	if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
		
		SDL_Quit();
		return 1;
	}

	window = SDL_CreateWindow("SnemulDS 0.2", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 300, 200, SDL_WINDOW_SHOWN);

	renderer =  SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	audio_dev = SDL_OpenAudioDevice(NULL, 0, &audio_spec, &audio_spec, SDL_AUDIO_ALLOW_FREQUENCY_CHANGE);
	if(!audio_dev) {
		sdl_release();
		return 1;
	}

	SDL_PauseAudioDevice(audio_dev, SDL_FALSE);

	return 0;
}


#if defined(_MSC_VER)
//entrypoint
int main(int argc, char **argv)
{
	char rom_path[256] ;
	char sprites_path[256] = {0};
	char save_path[256] = {0};
	
#if defined(__WIN32__)
	QueryPerformanceFrequency(&counter_freq);
#endif

	if (sdl_init()) {
		return -1;
	}

	
	//entering main loop
	while(1==1){
	
	}
	
	//exit
	sdl_release();
	return 0;
}
#endif

#endif