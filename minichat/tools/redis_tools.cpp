
#include "r3c/r3c.h"

#include "phxrpc/file.h"
#include "phxrpc/network.h"

#include "common/redis_client_config.h"

#include <vector>
#include <string>

int flushall( phxrpc::OptMap & opt_map )
{
    if( ! opt_map.Has( 'c' ) ) return -1;

    RedisClientConfig config;
    config.Read( opt_map.Get( 'c' ) );

    for( int i = 0; i < config.GetCount(); i++ ) {
        const phxrpc::Endpoint_t * ep = config.GetByIndex( i );

        char line[ 128 ] = { 0 };
        int k = 0;
        for( k = 0; k < 3; k++ ) {
            phxrpc::BlockTcpStream socket;
            bool open_ret = phxrpc::BlockTcpUtils::Open( &socket, ep->ip, ep->port,
                    config.GetConnectTimeoutMS(), NULL, 0 );
            if( ! open_ret ) continue;
            socket << "flushall" << std::endl;
            if( ! socket.flush().good() ) continue;
            if( socket.getlineWithTrimRight(line, sizeof(line)).good() ) break;
        }
        if( k < 3 ) {
            printf( "%s:%d [%s]\n", ep->ip, ep->port, line );
        } else {
            printf( "%s:%d fail\n", ep->ip, ep->port );
        }
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

