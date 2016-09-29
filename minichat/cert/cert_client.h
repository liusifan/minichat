/* cert_client.h

 Generated by phxrpc_pb2client from cert.proto

*/

#pragma once

#include "cert.pb.h"
#include "phxrpc/rpc.h"

class CertClientRegister
{
public:
    CertClientRegister();
    ~CertClientRegister();
};


class CertClient
{
public:
    CertClient();
    ~CertClient();

    int PHXEcho( const google::protobuf::StringValue & req,
        google::protobuf::StringValue * resp );

    int PhxBatchEcho( const google::protobuf::StringValue & req,
        google::protobuf::StringValue * resp );

    int RSADecrypt( const cert::CodecBuff & req,
        google::protobuf::BytesValue * resp );

    int AESEncrypt( const cert::CodecBuff & req,
        google::protobuf::BytesValue * resp );

    int AESDecrypt( const cert::CodecBuff & req,
        google::protobuf::BytesValue * resp );

private:
    std::string package_name_;
    phxrpc::ClientConfig * config_;

};
