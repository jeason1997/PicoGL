
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/gl.h> 
#include <GL/glx.h> 
#include "picotk.h"
#include "kbd.c"



static int OriginalVT = -1;
static int ConsoleFD = -1;
static int FrameBufferFD = -1;
void *FrameBuffer = (void *) -1;

static  FBDevGLXContext *ctx;

static void
initialize_fbdev( void )
{
         char ttystr[1000];
	 int fd, vtnumber, ttyfd;
	 int sz;

	 (void) sz;

	 if (geteuid()) {
	         fprintf(stderr, "error: you need to be root\n");
		 exit(1);
	 }


	 /* open the framebuffer device */
	 FrameBufferFD = open("/dev/fb0", O_RDWR);
	 if (FrameBufferFD < 0) {
	         fprintf(stderr, "Error opening /dev/fb0: %s\n", strerror(errno));
		 exit(1);
	 }


	 /* open /dev/tty0 and get the vt number */
	 if ((fd = open("/dev/tty0", O_WRONLY, 0)) < 0) {
	         fprintf(stderr, "error opening /dev/tty0\n");
		 exit(1);
	 }
	 if (ioctl(fd, VT_OPENQRY, &vtnumber) < 0 || vtnumber < 0) {
	         fprintf(stderr, "error: couldn't get a free vt\n");
		 exit(1);
	 }
	 close(fd);

	 /* open the console tty */
	 sprintf(ttystr, "/dev/tty%d", vtnumber);  /* /dev/tty1-64 */
	 ConsoleFD = open(ttystr, O_RDWR | O_NDELAY, 0);
	 if (ConsoleFD < 0) {
	         fprintf(stderr, "error couldn't open console fd\n");
		 exit(1);
	 }

	 /* save current vt number */
	 {
	   struct vt_stat vts;
	   if (ioctl(ConsoleFD, VT_GETSTATE, &vts) == 0)
	         OriginalVT = vts.v_active;
	 }

	 /* disconnect from controlling tty */
	 ttyfd = open("/dev/tty", O_RDWR);
	 if (ttyfd >= 0) {
                ioctl(ttyfd, TIOCNOTTY, 0);
		close(ttyfd);
	 }

	 /* some magic to restore the vt when we exit */
	 {
	   struct vt_mode vt;
	   if (ioctl(ConsoleFD, VT_ACTIVATE, vtnumber) != 0)
	          fprintf(stderr,"ioctl VT_ACTIVATE: %s\n", strerror(errno));
	   if (ioctl(ConsoleFD, VT_WAITACTIVE, vtnumber) != 0)
	          fprintf(stderr,"ioctl VT_WAITACTIVE: %s\n", strerror(errno));
  	   
	   if (ioctl(ConsoleFD, VT_GETMODE, &vt) < 0) {
	          fprintf(stderr, "error: ioctl VT_GETMODE: %s\n", strerror(errno));
	   	  exit(1);
	   }

	   vt.mode = VT_PROCESS;
	   vt.relsig = SIGUSR1;
	   vt.acqsig = SIGUSR1;
	   if (ioctl(ConsoleFD, VT_SETMODE, &vt) < 0) {
	           fprintf(stderr, "error: ioctl(VT_SETMODE) failed: %s\n",
			   strerror(errno));
		   exit(1);
	   }
	 }

	 /* go into graphics mode */
	 if (ioctl(ConsoleFD, KDSETMODE, KD_GRAPHICS) < 0) {
	         fprintf(stderr, "error: ioctl(KDSETMODE, KD_GRAPHICS) failed: %s\n",
			 strerror(errno));
		 exit(1);
	 }


	 /* Get the fixed screen info */
	 if (ioctl(FrameBufferFD, FBIOGET_FSCREENINFO, &FixedInfo)) {
	         fprintf(stderr, "error: ioctl(FBIOGET_FSCREENINFO) failed: %s\n",
			 strerror(errno));
		 exit(1);
	 }

	 

	 /* get the variable screen info */
	 if (ioctl(FrameBufferFD, FBIOGET_VSCREENINFO, &OrigVarInfo)) {
	         fprintf(stderr, "error: ioctl(FBIOGET_VSCREENINFO) failed: %s\n",
			 strerror(errno));
		 exit(1);
	 }

	 /* operate on a copy */
	 VarInfo = OrigVarInfo;


	 if (VarInfo.bits_per_pixel == 16) {
	   VarInfo.red.offset = 11;
	   VarInfo.green.offset = 5;
	   VarInfo.blue.offset = 0;
	   VarInfo.red.length = 5;
	   VarInfo.green.length = 6;
	   VarInfo.blue.length = 5;
	   VarInfo.transp.offset = 0;
	   VarInfo.transp.length = 0;
	 }
	 else if (VarInfo.bits_per_pixel == 32) {
	   VarInfo.red.offset = 16;
	   VarInfo.green.offset = 8;
	   VarInfo.blue.offset = 0;
	   VarInfo.transp.offset = 24;
	   VarInfo.red.length = 8;
	   VarInfo.green.length = 8;
	   VarInfo.blue.length = 8;
	   VarInfo.transp.length = 8;
	 }
	 /* timing values taken from /etc/fb.modes (1280x1024 @ 75Hz) */
	 VarInfo.xres_virtual = VarInfo.xres = 1280;
	 VarInfo.yres_virtual = VarInfo.yres = 1024;
	 VarInfo.pixclock = 7408;
	 VarInfo.left_margin = 248;
	 VarInfo.right_margin = 16;
	 VarInfo.upper_margin = 38;
	 VarInfo.lower_margin = 1;
	 VarInfo.hsync_len = 144;
	 VarInfo.vsync_len = 3;

	 VarInfo.xoffset = 0;
	 VarInfo.yoffset = 0;
	 VarInfo.nonstd = 0;
	 VarInfo.vmode &= ~FB_VMODE_YWRAP; /* turn off scrolling */

	 /* set new variable screen info */
	 if (ioctl(FrameBufferFD, FBIOPUT_VSCREENINFO, &VarInfo)) {
	         fprintf(stderr, "ioctl(FBIOPUT_VSCREENINFO failed): %s\n",
			 strerror(errno));
		 exit(1);
	 }

	   
	 if (FixedInfo.visual != FB_VISUAL_TRUECOLOR &&
	         FixedInfo.visual != FB_VISUAL_DIRECTCOLOR) {
	   fprintf(stderr, "non-TRUE/DIRECT-COLOR visuals (0x%x) not supported by this demo.\n", FixedInfo.visual);
	   exit(1);
	 }

	 /* initialize colormap */
	 if (FixedInfo.visual == FB_VISUAL_DIRECTCOLOR) {
	         struct fb_cmap cmap;
		 unsigned short red[256], green[256], blue[256];
		 int i;

		 /* we're assuming 256 entries here */
		 cmap.start = 0;
		 cmap.len = 256;
		 cmap.red   = red;
		 cmap.green = green;
		 cmap.blue  = blue;
		 cmap.transp = NULL;
		 for (i = 0; i < cmap.len; i++) {
		         red[i] = green[i] = blue[i] = (i << 8) | i;
		 }
		 if (ioctl(FrameBufferFD, FBIOPUTCMAP, (void *) &cmap) < 0) {
		         fprintf(stderr, "ioctl(FBIOPUTCMAP) failed [%d]\n", i);
		 }
	 }

	 /*
	  * fbdev says the frame buffer is at offset zero, and the mmio region
	  * is immediately after.
	  */

	 /* mmap the framebuffer into our address space */
	 FrameBuffer = (caddr_t) mmap(0, /* start */
				      FixedInfo.smem_len, /* bytes */
				      PROT_READ | PROT_WRITE, /* prot */
				      MAP_SHARED, /* flags */
				      FrameBufferFD, /* fd */
				      0 /* offset */);

	 if (FrameBuffer == (caddr_t) - 1) {
	         fprintf(stderr, "error: unable to mmap framebuffer: %s\n",
			 strerror(errno));
		 exit(1);
	 }
	 

#if 0 /* TODO: MMIO support*/
	 /* mmap the MMIO region into our address space */
	 MMIOAddress = (caddr_t) mmap(0, /* start */
				      FixedInfo.mmio_len, /* bytes */
				      PROT_READ | PROT_WRITE, /* prot */
				      MAP_SHARED, /* flags */
				      FrameBufferFD, /* fd */
				      FixedInfo.smem_len /* offset */);
	 if (MMIOAddress == (caddr_t) - 1) {
	         fprintf(stderr, "error: unable to mmap mmio region: %s\n",
			 strerror(errno));
	 }
	 

	 /* try out some simple MMIO register reads */
	 if (1)
	   {
	           typedef unsigned int CARD32;
	           typedef unsigned char CARD8;
#define RADEON_CONFIG_MEMSIZE               0x00f8
#define RADEON_MEM_SDRAM_MODE_REG           0x0158
#define MMIO_IN32(base, offset) \
	           *(volatile CARD32 *)(void *)(((CARD8*)(base)) + (offset))
#define INREG(addr)         MMIO_IN32(MMIOAddress, addr)
		   int sz, type;
		   const char *typeStr[] = {"SDR", "DDR", "64-bit SDR"};
		   sz = INREG(RADEON_CONFIG_MEMSIZE);
		   type = INREG(RADEON_MEM_SDRAM_MODE_REG);
		
			  type >> 30, typeStr[type>>30]);
	   }
