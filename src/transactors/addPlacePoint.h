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


#ifndef ADDPLACEPOINT_H_
#define ADDPLACEPOINT_H_

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
    class AddPlacePoint : public pqxx::transactor<>
    {
    public:
        /**
         * Default Constructor
         * @param 	placeName	placeName of the new/existing point
         * @param 	geometry	place geometry as WKT
         */

        AddPlacePoint(const std::string& placename, const std::string& geometry,
                      const std::string& validfrom, const std::string& validto = "infinity",
                      const std::string& transactorname = "AddPlacePoint")
            : pqxx::transactor<>(transactorname),
              placeName_(placename), geometry_(geometry),
              validfrom_(validfrom), validto_(validto)
        {
//            std::ostringstream query;
//            query << "SELECT wci.addOrUpdatePlacePoint"<<"("<<"\'"<< placeName_ <<"\'"<< "," << "ST_GeomFromText("<< "\'"<< geometry_<<"\'"<<", 4030 )" << "," << "\'" << validfrom_ << "\'" << "," << "\'" << validto_ << "\'" << ");";
//            query_ = query.str();
        }

        /**
          * Functor. The transactors functor executes the query.
          */
        void operator()(argument_type &T)
        {
            R = T.prepared("UpdatePlacePoint")
                                             (placeName_)
                                             (geometry_)
                                             (validfrom_)
                                             (validto_).exec();
//            R = T.exec(query_);
//            WDB_LOG & log = WDB_LOG::getInstance("wdb.load.addplacepoint");
//            std::cerr << query_ << std::endl;
        }

        /**
          * Commit handler. This is called if the transaction succeeds.
          */
        void on_commit()
        {
//            WDB_LOG & log = WDB_LOG::getInstance("wdb.load.addplacepoint");
//            log.infoStream() << "wci.addplacepoint call complete";
        }

        /**
          * Abort handler. This is called if the transaction fails.
          * @param	Reason	The reason for the abort of the call
          */
        void on_abort(const char Reason[]) throw ()
        {
//            WDB_LOG & log = WDB_LOG::getInstance("wdb.load.addplacepoint");
//            log.errorStream() << "Transaction " << Name() << " failed " << Reason;
        }

        /**
          * Special error handler. This is called if the transaction fails with an unexpected error.
          * Read the libpqxx documentation on transactors for details.
          */
        void on_doubt() throw ()
        {
//            WDB_LOG & log = WDB_LOG::getInstance("wdb.load.addplacepoint");
//            log.errorStream() << "Transaction " << Name() << " in indeterminate state";
        }

        std::string toString() const
        {
            return query_;
        }

    protected:
        /// The result returned by the query
        pqxx::result R;
        /// Place Name
        std::string placeName_;
        /// Place Geometry - WKT
        std::string geometry_;
        /// Valid From time
        std::string validfrom_;
        /// Valid To time
        std::string validto_;
        /// query text - used in toString()
        std::string query_;
    };

} } /* end namespaces */


#endif /*ADDPLACEPOINT_H_*/
