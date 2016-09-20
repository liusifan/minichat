
#include "cert_codec.h"

#include "taocrypt/include/config.h"
#include "taocrypt/include/rsa.hpp"
#include "taocrypt/include/file.hpp"

#include "pem_file.h"

CertCodec * CertCodec :: GetDefault()
{
    static CertCodec codec;

    return &codec;
}

CertCodec :: CertCodec()
{
    priv_key_ = NULL;

    InitRSA( "~/minichat/etc/minichat_privkey.pem" );
}

CertCodec :: ~CertCodec()
{
    if( NULL != priv_key_ ) delete priv_key_;
}

bool CertCodec :: InitRSA( const char * priv_key_file )
{
    priv_key_ = new TaoCrypt::RSA_PrivateKey();

    return PemFileUtils::LoadPrivKey( priv_key_file, priv_key_ );
}

int CertCodec :: RSADecrypt( const cert::EncBuff & req,
    google::protobuf::BytesValue * resp )
{
    if( req.enc_buff().size() != priv_key_->FixedCiphertextLength() ) {
        return -2;
    }

    TaoCrypt::RandomNumberGenerator rng;

    TaoCrypt::RSAES_Decryptor dec( * priv_key_ );

    resp->mutable_value()->resize( priv_key_->FixedCiphertextLength() );

    int len = dec.Decrypt( (unsigned char*)req.enc_buff().data(), req.enc_buff().size(),
            (unsigned char*)resp->mutable_value()->data(), rng );

    resp->mutable_value()->resize( len );

    return 0;
}

int CertCodec :: AESDecrypt( const cert::EncBuff & req,
    google::protobuf::BytesValue * resp )
{
    return 0;
}

