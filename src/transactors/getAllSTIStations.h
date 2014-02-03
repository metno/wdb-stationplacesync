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

#include "../DBStructs.h"
#include "../STLoaderConfiguration.h"

// WDB
//
#include <wdbException.h>
#include <wdbLogHandler.h>

// PQXX
//
#include <pqxx/transactor>
#include <pqxx/result>

// std
#include <string>
#include <vector>
#include <sstream>

using namespace std;

namespace wdb { namespace load {

    class GetAllSTIStations : public pqxx::transactor<>
    {
    public:
        /**
         * Default Constructor
         */
        GetAllSTIStations(vector<STIStationRecord>& out,
        		const std::string& edited_after, const std::string & earliest )
            : pqxx::transactor<>("GetAllSTIStations"),
              out_(out),
              edited_after_(edited_after),
              earliest_(earliest)
        {
            // NOOP
        }

        /**
         * Functor. The transactors functor executes the query.
         */
        void operator()(argument_type &T)
        {
            std::ostringstream query;

            query << "SELECT " << STIStationRecord::selectWhat;
            query << " FROM station st1";
            query << " WHERE st1.lon IS NOT NULL AND st1.lat IS NOT NULL";

            if ( not earliest_.empty() )
            	query << " AND st1.totime > " << T.esc(earliest_);

            if(!edited_after_.empty())
                query << " AND st1.edited_at >= " << T.esc(edited_after_);

            query << " ORDER BY st1.edited_at DESC ";


            WDB_LOG & log = WDB_LOG::getInstance("wdb.load.getallstistations");
            log.debugStream() <<__FUNCTION__<< " @ line["<< __LINE__ << "] " << " query == " << query;
            R_ = T.exec(query.str());
            log.debugStream() <<__FUNCTION__<< " @ line["<< __LINE__ << "] " << " R_.size() == " << R_.size();
        }

        /**
         * Commit handler. This is called if the transaction succeeds.
         */
        void on_commit()
        {
            out_.clear();
            size_t rCount = R_.size();
            for(size_t r = 0; r < rCount; ++r) {
                STIStationRecord rec(R_[r]);
                out_.push_back(rec);
            }
            WDB_LOG & log = WDB_LOG::getInstance("wdb.load.getallstistations");
            log.debugStream() <<__FUNCTION__<< " @ line["<< __LINE__ << "] " << " out_.size() == " << out_.size();
        }

        /**
         * Abort handler. This is called if the transaction fails.
         * @param	Reason	The reason for the abort of the call
         */
        void on_abort(const char Reason[]) throw ()
        {
            WDB_LOG & log = WDB_LOG::getInstance("wdb.load.getallstistations");
            log.errorStream() <<__FUNCTION__<< " @ line["<< __LINE__ << "] " << "Transaction " << Name() << " failed " << Reason;
        }

        /**
         * Special error handler. This is called if the transaction fails with an unexpected error.
         * Read the libpqxx documentation on transactors for details.
         */
        void on_doubt() throw ()
        {
            WDB_LOG & log = WDB_LOG::getInstance("wdb.load.getallstistations");
            log.debugStream() <<__FUNCTION__<< " @ line["<< __LINE__ << "] " << "Transaction " << Name() << " in indeterminate state";
        }

    protected:
        /// The result returned by the query
        pqxx::result R_;

        vector<STIStationRecord>& out_;

        std::string edited_after_;
        std::string earliest_;
    };

} } /* end namespaces */

//SELECT st1.stationid, st1.name, st1.lon, st1.lat, st1.wmono, st1.fromtime, st1.totime FROM station st1  JOIN (SELECT stationid, MAX(fromtime) AS maxtime FROM station WHERE(lat IS NOT NULL AND lon IS NOT NULL) group by stationid) st2 ON (st1.stationid = st2.stationid AND st1.fromtime >= st2.maxtime) WHERE st1.lon IS NOT NULL AND st1.lat IS NOT NULL;
#endif /*GETALLSTISTATIONS_H_*/
