#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <SDL/SDL.h>

#include <GL/gl.h> 
#include <GL/glx.h>
#include "picotk.h"

#ifndef M_PI
#  define M_PI 3.14159265
#endif

void tkSwapBuffers(void)
{
	sdl_glXSwapBuffers();
}

int ui_loop(int argc, char **argv, const char *name)
{
	SDL_Surface *screen;
	SDL_GLXContext *ctx = sdl_glXCreateContext();

	SDL_Init(SDL_INIT_VIDEO);

	screen = SDL_SetVideoMode(320, 240, 8, SDL_HWSURFACE | SDL_DOUBLEBUF);
	sdl_glXMakeCurrent(screen,ctx);

	init();
	reshape(320, 240);

	int done = 0;

	while (!done) {
		SDL_Event event;

		while (SDL_PollEvent(&event) > 0) {
			if (event.type == SDL_QUIT)
				done = 1;
			else {
				if (event.type == SDL_KEYDOWN) {
					SDLKey sdlkey=event.key.keysym.sym;
					int k = 0;

					switch (sdlkey) {
						case SDLK_UP:
							k = KEY_UP;
							break;
						case SDLK_DOWN:
							k = KEY_DOWN;
							break;
						case SDLK_LEFT:
							k = KEY_LEFT;
							break;
						case SDLK_RIGHT:
							k = KEY_RIGHT;
							break;
						case SDLK_ESCAPE:
							k = KEY_ESCAPE;
							break;
						default:
							k = (int) sdlkey;
					}
					key(k, 0);
				}
			}
		}
		idle();
	}

	SDL_Quit();
	return 0;
}
