
#include "addrbook_dao.h"

#include "r3c/r3c.h"

#include "addrbook.pb.h"

AddrbookDAO :: AddrbookDAO( r3c::CRedisClient & client )
    : client_( client )
{
}

AddrbookDAO :: ~AddrbookDAO()
{
}

int AddrbookDAO :: Add( const addrbook::ContactReq & req,
    google::protobuf::Empty * resp )
{
    char key[ 128 ] = { 0 };
    snprintf( key, sizeof( key ), "addrbook_%s", req.username().c_str() );

    std::string value;
    req.contact().SerializeToString( &value );

    client_.hset( key, req.contact().username(), value, NULL );

    return 0;
}

int AddrbookDAO :: GetAll( const google::protobuf::StringValue & req,
    addrbook::ContactList * resp )
{
    char key[ 128 ] = { 0 };
    snprintf( key, sizeof( key ), "addrbook_%s", req.value().c_str() );

    std::map< std::string, std::string > list;

    int ret = client_.hgetall( key, &list, NULL );

    std::map< std::string, std::string >::iterator it = list.begin();
    for( ; list.end() != it; ++it ) {
        resp->add_contact()->ParseFromString( it->second );
    }

    return ret >= 0 ? 0 : -1;
}

int AddrbookDAO :: GetOne( const addrbook::GetOneReq & req,
    addrbook::Contact * resp )
{
    char key[ 128 ] = { 0 };
    snprintf( key, sizeof( key ), "addrbook_%s", req.username().c_str() );

    std::string value;

    bool ret = client_.hget( key, req.contact().c_str(), &value );
    if( ret ) resp->ParseFromString( value );

    return ret ? 0 : -2;
}

