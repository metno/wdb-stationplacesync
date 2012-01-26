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
#include <algorithm>

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
        GetAllSTIStations(std::map<std::string, STIStationRecord>& out, const std::string transactorname = "GetAllSTIStations")
            : pqxx::transactor<>(transactorname), out_(out)
        {
            // NOOP
        }

        /**
         * Functor. The transactors functor executes the query.
         */
        void operator()(argument_type &T)
        {
            std::string query =
                    " SELECT st1.stationid, st1.name, st1.lon, st1.lat, st1.fromtime, st1.totime FROM station st1 "
                    " INNER JOIN "
                    " (SELECT stationid, MAX(edited_at) AS updated, MAX(fromtime) AS fromtime FROM station WHERE (lat IS NOT NULL AND lon IS NOT NULL) GROUP BY stationid) AS st2 "
                    " ON (st1.stationid = st2.stationid AND st1.edited_at = st2.updated AND st1.fromtime = st2.fromtime) WHERE (st1.lon IS NOT NULL AND st1.lat IS NOT NULL);" ;

            R_ = T.exec(query);

            WDB_LOG & log = WDB_LOG::getInstance("wdb.load.getallstistations");
            std::cerr << query << std::endl;
        }

        void getStations()
        {
            WDB_LOG & log = WDB_LOG::getInstance("wdb.load.wdbdatabaseconnection");
            std::cerr<<__FUNCTION__<<" stations_ size: " << stations_.size()<< std::endl;
        }

        /**
         * Commit handler. This is called if the transaction succeeds.
         */
        void on_commit()
        {
            WDB_LOG & log = WDB_LOG::getInstance("wdb.load.getallstistations");
            if ( R_.size() == 0 ) {
                log.debugStream() << "R_  empty";
            } else {
                std::cerr<<__FUNCTION__<<" R_ size: " << R_.size()<< std::endl;

                for(size_t r = 0; r < R_.size(); ++r) {
                    STIStationRecord rec;
                    rec.id_   = R_[r][0].as<std::string>();
                    rec.name_ = R_[r][1].as<std::string>();
                    rec.lon_  = R_[r][2].as<float>();
                    rec.lat_  = R_[r][3].as<float>();
//                    rec.wmo_  = R_[r][4].is_null() ? std::string() : R_[r][4].as<std::string>();
                    rec.from_ = R_[r][4].is_null() ? std::string() : R_[r][4].as<std::string>()+"+00";           // adding time zone for later comparison
                    rec.to_   = R_[r][5].is_null() ? std::string("infinity") : R_[r][5].as<std::string>()+"+00"; // adding time zone for later comparison

                    if(stations_.count(rec.id_) != 0) {
                        std::cerr << "already have entry with STATIONID: " << rec.id_ << std::endl;
                    }

//                    std::cerr
//                            << __FUNCTION__ << " "
//                            << rec.id_ <<" | "<< rec.name_ << " | " << rec.lon_<< " | "
//                            << rec.lat_ << "|"<< rec.from_ << " | " << rec.to_
//                            << std::endl;

                    stations_.insert(std::make_pair<std::string, STIStationRecord>(rec.id_, rec));
                }

                stations_.swap(out_);

                std::cerr<<__FUNCTION__<<" out_ size: " << out_.size()<< std::endl;
            }
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
        pqxx::result R_;
        std::map<std::string, STIStationRecord>& out_;
        std::map<std::string, STIStationRecord>  stations_;

    };

} } /* end namespaces */

//SELECT st1.stationid, st1.name, st1.lon, st1.lat, st1.wmono, st1.fromtime, st1.totime, st1.edited_at AS updated FROM station st1 INNER JOIN (SELECT stationid, MAX(edited_at) AS updated, MAX(fromtime) AS fromtime FROM station WHERE (lat IS NOT NULL AND lon IS NOT NULL) GROUP BY stationid) AS st2 ON (st1.stationid = st2.stationid AND st1.edited_at = st2.updated AND st1.fromtime = st2.fromtime) WHERE (st1.lon IS NOT NULL AND st1.lat IS NOT NULL);
#endif /*ADDPLACEPOINT_H_*/
