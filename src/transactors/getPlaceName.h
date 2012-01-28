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


#ifndef GETPLACENAME_H_
#define GETPLACENAME_H_

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
    class GetPlaceName : public pqxx::transactor<>
    {
    public:
        /**
         * Default Constructor
         * @param 	placeName	placeName of the new/existing point
         * @param 	geometry	place geometry as WKT
         */
        GetPlaceName(const std::string& placename = "", const std::string& validtime = "") :
            pqxx::transactor<>("GetPlaceName"),
            placeName_(placename),
            validtime_(validtime)
        {
            // NOOP
        }

        /**
         * Functor. The transactors functor executes the query.
         */
        void operator()(argument_type &T)
        {
            std::ostringstream query;
            query << "SELECT * wci.getPlaceName"<<"("<<"\'"<< placeName_ <<"\'"<< "," <<  "\'"<< validtime_<<"\'"<< " )";
            R = T.exec(query.str());
            WDB_LOG & log = WDB_LOG::getInstance("wdb.load.getplacename");
            log.infoStream() << query.str();
        }

        /**
         * Commit handler. This is called if the transaction succeeds.
         */
        void on_commit()
        {
            WDB_LOG & log = WDB_LOG::getInstance("wdb.load.getplacename");
            log.infoStream() << "wdb.load.getplacename call complete";
        }

        /**
         * Abort handler. This is called if the transaction fails.
         * @param	Reason	The reason for the abort of the call
         */
        void on_abort(const char Reason[]) throw ()
        {
            WDB_LOG & log = WDB_LOG::getInstance("wdb.load.getplacename");
            log.errorStream() << "Transaction " << Name() << " failed " << Reason;
        }

        /**
         * Special error handler. This is called if the transaction fails with an unexpected error.
         * Read the libpqxx documentation on transactors for details.
         */
        void on_doubt() throw ()
        {
            WDB_LOG & log = WDB_LOG::getInstance("wdb.load.getplacename");
            log.errorStream() << "Transaction " << Name() << " in indeterminate state";
        }

    protected:
        /// The result returned by the query
        pqxx::result R;
        /// Place Name
        std::string placeName_;
        /// Valid Time
        std::string validtime_;
    };

} } /* end namespaces */

//" SELECT tb1.placeid id, tb1.placename AS name, tb1.originalsrid srid, ST_AsText(tb1.placegeometry) wkt, tb2.placenamevalidfrom AS validfrom, tb2.placenamevalidto AS validto "
//" FROM wci.getPlacePoint(NULL) tb1 INNER JOIN (select * from wci.getPlaceName(NULL, NULL)) tb2 ON (tb1.placeid = tb2.placeid);"

//" SELECT tb1.placeid, tb1.placename, tb1.originalsrid, ST_AsText(tb1.placegeometry) FROM wci.getPlacePoint('%') tb1 INNER JOIN (SELECT * FROM wci.getPlaceName(NULL, NULL)) tb2 ON (tb1.palceid = tb2.placeid);
#endif /*GETPLACENAME_H_*/
