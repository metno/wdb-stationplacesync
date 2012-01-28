/*
 wdb

 Copyright (C) 2008 met.no

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

#include "STInfosysDatabaseConnection.h"

// WDB
//
#include <wdbLogHandler.h>

// PQXX
//
#include <pqxx/pqxx>

// STD
//
#include <map>
#include <string>
#include <stdexcept>
#include <iostream>

using namespace std;
using namespace pqxx;
using namespace pqxx::prepare;

namespace wdb { namespace load {

    STInfosysDatabaseConnection::STInfosysDatabaseConnection(const STLoaderConfiguration & config)
        : pqxx::connection(config.loading().pqDatabaseConnection()), config_(new STLoaderConfiguration(config))
    {
        setup_();
    }

    STInfosysDatabaseConnection::~STInfosysDatabaseConnection()
    {
        delete config_;
    }

    void STInfosysDatabaseConnection::setup_()
    {
        // NOOOP
    }

    void STInfosysDatabaseConnection::getAllStations(std::map<std::string, STIStationRecord>& result)
    {
        WDB_LOG & log = WDB_LOG::getInstance("wdb.load.stidatabaseconnection");

        // Create a transaction.
        pqxx::work transaction(*this);
        // This is the read query
        std::string query =
                          " SELECT st1.stationid, st1.name, st1.lon, st1.lat, st1.wmono, st1.fromtime, st1.totime FROM station st1 INNER JOIN (SELECT stationid, MAX(edited_at) AS last_updated, MAX(fromtime) AS fromtime FROM station WHERE(lat IS NOT NULL AND lon IS NOT NULL) GROUP BY stationid) st2 ON (st1.stationid = st2.stationid AND st1.edited_at = st2.last_updated AND st1.fromtime = st2.fromtime) WHERE (st1.lon IS NOT NULL AND st1.lat IS NOT NULL);";

        pqxx::result rows = transaction.exec(query);
        size_t rCount = rows.size();
        for(size_t r = 0; r < rCount; ++r) {
            STIStationRecord rec;
            rec.id_   = rows[r][0].as<std::string>();
            rec.name_ = rows[r][1].as<std::string>();
            rec.lon_  = rows[r][2].as<float>();
            rec.lat_  = rows[r][3].as<float>();
            rec.wmo_  = rows[r][4].is_null() ? std::string() : rows[r][4].as<std::string>();

            assert(!rows[r][5].is_null());
            rec.from_ = rows[r][5].as<std::string>()+std::string("+00");

            rec.to_   = rows[r][6].is_null() ? std::string("infinity") : rows[r][6].as<std::string>()+std::string("+00");

            if(result.count(rec.id_) != 0)
                std::cout << "EXISTS STATIONID: " << rec.id_<<std::endl;

            result.insert(std::make_pair<std::string, STIStationRecord>(rec.id_, rec));
        }

        log.debugStream() << "result size: "<< rCount;
    }

} } /* end namespaces */

//SELECT st1.stationid, st1.name, st1.lon, st1.lat, st1.wmono, st1.fromtime, st1.totime FROM station st1 INNER JOIN (SELECT stationid, MAX(edited_at) AS last_updated, MAX(fromtime) AS fromtime FROM station WHERE(lat IS NOT NULL AND lon IS NOT NULL) GROUP BY stationid) st2 ON (st1.stationid = st2.stationid AND st1.edited_at = st2.last_updated AND st1.fromtime = st2.fromtime) WHERE (st1.lon IS NOT NULL AND st1.lat IS NOT NULL);
