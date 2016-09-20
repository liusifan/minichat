/* cert_client.h

 Generated by phxrpc_pb2client from cert.proto

*/

#pragma once

#include "cert.pb.h"
#include "phxrpc/rpc.h"

class CertClient
{
public:
    static bool Init( const char * config_file );

    static const char * GetPackageName();

public:
    CertClient();
    ~CertClient();

    int PHXEcho( const google::protobuf::StringValue & req,
        google::protobuf::StringValue * resp );

    int PhxBatchEcho( const google::protobuf::StringValue & req,
        google::protobuf::StringValue * resp );

    int RSADecrypt( const cert::EncBuff & req,
        google::protobuf::BytesValue * resp );

    int AESDecrypt( const cert::EncBuff & req,
        google::protobuf::BytesValue * resp );


};
