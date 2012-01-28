/*
    wdb - weather and water data storage

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


#ifndef GETALLSTISTATIONS_H_
#define GETALLSTISTATIONS_H_

#include "../WDBDatabaseConnection.h"
#include "../STLoaderConfiguration.h"

// WDB
//
#include <wdbException.h>
#include <wdbLogHandler.h>

// PQXX
//
#include <pqxx/transactor>
#include <pqxx/result>

// STD
//
#include <iostream>
#include <string>
#include <sstream>

namespace wdb { namespace load {

    /**
     * Transactor to create a user
     */
    class GetAllSTIStations : public pqxx::transactor<>
    {
    public:
        /**
         * Default Constructor
         */
        GetAllSTIStations(const std::string transactorname = "GetAllSTIStations")
            : pqxx::transactor<>(transactorname)
        {
            // NOOP
        }

        /**
         * Functor. The transactors functor executes the query.
         */
        void operator()(argument_type &T)
        {
            std::string query =
                              " SELECT st1.stationid, st1.name, st1.lon, st1.lat, st1.wmono FROM station st1 "
                              " INNER JOIN "
                              " (SELECT stationid, MAX(edited_at) AS last_update_time FROM station GROUP BY stationid) stfiltered "
                              " ON (st1.stationid = stfiltered.stationid AND st1.edited_at = stfiltered.last_update_time) "
                              " WHERE (st1.lon IS NOT NULL AND st1.lat IS NOT NULL AND st1.name IS NOT NULL AND st1.wmono IS NOT NULL AND st1.totime IS NULL) ORDER BY st1.name; ";
            R_ = T.exec(query);
            WDB_LOG & log = WDB_LOG::getInstance("wdb.load.getallstistations");
            std::cerr << query << std::endl;
            std::cerr << " R size: " << R_.size()<< std::endl;

//            for(size_t r = 0; r < R_.size(); ++r) {
//                STIStationRecord rec;
//                rec.id_   = R_[r][0].as<std::string>();
//                rec.name_ = R_[r][1].as<std::string>();
//                rec.lon_  = R_[r][2].as<float>();
//                rec.lat_  = R_[r][3].as<float>();
//                rec.wmo_  = R_[r][4].as<int>();

//                std::cerr<< rec.id_<<" | "<< rec.name_<< " | " <<std::endl;
//            }
        }

        void getStations(std::map<std::string, STIStationRecord>& result)
        {
            WDB_LOG & log = WDB_LOG::getInstance("wdb.load.wdbdatabaseconnection");

            std::cerr << " R size: " << R_.size()<< std::endl;

            for(size_t r = 0; r < R_.size(); ++r) {
                STIStationRecord rec;
                rec.id_   = R_[r][0].as<std::string>();
                rec.name_ = R_[r][1].as<std::string>();
                rec.lon_  = R_[r][2].as<float>();
                rec.lat_  = R_[r][3].as<float>();
                rec.wmo_  = R_[r][4].as<int>();

                if(result.count(rec.id_) != 0) {
                    log.debugStream() << "already have entry with STATIONID: " << rec.id_;
                }

                std::cerr<< rec.id_<<" | "<< rec.name_<< " | " <<std::endl;

                result.insert(std::make_pair<std::string, STIStationRecord>(rec.name_, rec));
            }
        }

        /**
         * Commit handler. This is called if the transaction succeeds.
         */
        void on_commit()
        {
            std::cerr<<__FUNCTION__<<" R size: " << R_.size()<< std::endl;
            WDB_LOG & log = WDB_LOG::getInstance("wdb.load.getallstistations");
            log.infoStream() << "wdb.load.getallstistations";
        }

        /**
         * Abort handler. This is called if the transaction fails.
         * @param	Reason	The reason for the abort of the call
         */
        void on_abort(const char Reason[]) throw ()
        {
            std::cerr<<__FUNCTION__<<" R size: " << R_.size()<< std::endl;
            WDB_LOG & log = WDB_LOG::getInstance("wdb.load.getallstistations");
            log.errorStream() << "Transaction " << Name() << " failed " << Reason;
        }

        /**
         * Special error handler. This is called if the transaction fails with an unexpected error.
         * Read the libpqxx documentation on transactors for details.
         */
        void on_doubt() throw ()
        {
            std::cerr<<__FUNCTION__<<" R size: " << R_.size()<< std::endl;
            WDB_LOG & log = WDB_LOG::getInstance("wdb.load.getallstistations");
            log.errorStream() << "Transaction " << Name() << " in indeterminate state";
        }

    protected:
        /// The result returned by the query
        pqxx::result R_;
    };

} } /* end namespaces */


#endif /*ADDPLACEPOINT_H_*/
