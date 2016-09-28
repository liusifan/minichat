
#pragma once

#include <string>

namespace r3c {
    class CRedisClient;
};

class RedisClientEx {
public:
    RedisClientEx( r3c::CRedisClient & client );
    ~RedisClientEx();

    bool RedisCommand( const char * key, const char * cmd,
            const char * cmd_line, const char * excepted_status );

    bool RedisBinCommand( const char * key, const char * cmd,
            const std::string & value, const char * excepted_status );

    bool RedisMulti( const char * key );

    bool RedisExec( const char * key );

private:
    r3c::CRedisClient & client_;

};

