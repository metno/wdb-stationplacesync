/*
    wdb-stationplacesync

    Copyright (C) 2014 met.no

    Contact information:
    Norwegian Meteorological Institute
    Box 43 Blindern
    0313 OSLO
    NORWAY
    E-mail: post@met.no

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


#include "DBStructs.h"
#include <wdbMath.h>

namespace wdb {
namespace load {


const std::string STIStationRecord::selectWhat = "stationid, name, lon, lat, wmono, fromtime AT TIME ZONE 'UTC' AS fromtime, totime AT TIME ZONE 'UTC' AS totime";

STIStationRecord::STIStationRecord(const pqxx::result::tuple & databaseRow)
{
    id   = databaseRow[0].as<std::string>();
    name = databaseRow[1].as<std::string>();
    lon  = databaseRow[2].as<float>();
    lat  = databaseRow[3].as<float>();
    wmo  = databaseRow[4].is_null() ? std::string() : databaseRow[4].as<std::string>();

    from = databaseRow[5].as<std::string>();
    to   = databaseRow[6].is_null() ? std::string("infinity") : databaseRow[6].as<std::string>();

	if(from.find("+00") == std::string::npos)
		from += "+00";

	if(to.find("infinity") == std::string::npos && to.find("+00") == std::string::npos)
		to += "+00";
}

std::string STIStationRecord::wkt() const
{
    std::string wkt("POINT(");
    wkt.append(boost::lexical_cast<std::string>(wdb::round(lon, 4))).append(" ").append(boost::lexical_cast<std::string>(wdb::round(lat, 4))).append(")");
    return wkt;
}

}
}
