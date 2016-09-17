
#pragma once

namespace r3c {
    class CRedisClient;
};

class RedisUtils {
 public:
  static r3c::CRedisClient * CreateClient( const char * config_file );
};

