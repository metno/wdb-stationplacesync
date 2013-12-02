wdb_stationplacesync_SOURCES = \
	src/main.cpp \
        src/DBStructs.h \
        src/STLoaderConfiguration.h \
	src/STLoaderConfiguration.cpp \
        src/STInfosysDatabaseConnection.h \
	src/STInfosysDatabaseConnection.cpp \
	src/WDBDatabaseConnection.h \
	src/WDBDatabaseConnection.cpp \
	src/transactors/addOrUpdatePlacePoint.h \
	src/transactors/getAllSTIStations.h \
	src/transactors/wciTransactors.h

EXTRA_DIST += src/sources.mk
