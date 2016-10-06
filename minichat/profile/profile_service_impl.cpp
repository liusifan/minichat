/* profile_service_impl.cpp

 Generated by phxrpc_pb2service from profile.proto

*/

#include "profile_service_impl.h"
#include "profile_server_config.h"
#include "profile.pb.h"
#include "phxrpc/file.h"

#include "common/redis_client_factory.h"

#include "profile_dao.h"

ProfileServiceImpl :: ProfileServiceImpl( ServiceArgs_t & args )
    : args_( args )
{
}

ProfileServiceImpl :: ~ProfileServiceImpl()
{
}

int ProfileServiceImpl :: PHXEcho( const google::protobuf::StringValue & req,
        google::protobuf::StringValue * resp )
{
    resp->set_value( req.value() );
    return 0;
}

int ProfileServiceImpl :: Set( const profile::Setting & req,
        google::protobuf::Empty * resp )
{
    ProfileDAO dao( RedisClientFactory::GetDefault()->Get() );

    return dao.Set( req, resp );
}

int ProfileServiceImpl :: Get( const google::protobuf::StringValue & req,
        profile::Setting * resp )
{
    ProfileDAO dao( RedisClientFactory::GetDefault()->Get() );

    return dao.Get( req, resp );
}


