
#include "redis_client_factory.h"

#include "phxrpc/rpc.h"
#include "phxrpc/file.h"

#include "r3c/r3c.h"

RedisClientFactory :: RedisClientFactory( const char * config_file )
{
    char path[ 1024 ] = { 0 };
    if( '~' == config_file[0] ) {
        snprintf( path, sizeof( path ), "%s%s", getenv( "HOME" ), config_file + 1 );
    } else {
        snprintf( path, sizeof( path ), "%s", config_file );
    }

    phxrpc::log( LOG_DEBUG, "read config %s", path );

    phxrpc::ClientConfig config;

    if( config.Read( path ) ) {
        char buff[ 128 ] = { 0 };

        for( size_t i = 0; ; i++ ) {
            const phxrpc::Endpoint_t * ep = config.GetByIndex( i );

            if( NULL == ep ) break;
            snprintf( buff, sizeof( buff ), "%s:%d", ep->ip, ep->port );

            if( i > 0 ) nodes.append( "," );
            nodes.append( buff );
        }
    }
}

RedisClientFactory :: ~RedisClientFactory()
{
}

r3c::CRedisClient & RedisClientFactory :: Get()
{
    static __thread r3c::CRedisClient * client = NULL;
    
    if( NULL == client ) client = new r3c::CRedisClient( nodes );

    return * client;
}

