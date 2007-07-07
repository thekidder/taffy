INCLUDES = -I$(top_srcdir)/SORE_lib
METASOURCES = AUTO
bin_PROGRAMS = sore_test
sore_test_LDADD = $(top_builddir)/SORE_lib/libsore.a
sore_test_SOURCES = main.h sore_test.cpp
sore_test_LDFLAGS = -lfreetype -lfontconfig -lSDL -lGL -lGLU
