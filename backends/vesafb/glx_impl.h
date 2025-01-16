/**
 * FBDev GLX-like backend
 */

#ifndef GLX_H
#define GLX_H

#include <assert.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <linux/fb.h>
#include <linux/kd.h>
#include <linux/vt.h>
#include <GL/gl.h>
#include <math.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/gl.h>

#ifdef __cplusplus
extern "C" {
#endif


 
  typedef struct TinyFBDevGLXContext FBDevGLXContext;
  struct fb_fix_screeninfo FixedInfo;
  struct fb_var_screeninfo VarInfo, OrigVarInfo;

  extern FBDevGLXContext *fbdev_glXCreateContext();

  extern void fbdev_glXDestroyContext();

  extern int fbdev_glXMakeCurrent(FBDevGLXContext *ctx);
	

  extern void fbdev_glXSwapBuffers(FBDevGLXContext *ctx);


#ifdef __cplusplus
}
#endif

#endif
