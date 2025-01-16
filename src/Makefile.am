SUBDIRS = glu

INCLUDES = \
	-I$(top_builddir)/include \
	-I$(top_srcdir)/backends/$(GFX_IMPL)

lib_LTLIBRARIES = libPicoGL.la

libPicoGL_la_SOURCES = \
	api.c \
	list.c \
	vertex.c \
	init.c \
	matrix.c \
	texture.c \
	misc.c \
	clear.c \
	light.c \
	clip.c \
	select.c \
	get.c \
	error.c \
	zbuffer.c \
	zline.c \
	zdither.c \
	ztriangle.c \
	zmath.c \
	image_util.c \
	oscontext.c \
	msghandling.c \
	arrays.c \
	specbuf.c \
	memory.c \
	$(top_srcdir)/backends/$(GFX_IMPL)/glx.c \
	$(top_srcdir)/backends/$(GFX_IMPL)/tk.c

libPicoGL_la_CFLAGS = \
	@X_CFLAGS@ \
	$(NANOX_CFLAGS) \
	@SDL_CFLAGS@ \
	-DGLX_IMPL_HEADER="\"$(GLX_IMPL_HEADER)\""
libPicoGL_la_LDFLAGS = $(GFX_LIBS)
