lib_LTLIBRARIES = libPicoGLU.la

INCLUDES = \
	-I$(top_builddir)/include
		
libPicoGLU_la_SOURCES = \
	glu.c \
	glu_sphere.c \
	glu_disk.c \
	glu_perspective.c \
	glu_cylinder.c \
	glu_torus.c

libPicoGLU_la_CFLAGS = \
	-DGLX_IMPL_HEADER="\"$(GLX_IMPL_HEADER)\""
