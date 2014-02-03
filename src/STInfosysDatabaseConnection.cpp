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
#include "transactors/getAllSTIStations.h"
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

namespace wdb {
namespace load {

    STInfosysDatabaseConnection::STInfosysDatabaseConnection(const STLoaderConfiguration & config)
        : pqxx::connection(config.stinfosys().pqDatabaseConnection())
    {
    }

    STInfosysDatabaseConnection::~STInfosysDatabaseConnection()
    {
    }

    void STInfosysDatabaseConnection::getAllStations(std::vector<STIStationRecord>& stations, const std::string& edited_after, const std::string & earliestValidTime)
    {
        WDB_LOG & log = WDB_LOG::getInstance("wdb.load.stidatabaseconnection");
        perform(GetAllSTIStations(stations, edited_after, earliestValidTime));
        log.infoStream() <<__FUNCTION__<< " @ line["<< __LINE__ << "] " << "# rows by STI: "<< stations.size();
    }

}
}
