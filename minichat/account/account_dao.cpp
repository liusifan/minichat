
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

    int ret = client_.setnx( key, buff );

    return ret == 1 ? 0 : -2;
}

int AccountDAO :: SetPwd( const account::PwdReq & req )
{
    char key[ 128 ] = { 0 };

    snprintf( key, sizeof( key ), "pwd_md5_%s", req.username().c_str() );

    client_.set( key, req.pwd_md5() );

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

int AccountDAO :: Auth( const account::PwdReq & req )
{
    char key[ 128 ] = { 0 };

    snprintf( key, sizeof( key ), "pwd_md5_%s", req.username().c_str() );

    std::string buff;

    int ret = -1;

    if( client_.get( key, &buff ) ) {
        if( 0 == strcasecmp( req.pwd_md5().c_str(), buff.c_str() ) ) {
            ret = 0;
        } else {
            ret = -99;
        }
    }

    printf( "DEBUG: Auth %d\n", ret );

    return ret;
}

