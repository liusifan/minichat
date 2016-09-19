
#pragma once

#include "account.pb.h"

namespace r3c {
  class CRedisClient;
};

class AccountDAO {
public:
    AccountDAO( r3c::CRedisClient & client );
    ~AccountDAO();

    int Add( const account::User & req );

    int SetPwd( const account::PwdReq & req );

    int Get( const char * username, account::User * resp );

    int Auth( const account::PwdReq & req );

private:
    r3c::CRedisClient & client_;
};

