
#include "crypt_utils.h"

void test()
{
    const char * key = "secret_aes_key";

    std::string in = "hello world";

    std::string out1, out2;

    CryptUtils::AES128Encrypt( key, in, &out1 );

    CryptUtils::AES128Decrypt( key, out1, &out2 );

    printf( "%d\n", out1.size() );

    printf( "dec( enc( '%s', '%s' ) ) = '%s'\n", key, in.c_str(), out2.c_str() );
}

int main( int argc, char * argv[] )
{
    test();

    return 0;
}

