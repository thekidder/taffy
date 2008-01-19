INCLUDES = -I/usr/include/freetype2/
METASOURCES = AUTO
lib_LIBRARIES = libsore.a
libsore_a_SOURCES = inotify-cxx.cpp sore_buffer.cpp sore_camera.cpp \
	sore_console.cpp sore_fileio.cpp sore_fileio_linux.cpp sore_font.cpp sore_graphics.cpp \
	sore_input.cpp sore_interpolater.cpp sore_kernel.cpp sore_logger.cpp \
	sore_logginginit.cpp sore_math.cpp sore_profiler.cpp sore_renderer.cpp sore_resource.cpp \
	sore_settings.cpp sore_shaders.cpp sore_texture.cpp sore_timing.cpp sore_util.cpp
noinst_HEADERS = inotify-cxx.h sore.h sore_allgl.h sore_buffer.h sore_camera.h \
	sore_console.h sore_defines.h sore_fileio.h sore_font.h sore_graphics.h sore_input.h \
	sore_interpolater.h sore_kernel.h sore_logger.h sore_logginginit.h sore_math.h \
	sore_profiler.h sore_renderer.h sore_resource.h sore_settings.h sore_shaders.h \
	sore_task.h sore_texture.h sore_util.h
libsore_a_LDFLAGS = -lGL -lGLEW -lGLU -lSDL -lfontconfig -lfreetype -lz
