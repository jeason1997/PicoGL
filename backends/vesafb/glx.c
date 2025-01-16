/**
 *  FBDev GLX-like backend
 */

#include <GL/glx.h>
#include <GL/internal/zgl.h>


struct TinyFBDevGLXContext{
  GLContext *gl_context; /* base class */
  void *screen;
  ZBuffer *zb;
  int line_length;
};


/* Create context */
FBDevGLXContext *fbdev_glXCreateContext()
{
        FBDevGLXContext *ctx;
  
        ctx = (FBDevGLXContext *) malloc(sizeof(FBDevGLXContext));
	if (ctx == NULL) 
	  return NULL;
  
	ctx->gl_context = NULL;
	ctx->screen = NULL;
	ctx->zb = NULL;
	ctx->line_length = 0; 
	return ctx;
}

/*! Destroy context */
void fbdev_glXDestroyContext(FBDevGLXContext *ctx)
{
        if (ctx->gl_context != NULL) {
	  glClose();
	}

	free (ctx);
}
/* resize the glx viewport */
static int glX_resize_viewport(GLContext *c,
			       int *xsize_ptr,
			       int *ysize_ptr)
{
        FBDevGLXContext *ctx;
	int xsize;
	int ysize;

	ctx = (FBDevGLXContext *) c->opaque;

	xsize = *xsize_ptr;
	ysize = *ysize_ptr;

	/* we ensure that xsize and ysize are multiples of 2 for the zbuffer. 
	   TODO: find a better solution */
	xsize &= ~3;
	ysize &= ~3;
	
	if (xsize == 0 || ysize == 0)
	  return -1;

	*xsize_ptr = xsize;
	*ysize_ptr = ysize;

	/* resize the Z buffer */
	ZB_resize(c->zb, ctx->zb->pbuf, xsize, ysize);
	return 0;
}


/* we assume here that drawable is a buffer */
int fbdev_glXMakeCurrent(FBDevGLXContext *ctx)
{
        int mode;
	int xsize;
	int ysize;
	int n_colors = 0;
	int bpp;
	ZBuffer *zb;
	extern void*FrameBuffer;

	if (ctx->gl_context == NULL) {
	  /* create the PicoGL context */  
	  xsize = VarInfo.xres;
	  ysize = VarInfo.yres;
	  bpp = VarInfo.bits_per_pixel;
	  
	  switch(bpp)
	    {
	    case 8:
	      mode = ZB_MODE_INDEX;
	      break;
	    case 32:
	      mode = ZB_MODE_RGBA;
	      break;
	    case 24:
	      mode = ZB_MODE_RGB24;
	      break;
	    default:
	      mode = ZB_MODE_5R6G5B;
	      break;
	    }
	  zb = ZB_open (xsize, ysize, mode, n_colors,
			NULL, NULL, NULL);
	  
	  if(zb == NULL)
	    return 0;

	  /* initialisation of the TinyGL interpreter */
	  glInit( zb );
	  ctx->gl_context = gl_get_context();
	  ctx->gl_context->opaque = (void *) ctx;
	  ctx->gl_context->gl_resize_viewport = glX_resize_viewport;

	  /* set the viewport */
	  /*  TIS: !!! HERE SHOULD BE -1 on both to force reshape  */
	  /*  which is needed to make sure initial reshape is  */
	  /*  called, otherwise it is not called..  */
	  ctx->gl_context->viewport.xsize = xsize;
	  ctx->gl_context->viewport.ysize = ysize;
	  ctx->line_length = FixedInfo.line_length;
	  glViewport( 0, 0, xsize, ysize );
	}

	ctx->screen = FrameBuffer;

	ctx->zb=zb;
	return 1;
}

/* Swap buffers */
void fbdev_glXSwapBuffers(FBDevGLXContext *ctx)
{
       GLContext *gl_context; 
       
       assert(ctx->gl_context);
       assert(ctx->screen);
       assert(ctx->zb);
           
       gl_context=gl_get_context();
       ctx=(FBDevGLXContext *)gl_context->opaque;
  
       ZB_copyFrameBuffer(ctx->zb, ctx->screen, ctx->line_length);
}

