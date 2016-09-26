
#pragma once

#include <string>

class CryptUtils {
public:
    static int AES128Encrypt( const char * key, const std::string & in, std::string * out );

    static int AES128Decrypt( const char * key, const std::string & in, std::string * out );
};

