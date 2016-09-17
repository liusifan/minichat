
#include "redis_utils.h"

#include "r3c/r3c.h"

#include "phxrpc/rpc.h"

r3c::CRedisClient * RedisUtils :: CreateClient( const char * config_file )
{
    r3c::CRedisClient * ret = NULL;

    phxrpc::ClientConfig config;

    if( config.Read( config_file ) ) {

        std::string nodes;
        char buff[ 128 ] = { 0 };

        for( size_t i = 0; ; i++ ) {
            const phxrpc::Endpoint_t * ep = config.GetByIndex( i );

            if( NULL == ep ) break;
            snprintf( buff, sizeof( buff ), "%s:%d", ep->ip, ep->port );

            if( i > 0 ) nodes.append( "," );
            nodes.append( buff );
        }

        ret = new r3c::CRedisClient( nodes, config.GetConnectTimeoutMS(),
                                    config.GetSocketTimeoutMS() );
    }

    return ret;
}

