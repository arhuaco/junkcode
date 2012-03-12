## Process this file with automake to produce Makefile.in

#bin_PROGRAMS=miga_ex01

#miga_ex01_SOURCES =                 \
#	common.h                    \
#	pop.c           pop.h       \
#	1dgenome.c      1dgenome.h  \
#	ga_random.c     ga_random.h \
#	ga_params.c     ga_params.h \
#	ga.c            ga.h        \
#	main.c
#miga_ex01_LDADD    = -lm 

lib_LTLIBRARIES   = libmiga.la

libmiga_la_SOURCES = \
	common.h                    \
	pop.c           pop.h       \
	1dgenome.c      1dgenome.h  \
	ga_random.c     ga_random.h \
	ga_params.c     ga_params.h \
	ga.c            ga.h 
libmiga_la_LDFLAGS = -version-info 0:0:0 -lm

pkginclude_HEADERS=miga.h common.h 1dgenome.h ga_params.h pop.h ga.h ga_random.h

# /*vim:noet
# /*vim:set ts=8
