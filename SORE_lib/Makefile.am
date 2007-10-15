INCLUDES = -I/usr/include/freetype2/
METASOURCES = AUTO
lib_LIBRARIES = libsore.a
libsore_a_LDFLAGS = -lGL -lGLU -lSDL -lfontconfig -lfreetype -lz
libsore_a_SOURCES = sore_buffer.cpp sore_camera.cpp sore_console.cpp \
	sore_fileio.cpp sore_font.cpp sore_glext.cpp sore_graphics.cpp sore_input.cpp \
	sore_kernel.cpp sore_logger.cpp sore_math.cpp sore_profiler.cpp sore_renderer.cpp \
	sore_resource.cpp sore_texture.cpp sore_util.cpp
noinst_HEADERS = glextproc.h sore.h sore_allgl.h sore_buffer.h sore_camera.h \
	sore_console.h sore_fileio.h sore_font.h sore_glext.h sore_graphics.h sore_input.h \
	sore_kernel.h sore_logger.h sore_math.h sore_profiler.h sore_renderer.h \
	sore_resource.h sore_texture.h sore_util.h
