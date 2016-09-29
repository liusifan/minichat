
#pragma once

#include <string>

namespace r3c {
    class CRedisClient;
};

class RedisClientFactory {
public:
    RedisClientFactory( const char * config_file );
    ~RedisClientFactory();

    r3c::CRedisClient & Get();

private:
    std::string nodes_;
};

