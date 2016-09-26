
#include "cert_codec.h"

#include "taocrypt/include/config.h"
#include "taocrypt/include/rsa.hpp"

#include "crypt/pem_file.h"
#include "crypt/crypt_utils.h"

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

int CertCodec :: RSADecrypt( const cert::CodecBuff & req,
    google::protobuf::BytesValue * resp )
{
    if( req.buff().size() != priv_key_->FixedCiphertextLength() ) {
        return -2;
    }

    TaoCrypt::RandomNumberGenerator rng;

    TaoCrypt::RSAES_Decryptor dec( * priv_key_ );

    resp->mutable_value()->resize( priv_key_->FixedCiphertextLength() );

    int len = dec.Decrypt( (unsigned char*)req.buff().data(), req.buff().size(),
            (unsigned char*)resp->mutable_value()->data(), rng );

    resp->mutable_value()->resize( len );

    return 0;
}

static const char * aes_key_for_auto_auth_ticker_ = "minichat//ticket";

int CertCodec :: AESEncrypt( const cert::CodecBuff & req,
    google::protobuf::BytesValue * resp )
{
    return CryptUtils::AES128Encrypt( aes_key_for_auto_auth_ticker_,
            req.buff(), resp->mutable_value() );
}

int CertCodec :: AESDecrypt( const cert::CodecBuff & req,
    google::protobuf::BytesValue * resp )
{
    return CryptUtils::AES128Decrypt( aes_key_for_auto_auth_ticker_,
            req.buff(), resp->mutable_value() );
}

