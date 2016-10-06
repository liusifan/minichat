
#pragma once

#include <string>

namespace r3c {
    class CRedisClient;
};

class RedisClientFactory {
public:
    static RedisClientFactory * GetDefault();

public:
    RedisClientFactory();
    ~RedisClientFactory();

    r3c::CRedisClient & Get();

private:
    std::string nodes_;
};

