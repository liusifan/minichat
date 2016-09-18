
#pragma once

#include "Account.pb.h"

namespace r3c {
  class CRedisClient;
};

class AccountDAO {
public:
    AccountDAO( r3c::CRedisClient & client );
    ~AccountDAO();

    int Add( const account::User & req );

    int Get( const char * username, account::User * resp );

    int Auth( const account::AuthReq & req );

private:
    r3c::CRedisClient & client_;
};

