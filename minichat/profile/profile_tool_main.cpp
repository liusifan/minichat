/* profile_tool_main.cpp

 Generated by phxrpc_pb2tool from profile.proto

*/

#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "phxrpc_profile_tool.h"
#include "profile_tool_impl.h"

#include "profile_client.h"

#include "phxrpc/file.h"

using namespace phxrpc;

void showUsage( const char * program )
{
    printf( "\nUsage: %s [-c <config>] [-f <func>] [-v]\n", program );

    ProfileTool::Name2Func_t * name2func = ProfileTool::GetName2Func();

    for( int i = 0; ; i++ ) {
        ProfileTool::Name2Func_t * iter = &( name2func[i] );

        if( NULL == iter->name ) break;

        printf( "    -f %s %s\n", iter->name, iter->usage );
    }
    printf( "\n" );
    exit( 0 );
}

int main( int argc, char * argv[] )
{
    const char * func = NULL;
    const char * config = NULL;

    for( int i = 1; i < argc - 1; i++ ) {
        if( 0 == strcmp( argv[i], "-c" ) ) {
            config = argv[ ++i ];
        }
        if( 0 == strcmp( argv[i], "-f" ) ) {
            func = argv[ ++i ];
        }
        if( 0 == strcmp( argv[i], "-v" ) ) {
            showUsage( argv[0] );
        }
    }

    if( NULL == func ) showUsage( argv[0] );

    if( NULL != config ) ProfileClient::Init( config );

    ProfileTool::Name2Func_t * target = NULL;

    ProfileTool::Name2Func_t * name2func = ProfileTool::GetName2Func();

    for( int i = 0; i < 100; i++ ) {
        ProfileTool::Name2Func_t * iter = &( name2func[i] );

        if( NULL == iter->name ) break;

        if( 0 == strcasecmp( func, iter->name ) ) {
            target = iter;
            break;
        }
    }

    if( NULL == target ) showUsage( argv[0] );

    OptMap opt_map( target->opt_string );

    if( ! opt_map.Parse( argc, argv ) ) showUsage( argv[0] );

    ProfileTool::ToolFunc_t targefunc = target->func;

    ProfileToolImpl tool;

    if( 0 != ( tool.*targefunc ) ( opt_map ) ) showUsage( argv[0] );

    return 0;
}