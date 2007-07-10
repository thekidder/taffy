INCLUDES = -I/usr/src/freetype
METASOURCES = AUTO
lib_LIBRARIES = libsore.a
libsore_a_SOURCES = 2d_overlay_thoughts allgl.h camera.cpp camera.h fileio.cpp \
	fileio.h font.cpp font.h geometry.cpp geometry.h particle.cpp particle.h \
	renderer_thoughts sdp_info sore_console.cpp sore_console.h sore_graphics.cpp \
	sore_renderer.cpp sore_util.cpp sore_util.h texture.cpp texture.h
libsore_a_LDFLAGS = -lGL -lGLU -lSDL -lfontconfig -lfreetype -lz
noinst_HEADERS = fileio.h sore.h sore_graphics.h sore_renderer.h
