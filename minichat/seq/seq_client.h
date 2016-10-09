/* seq_client.h

 Generated by phxrpc_pb2client from seq.proto

*/

#pragma once

#include <string>
#include "seq.pb.h"
#include "phxrpc/rpc.h"

class SeqClient
{
public:
    SeqClient();
    ~SeqClient();

    int PHXEcho( const google::protobuf::StringValue & req,
        google::protobuf::StringValue * resp );

    int PhxBatchEcho( const google::protobuf::StringValue & req,
        google::protobuf::StringValue * resp );

    int Alloc( const seq::AllocReq & req,
        google::protobuf::UInt32Value * resp );

    int Get( const google::protobuf::StringValue & req,
        seq::SyncKey * resp );


private:
    std::string package_name_;
    phxrpc::ClientConfig * config_;
};
