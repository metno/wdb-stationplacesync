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
#include "../STInfosysDatabaseConnection.h"
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
        GetAllSTIStations(std::map<std::string, STIStationRecord>& out, const std::string& edited_after = std::string(), const std::string& edited_before = std::string(), const std::string& limit = std::string())
            : pqxx::transactor<>("GetAllSTIStations"), out_(out), edited_after_(edited_after), edited_before_(edited_before), limit_(limit)
        {
            // NOOP
        }

        /**
         * Functor. The transactors functor executes the query.
         */
        void operator()(argument_type &T)
        {
            std::string query =
                    " SELECT st1.stationid, st1.name, st1.lon, st1.lat, st1.wmono, st1.fromtime, st1.totime FROM station st1 "
                    " INNER JOIN (SELECT stationid, MAX(edited_at) AS last_updated, MAX(fromtime) AS fromtime FROM station WHERE(lat IS NOT NULL AND lon IS NOT NULL) GROUP BY stationid) st2 "
                    " ON (st1.stationid = st2.stationid AND st1.edited_at = st2.last_updated AND st1.fromtime = st2.fromtime) WHERE st1.lon IS NOT NULL AND st1.lat IS NOT NULL ";

            if(!edited_after_.empty())
                query.append(" AND st1.edited_at >= ").append("'").append(edited_after_).append("'");

            if(!edited_before_.empty())
                query.append(" AND st1.edited_at <= ").append("'").append(edited_before_).append("'");

            if(!limit_.empty())
                query.append(" LIMIT ").append("'").append(limit_).append("'");

            R_ = T.exec(query);
//            WDB_LOG & log = WDB_LOG::getInstance("wdb.load.getallstistations");
//            std::cerr << query << std::endl;
//            std::cerr << " R size: " << R_.size()<< std::endl;
        }

        /**
         * Commit handler. This is called if the transaction succeeds.
         */
        void on_commit()
        {
            out_.clear();
            size_t rCount = R_.size();
            for(size_t r = 0; r < rCount; ++r) {
                STIStationRecord rec;
                rec.id_   = R_[r][0].as<std::string>();
                rec.name_ = R_[r][1].as<std::string>();
                rec.lon_  = R_[r][2].as<float>();
                rec.lat_  = R_[r][3].as<float>();
                rec.wmo_  = R_[r][4].is_null() ? std::string() : R_[r][4].as<std::string>();

                assert(!R_[r][5].is_null());
                rec.from_ = R_[r][5].as<std::string>()+std::string("+00");

                rec.to_   = R_[r][6].is_null() ? std::string("infinity") : R_[r][6].as<std::string>()+std::string("+00");

                if(out_.count(rec.id_) != 0)
                    std::cout << "EXISTS STATIONID: " << rec.id_<<std::endl;

                out_.insert(std::make_pair<std::string, STIStationRecord>(rec.id_, rec));
            }
            WDB_LOG & log = WDB_LOG::getInstance("wdb.load.getallstistations");
            log.infoStream() << "wdb.load.getallstistations";
        }

        /**
         * Abort handler. This is called if the transaction fails.
         * @param	Reason	The reason for the abort of the call
         */
        void on_abort(const char Reason[]) throw ()
        {
//            std::cerr<<__FUNCTION__<<" R size: " << R_.size()<< std::endl;
            WDB_LOG & log = WDB_LOG::getInstance("wdb.load.getallstistations");
            log.errorStream() << "Transaction " << Name() << " failed " << Reason;
        }

        /**
         * Special error handler. This is called if the transaction fails with an unexpected error.
         * Read the libpqxx documentation on transactors for details.
         */
        void on_doubt() throw ()
        {
//            std::cerr<<__FUNCTION__<<" R size: " << R_.size()<< std::endl;
            WDB_LOG & log = WDB_LOG::getInstance("wdb.load.getallstistations");
            log.errorStream() << "Transaction " << Name() << " in indeterminate state";
        }

    protected:
        /// The result returned by the query
        pqxx::result R_;
        ///
        std::map<std::string, STIStationRecord>& out_;
        ///
        std::string edited_after_;
        std::string edited_before_;
        std::string limit_;
    };

} } /* end namespaces */


#endif /*GETALLSTISTATIONS_H_*/
