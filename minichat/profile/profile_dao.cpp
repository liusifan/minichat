
#include "profile_dao.h"

#include "r3c/r3c.h"

#include "phxrpc/file.h"

ProfileDAO :: ProfileDAO( r3c::CRedisClient & client )
    : client_( client )
{
}

ProfileDAO :: ~ProfileDAO()
{
}

int ProfileDAO :: Set( const profile::Setting & req,
    google::protobuf::Empty * resp )
{
    char key[ 128 ] = { 0 };

    snprintf( key, sizeof( key ), "profile_%s", req.username().c_str() );

    std::string buff;
    req.SerializeToString( &buff );

    client_.set( key, buff );

    return 0;
}

int ProfileDAO :: Get( const google::protobuf::StringValue & req,
    profile::Setting * resp )
{
    char key[ 128 ] = { 0 };

    snprintf( key, sizeof( key ), "profile_%s", req.value().c_str() );

    std::string buff;

    bool ret = client_.get( key, &buff );

    if( ret ) resp->ParseFromString( buff );

    return ret ? 0 : -1;
}

