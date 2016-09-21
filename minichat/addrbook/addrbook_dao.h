#pragma once

#include "addrbook.pb.h"

namespace r3c {
  class CRedisClient;
};

class AddrbookDAO {
public:
    AddrbookDAO( r3c::CRedisClient & client );
    ~AddrbookDAO();

    int Set( const addrbook::ContactReq & req,
        google::protobuf::Empty * resp );

    int GetAll( const google::protobuf::StringValue & req,
        addrbook::ContactList * resp );

    int GetOne( const addrbook::GetOneReq & req,
        addrbook::Contact * resp );

    int GetBySeq( const addrbook::GetBySeqReq & req,
        addrbook::ContactList * resp );

private:
    r3c::CRedisClient & client_;
};

