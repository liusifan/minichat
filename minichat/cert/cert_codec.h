
#pragma once

#include "cert.pb.h"

namespace TaoCrypt {
    class RSA_PrivateKey;
};

class CertCodec {
public:
    static CertCodec * GetDefault();

public:
    CertCodec();
    ~CertCodec();

    bool InitRSA( const char * priv_key_file );

    int RSADecrypt( const cert::CodecBuff & req,
        google::protobuf::BytesValue * resp );

    int AESEncrypt( const cert::CodecBuff & req,
        google::protobuf::BytesValue * resp );

    int AESDecrypt( const cert::CodecBuff & req,
        google::protobuf::BytesValue * resp );

private:

    TaoCrypt::RSA_PrivateKey * priv_key_;
};

