/* presence_client.h

 Generated by phxrpc_pb2client from presence.proto

*/

#pragma once

#include "presence.pb.h"
#include "phxrpc/rpc.h"

class PresenceClient
{
public:
    static bool Init( const char * config_file );

    static const char * GetPackageName();

public:
    PresenceClient();
    ~PresenceClient();

    int PHXEcho( const google::protobuf::StringValue & req,
        google::protobuf::StringValue * resp );

    int PhxBatchEcho( const google::protobuf::StringValue & req,
        google::protobuf::StringValue * resp );

    int Create( const google::protobuf::StringValue & req,
        presence::Session * resp );

    int Get( const google::protobuf::StringValue & req,
        presence::Session * resp );

    int Remove( const google::protobuf::StringValue & req,
        presence::Session * resp );


};
