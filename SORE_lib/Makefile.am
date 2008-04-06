INCLUDES = -I/usr/local/include/CEGUI -I/usr/include/freetype2/
METASOURCES = AUTO
lib_LIBRARIES = libsore.a
libsore_a_SOURCES = inotify-cxx.cpp sore_2dprimitives.cpp sore_2dscenegraph.cpp \
	sore_fileio.cpp sore_fileio_linux.cpp sore_font.cpp sore_graphics.cpp sore_input.cpp \
	sore_interpolater.cpp sore_kernel.cpp sore_logger.cpp sore_logginginit.cpp sore_material.cpp \
	sore_math.cpp sore_network.cpp sore_network_client.cpp sore_network_server.cpp \
	sore_profiler.cpp sore_renderer.cpp sore_resource.cpp sore_screen.cpp sore_screeninfo.cpp \
	sore_settings.cpp sore_shaders.cpp sore_texture.cpp sore_timing.cpp sore_util.cpp
noinst_HEADERS = inotify-cxx.h sore.h sore_2dprimitives.h sore_2dscenegraph.h \
	sore_allgl.h sore_defines.h sore_fileio.h sore_font.h sore_gamestate.h \
	sore_graphics.h sore_input.h sore_interpolater.h sore_kernel.h sore_logger.h \
	sore_logginginit.h sore_material.h sore_math.h sore_network.h sore_profiler.h \
	sore_renderer.h sore_resource.h sore_screen.h sore_screeninfo.h sore_settings.h \
	sore_shaders.h sore_task.h sore_texture.h sore_tree.h sore_util.h
libsore_a_LDFLAGS = -lGL -lGLEW -lGLU -lSDL -lfontconfig -lfreetype -lz
