#pragma once

#include "profile.pb.h"

namespace r3c {
  class CRedisClient;
};

class ProfileDAO {
public:
    ProfileDAO( r3c::CRedisClient & client );
    ~ProfileDAO();

    int Set( const profile::Setting & req,
        google::protobuf::Empty * resp );

    int Get( const google::protobuf::StringValue & req,
        profile::Setting * resp );

private:
    r3c::CRedisClient & client_;
};

