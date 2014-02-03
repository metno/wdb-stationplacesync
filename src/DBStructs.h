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

#ifndef DBSTRUCTS_H_
#define DBSTRUCTS_H_

#include "STLoaderConfiguration.h"

#include <pqxx/result.hxx>
#include <string>

namespace wdb {
namespace load {

    struct STIStationRecord
    {
    	STIStationRecord(const pqxx::result::tuple & databaseRow);

    	/**
    	 * Get the position's Well-Known-Text (wkt) representation
    	 */
    	std::string wkt() const;

        std::string id;
        std::string name;
        float       lon;
        float       lat;
        std::string wmo;
        std::string from;
        std::string to;

        /**
         * The where clause that is needed to use this constructor
         */
        static const std::string selectWhat;
    };

} } // end namespaces

#endif // DBSTRUCTS_H_
