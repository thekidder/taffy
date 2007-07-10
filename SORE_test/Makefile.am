INCLUDES = -I$(top_srcdir)/SORE_lib -I/usr/include/freetype2/
METASOURCES = AUTO
bin_PROGRAMS = sore_test
sore_test_LDADD = $(top_builddir)/SORE_lib/libsore.a
sore_test_SOURCES = main.h sore_test.cpp
sore_test_LDFLAGS = -lGL -lGLU -lSDL -lfontconfig -lfreetype -lz
