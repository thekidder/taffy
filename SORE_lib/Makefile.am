INCLUDES = -I/usr/include/freetype2/
METASOURCES = AUTO
lib_LIBRARIES = libsore.a
libsore_a_LDFLAGS = -lGL -lGLU -lSDL -lfontconfig -lfreetype -lz
libsore_a_SOURCES = allgl.h camera.cpp camera.h fileio.cpp fileio.h font.cpp \
	font.h geometry.cpp geometry.h manager particle.cpp particle.h resource \
	sdp_info sore.h sore_buffer.cpp sore_console.cpp sore_console.h \
	sore_graphics.cpp sore_graphics.h sore_input.cpp sore_kernel.cpp sore_logger.cpp \
	sore_renderer.cpp sore_renderer.h sore_resource.cpp sore_texture.cpp sore_util.cpp \
	sore_util.h texture.cpp texture.h
noinst_HEADERS = sore_buffer.h sore_input.h sore_kernel.h sore_logger.h \
	sore_resource.h sore_texture.h
