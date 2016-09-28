
#include "redis_client_ex.h"

#include "r3c/r3c.h"

RedisClientEx :: RedisClientEx( r3c::CRedisClient & client )
    : client_( client )
{
}

RedisClientEx :: ~RedisClientEx()
{
}

bool RedisClientEx :: RedisCommand( const char * key, const char * cmd,
        const std::string & value, const char * excepted_status )
{
    bool is_ok = false;

    std::string key_str( key );

    const char * argv[3] = { cmd, key, value.c_str() };
    size_t argvlen[3] = { strlen( cmd ), strlen( key ), value.size() };

    int argc = value.size() > 0 ? 3 : 2;

    const redisReply * reply = client_.redis_command( REDIS_REPLY_STATUS, NULL,
            &key_str, cmd, argc, argv, argvlen );

    if( NULL != reply ) {
        is_ok = ( 0 == strcmp( reply->str, excepted_status ) );
        freeReplyObject( (redisReply*)reply );
    }

    return is_ok;
}

bool RedisClientEx :: RedisMulti( const char * key )
{
    bool is_ok = false;

    std::string key_str( key );
    std::string cmd_str_tmp( "MULTI" );

    const redisReply * reply = client_.redis_command( REDIS_REPLY_STATUS, NULL, &key_str, "MULTI", cmd_str_tmp );

    if( NULL != reply ) {
        is_ok = ( 0 == strcmp( reply->str, "OK" ) );
        freeReplyObject( (redisReply*)reply );
    }

    return is_ok;
}

bool RedisClientEx :: RedisExec( const char * key )
{
    bool is_ok = false;

    std::string key_str( key );
    std::string cmd_str_tmp( "EXEC" );

    const redisReply * reply = client_.redis_command( REDIS_REPLY_ARRAY, NULL, &key_str, "EXEC", cmd_str_tmp );

    if( NULL != reply ) {
        is_ok = ( reply->elements > 0 );
        freeReplyObject( (redisReply*)reply );
    }

    return is_ok;
}

