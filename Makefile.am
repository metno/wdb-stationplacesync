#-----------------------------------------------------------------------------
# Project Makefile for wdb's feltLoad
#
# This file is processed by automake to generate the non-recursive
# project Makefile
#-----------------------------------------------------------------------------


# Scanned by Autoreconf; must be static (no expansion)
ACLOCAL_AMFLAGS =               -I config/m4
                                
# Share directory organization
wdbdatadir =                    $(datadir)/wdb
sqldir =                        $(wdbdatadir)/sql
wdblibdir =                     $(libdir)/wdb/

stationloadbindir= $(bindir)
#dist_stationloadbin_SCRIPTS= stationLoad

etcstationloaddir = /etc
dist_etcstationload_DATA =

# Setting Targets
bin_PROGRAMS = stationLoad

# This is to be replaced by a "find wdb" macro:
AM_CXXFLAGS = -DSYSCONFDIR=\"$(pkgsysconfdir)\" $(wdb_CFLAGS)
        
stationLoad_LDADD = $(wdb_LIBS) -lwdbLoad -lwdbMath -lwdbExcept -lwdbLog -lwdbMath

CLEANFILES =
EXTRA_DIST =
include src/sources.mk
include doc/doc.mk
include etc/etc.mk
include config/mk/debian.mk
