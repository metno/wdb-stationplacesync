/*
 wdb

 Copyright (C) 2012 met.no

 Contact information:
 Norwegian Meteorological Institute
 Box 43 Blindern
 0313 OSLO
 NORWAY
 E-mail: wdb@met.no

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 MA  02110-1301, USA
 */

#include "WDBDatabaseConnection.h"
#include "STLoaderConfiguration.h"
#include "transactors/addOrUpdatePlacePoint.h"
#include "transactors/wciTransactors.h"
#include "transactors/getAllSTIStations.h"
//#include "transactors/getAllWDBStations.h"

// WDB
#include <wdbMath.h>
#include <wdbMathC.h>
#include <wdb/wdbSetup.h>
#include <wdb/LoaderConfiguration.h>

// GEOS
#include <geos_c.h>
#include <cstdarg>

// PQXX
#include <pqxx/pqxx>

// BOOST
#include <boost/lexical_cast.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

// STD
#include <cassert>
#include <map>
#include <string>
#include <stdexcept>
#include <iostream>

using namespace std;
using namespace pqxx;
using namespace pqxx::prepare;

namespace wdb { namespace load {

    static void notice(const char *fmt, ...)
    {
        std::fprintf(stderr, "GEOS: ");

        va_list ap;
        va_start(ap, fmt);
        std::vfprintf(stderr, fmt, ap);
        va_end(ap);

        std::fprintf(stderr, "\n");
    }


    WDBDatabaseConnection::WDBDatabaseConnection(const STLoaderConfiguration & config, WdbLogHandler & logHandler)
        : pqxx::connection(config.database().pqDatabaseConnection()), config_(config), logHandler_(logHandler)
    {
        setup_();
    }

    WDBDatabaseConnection::~WDBDatabaseConnection()
    {
        unprepare("UpdatePlacePoint");
    }

    void WDBDatabaseConnection::setup_()
    {
        prepare("UpdatePlacePoint",
                        "SELECT "
                        "wci.addOrUpdatePlacePoint ("
                        "$1::text,"
                "ST_GeomFromText("
                        "$2::text,"
                        "4030"
                "),"
                        "$3::timestamp with time zone,"
                        "$4::timestamp with time zone"
                        ")" )
                        ("varchar", treat_direct )
                        ("varchar", treat_direct )
                        ("varchar", treat_direct )
                        ("varchar", treat_direct );
    }


    void WDBDatabaseConnection::updateStations(std::vector<STIStationRecord>& sti_stations)
    {
        WDB_LOG & log = WDB_LOG::getInstance("wdb.load.wdbdatabaseconnection");
        log.debugStream() <<__FUNCTION__<< " @ line["<< __LINE__ << "] CHECK ";

        initGEOS(notice, notice);

        updateCnsNames_(sti_stations);
        updateWmoNames_(sti_stations);
    }

    void WDBDatabaseConnection::updateCnsNames_(const std::vector<STIStationRecord>& sti_stations)
    {
        WDB_LOG & log = WDB_LOG::getInstance("wdb.load.wdbdatabaseconnection.cns");

        perform(WciBegin(config_.database().user, 88, config_.loading().cnsNamespace, 88));

        std::vector<STIStationRecord>::const_iterator cit;
        for(cit = sti_stations.begin(); cit != sti_stations.end(); ++cit) {

            const STIStationRecord& sti_st = *cit;

            std::string wkt = sti_st.wkt();

            log.debugStream() <<__FUNCTION__<< " @ line["<< __LINE__ << "] " << "ADD/UPDATE statioinid: "<< sti_st.id << " WKT: " << wkt;

            if(!config_.output().dry_run) {
                try {
                    perform(AddOrUpdatePlacePoint(sti_st.id, wkt, sti_st.from, sti_st.to));
                } catch (const std::exception& e) {
                    log.errorStream() <<__FUNCTION__<< " @ line["<< __LINE__ << "] " << e.what();
                }
            }
        }

        perform(WciEnd(), 1);
    }

    void WDBDatabaseConnection::updateWmoNames_(const std::vector<STIStationRecord>& sti_stations)
    {
        WDB_LOG & log = WDB_LOG::getInstance("wdb.load.wdbdatabaseconnection.wmo");

        perform(WciBegin((config_.database().user), 88, config_.loading().wmoNamespace, 88));

        std::vector<STIStationRecord>::const_iterator cit;
        for(cit = sti_stations.begin(); cit != sti_stations.end(); ++cit) {

            const STIStationRecord& sti_st = *cit;

            if(sti_st.wmo.empty()) {
                continue;
            }

            std::string wkt = sti_st.wkt();

            log.debugStream() <<__FUNCTION__<< " @ line["<< __LINE__ << "] " << "ADD/UPDATE wmono: "<<sti_st.wmo<<" WKT: " <<wkt;

            if(!config_.output().dry_run) {
                try {
                    perform(AddOrUpdatePlacePoint(sti_st.wmo, wkt, sti_st.from, sti_st.to));
                } catch(const std::exception& e) {
                    log.errorStream() <<__FUNCTION__<< " @ line["<< __LINE__ << "] " << e.what();
                }
            }
        }

        perform(WciEnd(), 1);
    }


}

} /* end namespaces */
