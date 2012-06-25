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


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "STLoaderConfiguration.h"

// BOOST
//
#include <boost/program_options.hpp>

using namespace std;
using namespace boost::program_options;
using namespace wdb::load;

// Support Functions
namespace
{

/**
 * Define the input options for the Loader
 * @param	out		Reference to option structure
 */
options_description
getInput(STLoaderConfiguration::InputOptions & out)
{
    options_description input( "Input" );
    input.add_options()
    ( "name", value( & out.file ), "Name of file to process" )
    ;

        return input;
}

/**
 * Define the output options for the Loader
 * @param	out		Reference to option structure
 */
options_description getOutput(STLoaderConfiguration::OutputOptions & out)
{
    options_description output( "Output" );
    output.add_options()
       ( "dry-run", bool_switch( & out.dry_run ), "Just show the SQL commands that would be sent to WDB database" )
    ;

    return output;
}

/**
 * Define the general options for the Loader
 * @param	out		Reference to option structure
 */
options_description
getLoading( STLoaderConfiguration::STLoadingOptions & out, const std::string & defaultDataProvider )
{
    out.defaultDataProvider = defaultDataProvider;
    options_description loading("Loading");
    loading.add_options()
//    ( "load_wmono", bool_switch( & out.load_wmono_ )->default_value( false ), "Load into WMO namespace" )
//    ( "load_stationid", bool_switch( & out.load_stationid_ )->default_value( false ), "Load into STATIONID namespace" )
        ( "stdatabase", value( & out.stdatabase ), "Specify stations database name" )
        ( "sthost", value( & out.sthost ), "Specify stations database host" )
        ( "stuser", value( & out.stuser ), "Specify stations database user" )
        ( "stpass", value( & out.stpass ), "Specify stations database password" )
        ( "stport", value( & out.stport )->default_value( 5432 ), "Specify stations database port (default 5432)" )
		( "cns-namespace", value( & out.cnsNamespace )->default_value( 88000 ), "Specify stationid namespace (default 88000)" )
		( "wmo-namespace", value( & out.wmoNamespace )->default_value( 88001 ), "Specify WMO namespace (default 88001)" )
//        ( "after", value( & out.stupdatedafter ), "Specify date of last update for stations (=> after)" )
//        ( "before", value( & out.stupdatedbefore ), "Specify date of last update for stations (=< before)" )
//        ( "limit", value( & out.stlimit ), "Specify the limit on stations to be updated (=< limit)" )
    ;

    return loading;
}

} // namespace support functions


STLoaderConfiguration::STLoaderConfiguration(const std::string & defaultDataProvider)
        : defaultDataProvider_(defaultDataProvider)
{
        cmdOptions().add( getInput( input_ ) );
        cmdOptions().add( getOutput( output_ ) );
        configOptions().add( getLoading( loading_, defaultDataProvider_ ) );

        shownOptions().add( getOutput( output_ ) );
        shownOptions().add( getLoading( loading_, defaultDataProvider_  ) );

        positionalOptions().add( "name", -1 );
}

STLoaderConfiguration::~STLoaderConfiguration()
{
    // NOOP
}

void STLoaderConfiguration::parse_( int argc, char ** argv )
{
        options_description opt;
        opt.add( cmdOptions() ).add( configOptions() );

        store( command_line_parser( argc, argv ).
                                options( opt ).
                                positional( positionalOptions_ ).
                                run(),
                        givenOptions_ );
}

string STLoaderConfiguration::STLoadingOptions::pqDatabaseConnection() const
{
        ostringstream ret;
        ret << "dbname=" << stdatabase.c_str() << " ";
        if(sthost != "")
            ret << "host=" << sthost.c_str() << " ";
        ret << "port=" << stport << " ";
        if(stuser != "")
            ret << "user=" << stuser.c_str() << " ";
        if(stpass != "")
            ret << "password=" << stpass.c_str() << " ";
        return ret.str();
}

string STLoaderConfiguration::STLoadingOptions::psqlDatabaseConnection() const
{
        ostringstream ret;
        ret << "-d" << stdatabase;
        if (!sthost.empty())
            ret << " -h" << sthost;
        ret << " -p" << stport;
        ret << " -U" << stuser;
        return ret.str();
}
