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

#ifndef WDBDATABASECONNECTION_H_
#define WDBDATABASECONNECTION_H_


#include "DBStructs.h"

// wdb
#include <wdbLogHandler.h>

// libpqxx
#include <pqxx/connection>

// std
#include <string>
#include <limits>

namespace wdb { namespace load {

    class WDBDatabaseConnection : public pqxx::connection
    {
    public:
        explicit WDBDatabaseConnection(const STLoaderConfiguration& configuration, WdbLogHandler & logHandler);
        virtual ~WDBDatabaseConnection();

        void updateStations(std::vector<STIStationRecord>& stations_by_id);

    private:
        void setup_();

        void updateCnsNames_(const std::vector<STIStationRecord> & sti_stations);
        void updateWmoNames_(const std::vector<STIStationRecord> & sti_stations);

        const STLoaderConfiguration & config_;
        wdb::WdbLogHandler & logHandler_;
    };

} } /* namespaces */

#endif /* WDBDATABASECONNECTION_H_ */
