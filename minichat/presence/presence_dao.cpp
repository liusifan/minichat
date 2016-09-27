
#include "presence_dao.h"

#include "r3c/r3c.h"

#include "phxrpc/file.h"

#include <random>     // random_device, uniform_int_distribution
#include <sstream>

PresenceDAO :: PresenceDAO( r3c::CRedisClient & client )
    : client_( client )
{
}

PresenceDAO :: ~PresenceDAO()
{
}

int PresenceDAO :: Create( const char * username, presence::Session * session )
{
    char key[ 128 ] = { 0 };

    snprintf( key, sizeof( key ), "session_%s", username );

    std::random_device rd;
    std::stringstream fmt;
    fmt << rd() << rd();

    session->set_username( username );
    session->set_session_key( fmt.str() );
    session->set_createtime( time( NULL ) );
    session->set_updatetime( time( NULL ) );

    fmt << rd() << rd();
    session->set_auto_auth_aes_key( fmt.str() );

    std::string buff;
    session->SerializeToString( &buff );

    client_.set( key, buff );

    return 0;
}

int PresenceDAO :: Get( const char * username, presence::Session * resp )
{
    char key[ 128 ] = { 0 };

    snprintf( key, sizeof( key ), "session_%s", username );

    std::string buff;

    bool ret = client_.get( key, &buff );

    if( ret ) resp->ParseFromString( buff );

    return ret ? 0 : -1;
}

int PresenceDAO :: Remove( const char * username, presence::Session * resp )
{
    char key[ 128 ] = { 0 };

    snprintf( key, sizeof( key ), "session_%s", username );

    std::string buff;

    bool ret = client_.get( key, &buff );

    if( ret ) resp->ParseFromString( buff );

    client_.del( key );

    return ret ? 0 : -1;
}