#endif

}


static void
shutdown_fbdev( void )
{
        struct vt_mode VT;

	/* restore original variable screen info */
	if (ioctl(FrameBufferFD, FBIOPUT_VSCREENINFO, &OrigVarInfo)) {
	        fprintf(stderr, "ioctl(FBIOPUT_VSCREENINFO failed): %s\n",
			strerror(errno));
		exit(1);
	}
#if 0
	munmap(MMIOAddress, FixedInfo.mmio_len);
#endif
	munmap(FrameBuffer, FixedInfo.smem_len);
	close(FrameBufferFD);

	/* restore text mode */
	ioctl(ConsoleFD, KDSETMODE, KD_TEXT);

	/* set vt */
	if (ioctl(ConsoleFD, VT_GETMODE, &VT) != -1) {
	        VT.mode = VT_AUTO;
		ioctl(ConsoleFD, VT_SETMODE, &VT);
	}

	/* restore original vt */
	if (OriginalVT >= 0) {
   	        ioctl(ConsoleFD, VT_ACTIVATE, OriginalVT);
		OriginalVT = -1;
	}

	close(ConsoleFD);
}

void errorcatcher();		/* routine to handle errors */

void tkSwapBuffers(void)
{

  fbdev_glXSwapBuffers(ctx);
}

