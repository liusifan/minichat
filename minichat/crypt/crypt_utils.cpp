
#include "crypt_utils.h"

#include <random>     // random_device, uniform_int_distribution
#include <sstream>

#include "taocrypt/include/config.h"
#include "taocrypt/include/rsa.hpp"
#include "taocrypt/include/aes.hpp"

#include "phxrpc/file.h"

int CryptUtils :: AES128Encrypt( const char * key, const std::string & in, std::string * out )
{
    const int bs(TaoCrypt::AES::BLOCK_SIZE);

    std::string fixed_key( key );
    fixed_key.resize( bs );

    int padding = bs - ( in.size() % bs );
    std::string fixed_in( in );
    fixed_in.append( padding, padding );

    out->resize( fixed_in.size() );

    TaoCrypt::AES_ECB_Encryption enc;
    enc.SetKey( (unsigned char*)fixed_key.c_str(), bs );

    enc.Process( (unsigned char*)out->c_str(),
            (unsigned char*)fixed_in.c_str(), fixed_in.size() );

    return 0;
}

int CryptUtils :: AES128Decrypt( const char * key, const std::string & in, std::string * out )
{
    const int bs(TaoCrypt::AES::BLOCK_SIZE);

    std::string fixed_key( key );
    fixed_key.resize( bs );

    std::string fixed_in( in );
    fixed_in.resize( bs * ( ( fixed_in.size() + bs - 1 ) / bs ) );

    out->resize( fixed_in.size() );

    TaoCrypt::AES_ECB_Decryption dec;
    dec.SetKey( (unsigned char*)fixed_key.c_str(), bs );

    dec.Process( (unsigned char*)out->c_str(),
            (unsigned char*)fixed_in.c_str(), fixed_in.size() );

    int padding = *( out->rbegin() );
    if( padding <= bs ) {
        out->resize( out->size() - padding );
    } else {
        phxrpc::log( LOG_ERR, "ERR: invalid padding %d", padding );
    }

    return 0;
}

