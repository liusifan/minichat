/* presence_tool_impl.cpp

 Generated by phxrpc_pb2tool from presence.proto

*/

#include "presence_tool_impl.h"
#include "presence_client.h"

#include "phxrpc/file.h"

using namespace phxrpc;

PresenceToolImpl:: PresenceToolImpl()
{
}

PresenceToolImpl:: ~PresenceToolImpl()
{
}

int PresenceToolImpl :: PHXEcho( phxrpc::OptMap & opt_map )
{
    google::protobuf::StringValue req;
    google::protobuf::StringValue resp;

    if( NULL == opt_map.Get( 's' ) ) return -1;

    req.set_value( opt_map.Get( 's' ) );

    PresenceClient client;
    int ret = client.PHXEcho( req, &resp );
    printf( "%s return %d\n", __func__, ret );
    printf( "resp: {\n%s}\n", resp.DebugString().c_str() );

    return ret;
}

int PresenceToolImpl :: Create( phxrpc::OptMap & opt_map )
{
    google::protobuf::StringValue req;
    presence::Session resp;

    //TODO: fill req from opt_map

    if( NULL == opt_map.Get( 'u' ) ) return -1;

    req.set_value( opt_map.Get( 'u' ) );

    PresenceClient client;
    int ret = client.Create( req, &resp );
    printf( "%s return %d\n", __func__, ret );
    printf( "resp: {\n%s}\n", resp.DebugString().c_str() );

    return ret;
}

int PresenceToolImpl :: Get( phxrpc::OptMap & opt_map )
{
    google::protobuf::StringValue req;
    presence::Session resp;

    //TODO: fill req from opt_map

    if( NULL == opt_map.Get( 'u' ) ) return -1;

    req.set_value( opt_map.Get( 'u' ) );

    PresenceClient client;
    int ret = client.Get( req, &resp );
    printf( "%s return %d\n", __func__, ret );
    printf( "resp: {\n%s}\n", resp.DebugString().c_str() );

    return ret;
}

int PresenceToolImpl :: Remove( phxrpc::OptMap & opt_map )
{
    google::protobuf::StringValue req;
    presence::Session resp;

    //TODO: fill req from opt_map

    if( NULL == opt_map.Get( 'u' ) ) return -1;

    req.set_value( opt_map.Get( 'u' ) );

    PresenceClient client;
    int ret = client.Remove( req, &resp );
    printf( "%s return %d\n", __func__, ret );
    printf( "resp: {\n%s}\n", resp.DebugString().c_str() );

    return ret;
}

