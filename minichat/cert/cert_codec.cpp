
#include "cert_codec.h"

#include "taocrypt/include/config.h"
#include "taocrypt/include/rsa.hpp"
#include "taocrypt/include/file.hpp"
#include "taocrypt/include/aes.hpp"

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
    std::string key( aes_key_for_auto_auth_ticker_ );
    key.resize( 16 );

    TaoCrypt::AES_ECB_Encryption enc;

    enc.SetKey( (unsigned char*)key.c_str(), key.size() );

    const int bs(TaoCrypt::AES::BLOCK_SIZE);
    resp->mutable_value()->resize( bs * ( ( req.buff().size() + bs - 1 ) / bs ) );

    std::string tmp = req.buff();
    tmp.resize( resp->value().size() );

    enc.Process( (unsigned char*)resp->value().c_str(), (unsigned char*)tmp.c_str(), tmp.size() );

    return 0;
}

int CertCodec :: AESDecrypt( const cert::CodecBuff & req,
    google::protobuf::BytesValue * resp )
{
    std::string key( aes_key_for_auto_auth_ticker_ );
    key.resize( 16 );

    TaoCrypt::AES_ECB_Decryption dec;

    dec.SetKey( (unsigned char*)key.c_str(), key.size() );

    const int bs(TaoCrypt::AES::BLOCK_SIZE);
    resp->mutable_value()->resize( bs * ( ( req.buff().size() + bs - 1 ) / bs ) );

    dec.Process( (unsigned char*)resp->value().c_str(),
            (unsigned char*)req.buff().c_str(), req.buff().size() );

    return 0;
}