int ui_loop(int argc, char **argv, const char *name)
{
 
        int done;
        int i;
	int k;
	KEY kbuf = 0;
	KEYMOD modifiers = 0;
	SCANCODE pscancode = 0;

        ctx = fbdev_glXCreateContext();

	initialize_fbdev();
	initKBD();
      	fbdev_glXMakeCurrent(ctx);
  
	init ();
	reshape (VarInfo.xres, VarInfo.yres);

        done = 0;

	while(!done) {
	   
          
	  updateKBD(&kbuf, &modifiers, &pscancode);
	  /* printf("kbuf == %x\n", kbuf);*/
	  switch(kbuf){
	             case _KEY_UP:
	                         k = KEY_UP;
		                 break;
	             case _KEY_DOWN:
	                         k = KEY_DOWN;
		                 break;
	             case _KEY_LEFT:
	                         k = KEY_LEFT;
				 break;
                     case _KEY_RIGHT:
	                         k = KEY_RIGHT;
				 break;
	             case _KEY_ESCAPE:
	                         done = 1;
	                         break;
	                         default:
	                       
	                         break;
	  }
	  key(k,0);
	  idle();
	
	}
        terminKBD(); 
	shutdown_fbdev();
	return 0;
}


/*
 * Here on an unrecoverable error.
 *
 */
void errorcatcher()
{

}

