
#include "pem_file.h"

#include "phxrpc/file.h"

//#include "taocrypt/include/config.h"
#include "taocrypt/include/rsa.hpp"
#include "taocrypt/include/file.hpp"
#include "taocrypt/include/coding.hpp"

bool PemFileUtils :: LoadPrivKey( const char * file, TaoCrypt::RSA_PrivateKey * priv_key )
{
    TaoCrypt::Source source;

    bool ret = ReadPemFile( file, &source );

    if( ret ) priv_key->Initialize( source );

    return ret;
}

bool PemFileUtils :: LoadPubKey( const char * file, TaoCrypt::RSA_PublicKey * pub_key )
{
    TaoCrypt::Source source;
    
    bool ret = ReadPemFile( file, &source );

    if( ret ) pub_key->Initialize( source );

    return ret;
}

bool PemFileUtils :: ReadPemFile( const char * file, TaoCrypt::Source * content )
{
    bool ret = false;

    std::string org;
    ret = phxrpc::FileUtils::ReadFile( file, &org );
    
    if( ret ) {
        const char * start = strstr( org.c_str(), "-----BEGIN " );
        const char * end = strstr( org.c_str(), "-----END " );

        if( NULL != start ) start = strchr( start, '\n' );

        if( NULL != start && NULL != end ) {
            org.erase( end - org.c_str() );
            org.erase( 0, start - org.c_str() + 1 );

            content->grow( org.size() );
            content->add( (unsigned char*)org.c_str(), org.size() );
            content->set_index(0);

            TaoCrypt::Base64Decoder b64dec( * content );
        } else {
            ret = false;
        }
    }

    return ret;
}

