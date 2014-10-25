AUTOMAKE_OPTIONS = foreign no-dependencies subdir-objects
bin_PROGRAMS = mal_daemon
mal_daemon_SOURCES = src/mal_daemon.c src/mal_daemon.h \
					src/Log.c src/Log.h \
					src/sql.c src/sql.h \
					src/configuration.c src/configuration.h \
					src/download.c src/download.h

AM_CPPFLAGS = -Wall

cleanall: distclean
	-rm -rf Makefilee Makefile.in aclocal.m4 autom4te.cache config.h* install-sh missing configure config.guess config.sub