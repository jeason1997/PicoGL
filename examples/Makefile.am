INCLUDES = \
	-I$(top_builddir)/include \
	-I$(top_srcdir)/backends/$(GFX_IMPL) \
	-DGLX_IMPL_HEADER="\"$(GLX_IMPL_HEADER)\"" \
	$(NANOX_CFLAGS)

COMMON_LIST = \
	texobj \
	mech \
	gears \
	spin

if COND_SDL
SDL_PICOGL = cube
endif

bin_PROGRAMS = $(COMMON_LIST) $(SDL_PICOGL)

mech_SOURCES = mech.c
mech_LDADD = \
	$(top_builddir)/src/libPicoGL.la \
	$(top_builddir)/src/glu/libPicoGLU.la -lm

texobj_SOURCES = texobj.c
texobj_LDADD = \
	$(top_builddir)/src/libPicoGL.la -lm

gears_SOURCES = gears.c
gears_LDADD = \
	$(top_builddir)/src/libPicoGL.la -lm

spin_SOURCES = spin.c
spin_LDADD = \
	$(top_builddir)/src/libPicoGL.la -lm

cube_SOURCES = cube.c
cube_LDADD = \
	$(top_builddir)/src/libPicoGL.la \
	$(top_builddir)/src/glu/libPicoGLU.la \
	@SDL_LIBS@ -lm
