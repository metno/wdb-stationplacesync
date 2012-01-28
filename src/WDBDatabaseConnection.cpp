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
//#include "transactors/addPlacePoint.h"
#include "transactors/updatePlacePoint.h"
#include "transactors/wciTransactors.h"
#include "transactors/getAllSTIStations.h"

// WDB
//
#include <wdbMath.h>
#include <wdbMathC.h>
#include <wdb/wdbSetup.h>
#include <wdb/LoaderConfiguration.h>

// GEOS
//
#include <geos_c.h>
#include <cstdarg>

// PQXX
//
#include <pqxx/pqxx>

// BOOST
//
#include <boost/lexical_cast.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

// STD
//
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


    WDBDatabaseConnection::WDBDatabaseConnection(const STLoaderConfiguration & config)
        : pqxx::connection(config.database().pqDatabaseConnection()), config_(new STLoaderConfiguration(config))
    {

        std::cerr<<__FUNCTION__<<" : "<<"stationidNameSpace : "<<config.loading().stationidNameSpace<<std::endl;
        std::cerr<<__FUNCTION__<<" : "<<"wmonoNameSpace : "    <<config.loading().wmonoNameSpace<<std::endl;

//        if(config.loading().stationidNameSpace.empty())
//            perform(WciBegin((config.database().user)));
//        else if(config.loading().stationidNameSpace == "test")
//            perform(WciBegin(config.database().user, 999, 999, 999));
//        else if(config.loading().stationidNameSpace == "default")
//            perform(WciBegin(config.database().user, 0, 0, 0));
//        else
//            throw std::logic_error("Unknown name space specification: " + config.loading().stationidNameSpace);

        setup_();
    }

    WDBDatabaseConnection::~WDBDatabaseConnection()
    {
//        perform(WciEnd(), 1);

        delete config_;
    }

    void WDBDatabaseConnection::setup_()
    {
        // NOOP
    }

    void WDBDatabaseConnection::getAllStations(std::map<std::string, wdb::load::WDBStationRecord>& stations_by_id, std::map<std::string, wdb::load::WDBStationRecord>& stations_by_wmono)
    {
        WDB_LOG & log = WDB_LOG::getInstance("wdb.load.wdbdatabaseconnection");

        // get stations from stationid
        {
            perform(WciBegin((config_->database().user)));

            // Create a transaction.
            pqxx::work transaction(*this);

            // This is the read query
            std::string query =
                    " SELECT tb1.placeid id, tb1.placename AS name, tb1.originalsrid srid, ST_AsText(tb1.placegeometry) wkt, "
                    " tb2.placenamevalidfrom AS validfrom, tb2.placenamevalidto AS validto "
                    " FROM wci.getPlacePoint(NULL) tb1 INNER JOIN (select * from wci.getPlaceName(NULL, NULL)) AS tb2 "
                    " ON (tb1.placeid = tb2.placeid);";

            pqxx::result rows = transaction.exec(query);
            size_t rCount = rows.size();
            for(size_t r = 0; r < rCount; ++r) {
                WDBStationRecord rec;
                rec.id_   = rows[r][0].as<std::string>();
                rec.name_ = rows[r][1].as<std::string>();
                rec.srid_ = rows[r][2].as<std::string>();
                rec.wkt_  = rows[r][3].as<std::string>();
                rec.from_ = rows[r][4].as<std::string>();
                rec.to_   = rows[r][5].as<std::string>();

                if(stations_by_id.count(rec.id_) != 0) {
                    log.debugStream() << "already have entry with STATIONID: " << rec.id_;
                }

    //            std::cerr
    //                    << rec.id_ <<" | "<< rec.name_ << " | " << rec.srid_<< " | "
    //                    << rec.wkt_ << "|"<< rec.from_ << " | " << rec.to_
    //                    << std::endl;

                stations_by_id.insert(std::make_pair<std::string, WDBStationRecord>(rec.name_, rec));
            }

            transaction.commit();

            std::cerr << "# rows by STATIONID: "<< rCount << std::endl;

            perform(WciEnd(), 1);
        }

        // get stations from wmono
        {
            perform(WciBegin((config_->database().user), 0, 4365, 0));

            // Create a transaction.
            pqxx::work transaction(*this);

            // This is the read query
            std::string query =
                    " SELECT tb1.placeid id, tb1.placename AS name, tb1.originalsrid srid, ST_AsText(tb1.placegeometry) wkt, "
                    " tb2.placenamevalidfrom AS validfrom, tb2.placenamevalidto AS validto "
                    " FROM wci.getPlacePoint(NULL) tb1 INNER JOIN (select * from wci.getPlaceName(NULL, NULL)) AS tb2 "
                    " ON (tb1.placeid = tb2.placeid);";

            pqxx::result rows = transaction.exec(query);
            size_t rCount = rows.size();
            for(size_t r = 0; r < rCount; ++r) {
                WDBStationRecord rec;
                rec.id_   = rows[r][0].as<std::string>();
                rec.name_ = rows[r][1].as<std::string>();
                rec.srid_ = rows[r][2].as<std::string>();
                rec.wkt_  = rows[r][3].as<std::string>();
                rec.from_ = rows[r][4].as<std::string>();
                rec.to_   = rows[r][5].as<std::string>();

                if(stations_by_wmono.count(rec.id_) != 0) {
                    log.debugStream() << "already have entry with WMONO: " << rec.id_;
                }

    //            std::cerr
    //                    << rec.id_ <<" | "<< rec.name_ << " | " << rec.srid_<< " | "
    //                    << rec.wkt_ << "|"<< rec.from_ << " | " << rec.to_
    //                    << std::endl;

                stations_by_wmono.insert(std::make_pair<std::string, WDBStationRecord>(rec.name_, rec));
            }

            transaction.commit();

            std::cerr << "# rows by WMONO : "<< rCount << std::endl;

            perform(WciEnd(), 1);
        }
    }

    void WDBDatabaseConnection::updateStations(std::map<std::string, STIStationRecord>& sti_stations)
    {
        WDB_LOG & log = WDB_LOG::getInstance("wdb.load.wdbdatabaseconnection");

        std::map<std::string, WDBStationRecord> stations_by_id;
        std::map<std::string, WDBStationRecord> stations_by_wmono;
        getAllStations(stations_by_id, stations_by_wmono);

        initGEOS(notice, notice);

        { /// update STATIONID namespace

            perform(WciBegin((config_->database().user)));

            std::map<std::string, STIStationRecord>::const_iterator cit;
            for(cit = sti_stations.begin(); cit != sti_stations.end(); ++cit) {
                STIStationRecord sti_st = cit->second;
                if(stations_by_id.find(sti_st.id_) == stations_by_id.end()) {
                    std::string wkt("POINT(");
                    wkt.append(boost::lexical_cast<std::string>(wdb::round(sti_st.lon_, 4))).append(" ").append(boost::lexical_cast<std::string>(wdb::round(sti_st.lat_, 4))).append(")");
//                    if(config_->output().list)
//                        std::cerr<<"ADD statioinid: "<<sti_st.id_<<" WKT: "<<UpdatePlacePoint(sti_st.id_, wkt, sti_st.from_, sti_st.to_).toString()<<std::endl;
//                    else
//                        perform(UpdatePlacePoint(sti_st.id_, wkt, sti_st.from_, sti_st.to_));
                } else {
                    // check if station param have been changed
                    WDBStationRecord wdb_st = stations_by_id[sti_st.id_];
                    GEOSGeometry* g = GEOSGeomFromWKT(wdb_st.wkt_.c_str());
                    assert(g != 0);
                    assert(GEOSGeomTypeId(g) == GEOS_POINT);

                    GEOSCoordSeq coords = const_cast<GEOSCoordSeq>(GEOSGeom_getCoordSeq(g));
                    assert(coords);

                    double wdb_lon; GEOSCoordSeq_getX(coords, 0, &wdb_lon);
                    double wdb_lat; GEOSCoordSeq_getY(coords, 0, &wdb_lat);

                    if(   static_cast<int>(wdb::round(wdb_lon, 4)*10000) == static_cast<int>(wdb::round(sti_st.lon_, 4)*10000)
                          && static_cast<int>(wdb::round(wdb_lat, 4)*10000) == static_cast<int>(wdb::round(sti_st.lat_, 4)*10000)
                          && wdb_st.from_ == sti_st.from_
                          && wdb_st.to_ == sti_st.to_)
                    {
//                        std::cerr<<"NOT CHANGED statioinid: "<<sti_st.id_<<" WKT: "<<GEOSGeomToWKT(g)<<std::endl;
                    }
                    else
                    {
                        std::string wkt("POINT(");
                        wkt.append(boost::lexical_cast<std::string>(wdb::round(sti_st.lon_, 4))).append(" ").append(boost::lexical_cast<std::string>(wdb::round(sti_st.lat_, 4))).append(")");
//                        if(config_->output().list)
//                            std::cerr<<"UPDATE statioinid: "<<sti_st.id_<<" WKT: "<<UpdatePlacePoint(sti_st.id_, wkt, sti_st.from_, sti_st.to_).toString()<<std::endl;
//                        else
//                            perform(UpdatePlacePoint(sti_st.id_, wkt, sti_st.from_, sti_st.to_));
                    }

                    GEOSGeom_destroy(g);
                }
            }

            perform(WciEnd(), 1);

        }

        { /// update WMONO namespace

            perform(WciBegin((config_->database().user), 0, 4365, 0));

            std::map<std::string, STIStationRecord>::const_iterator cit;
            for(cit = sti_stations.begin(); cit != sti_stations.end(); ++cit) {
                STIStationRecord sti_st = cit->second;

                if(sti_st.wmo_.empty()) {
                    continue;
                }

                if(stations_by_wmono.find(sti_st.wmo_) == stations_by_wmono.end()) {
                    std::string wkt("POINT(");
                    wkt.append(boost::lexical_cast<std::string>(wdb::round(sti_st.lon_, 4))).append(" ").append(boost::lexical_cast<std::string>(wdb::round(sti_st.lat_, 4))).append(")");
//                    if(config_->output().list)
//                        std::cerr<<"ADD wmono: "<<sti_st.wmo_<<" WKT: "<<UpdatePlacePoint(sti_st.wmo_, wkt, sti_st.from_, sti_st.to_).toString()<<std::endl;
//                    else
//                        perform(UpdatePlacePoint(sti_st.wmo_, wkt, sti_st.from_, sti_st.to_));

                } else {
                    // check if station param have been changed
                    WDBStationRecord wdb_st = stations_by_wmono[sti_st.wmo_];
                    GEOSGeometry* g = GEOSGeomFromWKT(wdb_st.wkt_.c_str());
                    assert(g != 0);
                    assert(GEOSGeomTypeId(g) == GEOS_POINT);

                    GEOSCoordSeq coords = const_cast<GEOSCoordSeq>(GEOSGeom_getCoordSeq(g));
                    assert(coords);

                    double wdb_lon; GEOSCoordSeq_getX(coords, 0, &wdb_lon);
                    double wdb_lat; GEOSCoordSeq_getY(coords, 0, &wdb_lat);

                    if(   static_cast<int>(wdb::round(wdb_lon, 4)*10000) == static_cast<int>(wdb::round(sti_st.lon_, 4)*10000)
                          && static_cast<int>(wdb::round(wdb_lat, 4)*10000) == static_cast<int>(wdb::round(sti_st.lat_, 4)*10000)
                          && wdb_st.from_ == sti_st.from_
                          && wdb_st.to_ == sti_st.to_)
                    {
//                        std::cerr<<"NOT CHANGED WMONO: "<<sti_st.wmo_<<" WKT: "<<GEOSGeomToWKT(g)<<std::endl;
                    }
                    else
                    {
                        std::string wkt("POINT(");
                        wkt.append(boost::lexical_cast<std::string>(wdb::round(sti_st.lon_, 4))).append(" ").append(boost::lexical_cast<std::string>(wdb::round(sti_st.lat_, 4))).append(")");
//                        if(config_->output().list)
//                            std::cerr<<"UPDATE WMONO: "<<sti_st.wmo_<<" WKT: "<<UpdatePlacePoint(sti_st.wmo_, wkt, sti_st.from_, sti_st.to_).toString()<<std::endl;
//                        else
//                            perform(UpdatePlacePoint(sti_st.wmo_, wkt, sti_st.from_, sti_st.to_));
                    }

                    GEOSGeom_destroy(g);
                }
            }

            perform(WciEnd(), 1);

        }
    }
} } /* end namespaces */
