#-----------------------------------------------------------------------------
# Project Makefile for wdb's feltLoad
#
# This file is processed by automake to generate the non-recursive
# project Makefile
#-----------------------------------------------------------------------------


# Scanned by Autoreconf; must be static (no expansion)
ACLOCAL_AMFLAGS =               -I config/m4

wdb_stationloadbindir= $(bindir)

# Setting Targets
bin_PROGRAMS = wdb-stationplacesync

# This is to be replaced by a "find wdb" macro:
AM_CXXFLAGS = -DSYSCONFDIR=\"$(pkgsysconfdir)\" $(wdb_CFLAGS)

wdb_stationplacesync_LDADD = -lboost_system $(wdb_LIBS)

man1_MANS = wdb-stationplacesync.man

wdb-stationplacesync.man:   wdb-stationplacesync
	help2man -N -n"Loads station meta-data into a wdb database " ./$< --include @srcdir@/detailed.h2m > $@

CLEANFILES = $(man1_MANS)
EXTRA_DIST = detailed.h2m
include src/sources.mk
include doc/doc.mk
include etc/etc.mk
include config/mk/debian.mk
