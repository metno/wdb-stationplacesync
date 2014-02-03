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


#ifndef STLOADERCONFIGURATION_H_
#define STLOADERCONFIGURATION_H_

#include "DBStructs.h"

// PROJECT INCLUDES
#include <wdbConfiguration.h>

// SYSTEM INCLUDES
#include <string>

using namespace std;

namespace wdb { namespace load {

    /**
      * STLoaderConfiguration handles the options
      * of the station laoder application.
      *
      * @see WdbConfiguration
      */
class STLoaderConfiguration : public WdbConfiguration
{
public:
        STLoaderConfiguration();
        ~STLoaderConfiguration();

        struct OutputOptions
        {
            bool dry_run;
        };

        struct StinfosysOptions
        {
            std::string stdatabase; /// < station database name.
            std::string sthost; /// < station database host.
            std::string stuser; /// < station database user.
            std::string stpass; /// < station database password.
            int         stport; /// < station database port.

            std::string stupdatedafter; /// look at stations that have been updated (edited_at) after given date

            /**
              * Generate and return a pq compatible database connection string using
              * the options given for the program.
              */
            std::string pqDatabaseConnection() const;
        };

        struct LoadingOptions
        {
            int cnsNamespace;
            int wmoNamespace;

            std::string earliestValidTime;
        };

        const OutputOptions & output() const { return output_; }
        const StinfosysOptions & stinfosys() const { return stinfosys_; }
        const LoadingOptions & loading() const { return loading_; }

protected:
        OutputOptions output_;
        StinfosysOptions stinfosys_;
        LoadingOptions loading_;
};

} }

#endif /*STLOADERCONFIGURATION_H_*/
