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

#include <pqxx/connection>
#include <string>
#include <limits>

namespace wdb { namespace load {

    class WDBDatabaseConnection : public pqxx::connection
    {
    public:
        explicit WDBDatabaseConnection(const STLoaderConfiguration& configuration);
        virtual ~WDBDatabaseConnection();

        void getAllStations(std::map<std::string, WDBStationRecord>& stations_by_id, std::map<std::string, WDBStationRecord>& stations_by_wmono);
        void updateStations(std::map<std::string, STIStationRecord>& stations_by_id);

    private:
        void setup_();
        const STLoaderConfiguration* config_;
    };

} } /* namespaces */

#endif /* WDBDATABASECONNECTION_H_ */
