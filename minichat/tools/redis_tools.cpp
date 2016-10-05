
#include "common/redis_client_factory.h"
#include "r3c/r3c.h"

#include "phxrpc/file.h"

#include <vector>
#include <string>

int flushall( phxrpc::OptMap & opt_map )
{
    if( ! opt_map.Has( 'c' ) ) return -1;

    RedisClientFactory factory( opt_map.Get( 'c' ) );

    std::vector<std::pair<std::string, std::string> > result;
    factory.Get().flushall( &result );

    printf( "Result %zu\n", result.size() );
    for( auto & i : result ) {
        printf( "%s, %s\n", i.first.c_str(), i.second.c_str() );
    }

    return 0;
}

void ShowUsage( const char * program )
{
    printf( "\nUsage: %s [-f <func>] [-c <config>]\n", program );
    printf( "\n" );
    printf( "\tfunc: flushall\n" );
    printf( "\n" );

    exit( -1 );
}

int main( int argc, char * argv[] )
{
    phxrpc::OptMap opt_map( "f:c:v" );

    if( ! opt_map.Parse( argc, argv ) ) ShowUsage( argv[0] );

    if( ( ! opt_map.Has( 'f' ) ) || opt_map.Has( 'v' ) ) ShowUsage( argv[0] );

    const char * func = opt_map.Get( 'f' );

    int ret = -1;

    if( 0 == strcasecmp( func, "flushall" ) ) {
        ret = flushall( opt_map );
    }

    if( 0 != ret ) ShowUsage( argv[0] );

    return 0;
}

