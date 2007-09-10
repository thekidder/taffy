INCLUDES = -I$(top_srcdir)/SORE_lib -I/usr/include/freetype2/
METASOURCES = AUTO
bin_PROGRAMS = sore_test
sore_test_LDADD = $(top_builddir)/SORE_lib/libsore.a
sore_test_SOURCES = noise.cpp perlin.cpp physics.cpp sore_test.cpp \
	terrain_graph.cpp
sore_test_LDFLAGS = -lGL -lGLU -lSDL -lfontconfig -lfreetype -lz
noinst_HEADERS = main.h noise.h perlin.h physics.h terrain_graph.h
