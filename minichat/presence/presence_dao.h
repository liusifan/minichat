
#pragma once

#include "presence.pb.h"

namespace r3c {
  class CRedisClient;
};

class PresenceDAO {
public:
    PresenceDAO( r3c::CRedisClient & client );
    ~PresenceDAO();

    int Create( const char * username, presence::Session * session );

    int Get( const char * username, presence::Session * session );

    int Remove( const char * username, presence::Session * session );

private:
    r3c::CRedisClient & client_;
};

