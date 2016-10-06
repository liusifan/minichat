
#include "redis_client_factory.h"
#include "redis_client_config.h"

#include "phxrpc/rpc.h"
#include "phxrpc/file.h"

#include "r3c/r3c.h"

#include "define.h"

void redis_log_error( const char * fmt, ... )
{
    va_list args;
    va_start(args, fmt);
    phxrpc::vlog( LOG_ERR, fmt, args );
    va_end(args);
}

void redis_log_info( const char * fmt, ... )
{
    va_list args;
    va_start(args, fmt);
    phxrpc::vlog( LOG_INFO, fmt, args );
    va_end(args);
}

void redis_log_debug( const char * fmt, ... )
{
    va_list args;
    va_start(args, fmt);
    phxrpc::vlog( LOG_INFO, fmt, args );
    va_end(args);
}



RedisClientFactory :: RedisClientFactory()
{
    r3c::set_error_log_write( redis_log_error );
    r3c::set_info_log_write( redis_log_info );
    r3c::set_debug_log_write( redis_log_debug );

    const char * config_file = MINI_REDIS_CLIENT_CONFIG;

    char path[ 1024 ] = { 0 };
    if( '~' == config_file[0] ) {
        snprintf( path, sizeof( path ), "%s%s", getenv( "HOME" ), config_file + 1 );
    } else {
        snprintf( path, sizeof( path ), "%s", config_file );
    }

    phxrpc::log( LOG_INFO, "read config %s", path );

    RedisClientConfig config;

    if( config.Read( path ) ) {
        nodes_ = config.GetNodes();
        phxrpc::log( LOG_INFO, "nodes: %s", nodes_.c_str() );
    } else {
        phxrpc::log( LOG_ERR, "read config %s failed", path );
    }
}

RedisClientFactory :: ~RedisClientFactory()
{
}

RedisClientFactory * RedisClientFactory :: GetDefault()
{
    static RedisClientFactory factory;
    return &factory;
}

r3c::CRedisClient & RedisClientFactory :: Get()
{
    static __thread r3c::CRedisClient * client = NULL;
    
    if( NULL == client ) client = new r3c::CRedisClient( nodes_ );

    return * client;
}

