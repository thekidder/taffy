INCLUDES = -I$(top_srcdir)/SORE_lib -I/usr/include/freetype2/
METASOURCES = AUTO
bin_PROGRAMS = sore_resource_testing
sore_resource_testing_LDADD = $(top_builddir)/SORE_lib/libsore.a
sore_resource_testing_LDFLAGS = -lfontconfig -lfreetype -lz -lGL -lGLU -lSDL
sore_resource_testing_SOURCES = sore_resource_testing.cpp
