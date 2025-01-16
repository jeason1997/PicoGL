/**
 * SDL GLX-like backend
 */

#ifndef GLX_H
#define GLX_H

#ifdef __cplusplus
extern "C" {
#endif

#include <GL/gl.h>
#include <GL/internal/zgl.h>
#include <SDL/SDL_video.h>

typedef struct {
	GLContext   *gl_context;
	SDL_Surface *surface;
	unsigned int pitch;
	unsigned int *palette;
	unsigned char *indexes;
} SDL_GLXContext;

extern SDL_GLXContext *sdl_glXCreateContext ();
extern void sdl_glXDestroyContext( SDL_GLXContext *ctx );
extern int sdl_glXMakeCurrent( SDL_Surface *surface, SDL_GLXContext *ctx );
extern void sdl_glXSwapBuffers();

#ifdef __cplusplus
}
#endif

#endif  /*  TEDDY_TINYGL_SDLSWGL_H  */


