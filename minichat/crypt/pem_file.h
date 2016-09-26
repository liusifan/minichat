
#pragma once

namespace TaoCrypt {
    class RSA_PrivateKey;
    class RSA_PublicKey;
    class Source;
};

class PemFileUtils {
public:
    static bool LoadPrivKey( const char * file, TaoCrypt::RSA_PrivateKey * priv_key );
    static bool LoadPubKey( const char * file, TaoCrypt::RSA_PublicKey * pub_key );

private:
    static bool ReadPemFile( const char * file, TaoCrypt::Source * content );
};

