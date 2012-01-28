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


#ifndef GETALLWDBSTATIONS_H_
#define GETALLWDBSTATIONS_H_

#include "../WDBDatabaseConnection.h"
#include "../STLoaderConfiguration.h"
#include "../STInfosysDatabaseConnection.h"
#include "wciTransactors.h"

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
    class GetAllWDBStations : public pqxx::transactor<>
    {
    public:
        /**
         * Default Constructor
         */
        GetAllWDBStations(std::map<std::string, WDBStationRecord>& out)
            : pqxx::transactor<>("GetAllSTIStations"), out_(out)
        {
            // NOOP
        }

        /**
         * Functor. The transactors functor executes the query.
         */
        void operator()(argument_type &T)
        {
            std::string query =
                    " SELECT tb1.placeid id, tb1.placename AS name, tb1.originalsrid srid, ST_AsText(tb1.placegeometry) wkt, "
                    " tb2.placenamevalidfrom AS validfrom, tb2.placenamevalidto AS validto "
                    " FROM wci.getPlacePoint(NULL) tb1 INNER JOIN (select * from wci.getPlaceName(NULL, NULL)) AS tb2 "
                    " ON (tb1.placeid = tb2.placeid);";

            R_ = T.exec(query);
            std::cerr << query << std::endl;
            std::cerr << " R size: " << R_.size()<< std::endl;
        }

        /**
         * Commit handler. This is called if the transaction succeeds.
         */
        void on_commit()
        {
            out_.clear();
            size_t rCount = R_.size();
            for(size_t r = 0; r < rCount; ++r) {
                WDBStationRecord rec;
                rec.id_   = R_[r][0].as<std::string>();
                rec.name_ = R_[r][1].as<std::string>();
                rec.srid_ = R_[r][2].as<std::string>();
                rec.wkt_  = R_[r][3].as<std::string>();
                rec.from_ = R_[r][4].as<std::string>();
                rec.to_   = R_[r][5].as<std::string>();

                if(out_.count(rec.id_) != 0) {
                    std::cerr << "already have entry with STATIONID: " << rec.id_ <<std::endl;
                }

    //            std::cerr
    //                    << rec.id_ <<" | "<< rec.name_ << " | " << rec.srid_<< " | "
    //                    << rec.wkt_ << "|"<< rec.from_ << " | " << rec.to_
    //                    << std::endl;

                out_.insert(std::make_pair<std::string, WDBStationRecord>(rec.name_, rec));
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
        /// The result returned by the query
        pqxx::result R_;
        ///
        std::map<std::string, WDBStationRecord>& out_;
    };

} } /* end namespaces */


#endif /*GETALLWDBSTATIONS_H_*/
