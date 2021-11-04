# $Id: Makefile.am,v 1.8 2002/06/28 05:01:33 vadimp Exp $

SUFFIXES=.koi8-r

.koi8-r.c: ce
	./ce --input-codepage=koi8-r < $< > $@

if DEBUG
DEBUG_FLAGS=-g
else
DEBUG_FLAGS=-DNDEBUG
endif

if PROFILE
PROFILE_FLAGS=-pg
endif

if COMPILE_GNU_GETOPT
GNU_GETOPT_SRC = getopt.c getopt1.c getopt.h
endif

if NO_CYGWIN
CYGWIN_FLAGS=-mno-cygwin
endif

AM_CFLAGS=-Wall -O2 $(DEBUG_FLAGS) $(PROFILE_FLAGS) $(CYGWIN_FLAGS)

bin_PROGRAMS = fe ce

GENERATED_FILES = ui.c fe.c lingtbl.c

fe_SOURCES =	fe.h \
		cyrillic.c tables.c cyrillic.h \
		avl.c avl_low.c avl.h \
		reader.c reader.h \
		context.h context.c \
		util.c util.h \
		wrappers.c wrappers.h \
		ui.h \
		$(GNU_GETOPT_SRC)
		
nodist_fe_SOURCES = $(GENERATED_FILES)

ce_SOURCES =	ce.c \
		cyrillic.c cyrillic.h tables.c \
		$(GNU_GETOPT_SRC)

EXTRA_DIST =	getopt.h getopt.c getopt1.c config-win32.h \
		ui.koi8-r

MOSTLYCLEANFILES = $(GENERATED_FILES)

# Cygwin sets EXEEXT to .exe -- must use it.
fe.c: ce$(EXEEXT)
ui.c: ce$(EXEEXT)
lingtbl.c: ce$(EXEEXT)
