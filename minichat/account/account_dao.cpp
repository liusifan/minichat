
#include "account_dao.h"

#include "r3c/r3c.h"

#include "phxrpc/file.h"

AccountDAO :: AccountDAO( r3c::CRedisClient & client )
    : client_( client )
{
}

AccountDAO :: ~AccountDAO()
{
}

int AccountDAO :: Add( const account::User & req )
{
    char key[ 128 ] = { 0 };

    snprintf( key, sizeof( key ), "acct_%s", req.username().c_str() );

    std::string buff;
    req.SerializeToString( &buff );

    client_.set( key, buff );

    return 0;
}

int AccountDAO :: Get( const char * username, account::User * resp )
{
    char key[ 128 ] = { 0 };

    snprintf( key, sizeof( key ), "acct_%s", username );

    std::string buff;

    bool ret = client_.get( key, &buff );

    if( ret ) resp->ParseFromString( buff );

    return ret ? 0 : -1;
}

int AccountDAO :: Auth( const account::AuthReq & req )
{
    account::User user;

    int ret = Get( req.username().c_str(), &user );

    if( 0 == ret ) {
        if( 0 != strcasecmp( req.pwd_md5().c_str(), user.pwd_md5().c_str() ) ) {
            ret = -99;
        }
    }

    return ret;
}

