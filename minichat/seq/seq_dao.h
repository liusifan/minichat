#pragma once

#include "seq.pb.h"

namespace r3c {
  class CRedisClient;
};

class SeqDAO {
public:
    SeqDAO( r3c::CRedisClient & client );
    ~SeqDAO();

    int Alloc( const seq::AllocReq & req,
        google::protobuf::UInt32Value * resp );

    int Get( const char * username, seq::SyncKey * resp );

private:
    r3c::CRedisClient & client_;
};

