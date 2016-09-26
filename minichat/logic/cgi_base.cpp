
#include "cgi_base.h"

#include "phxrpc/file.h"

#include "presence/presence_client.h"

#include "crypt/crypt_utils.h"

CgiBase :: CgiBase()
{
    PresenceClient::Init( "~/minichat/etc/client/presence_client.conf" );
}

CgiBase :: ~CgiBase()
{
}

int CgiBase :: AESProcess( const logic::MiniRequest & req, logic::MiniResponse * resp )
{
    google::protobuf::StringValue username;
    presence::Session session;

    PresenceClient client;

    username.set_value( req.head().username() );

    int ret = client.Get( username, &session );

    if( 0 != ret ) return ret;

    session.mutable_session_key()->resize( 16 );

    std::string req_buff, resp_buff;
    {
        CryptUtils::AES128Decrypt( session.session_key().c_str(),
                req.req_buff(), &req_buff );
    }

    ret = Process( req.head(), req_buff, &resp_buff );

    if( resp_buff.size() > 0 ) {
        CryptUtils::AES128Encrypt( session.session_key().c_str(),
                resp_buff.c_str(), resp->mutable_resp_buff() );
    }

    return ret;
}

int CgiBase :: Execute( const logic::MiniRequest & req, logic::MiniResponse * resp )
{
    int ret = 0;

    if( logic::ENC_AES == req.head().enc_algo() ) {         // for normal cgi
        ret = AESProcess( req, resp );
    } else if( ( logic::ENC_NONE == req.head().enc_algo() ) // WARNING: only for unit test
            || ( logic::ENC_RSA  == req.head().enc_algo() ) // for auth cgi
            ) {
        ret = Process( req.head(), req.req_buff(), resp->mutable_resp_buff() );
    }

    resp->set_ret( ret );

    phxrpc::log( LOG_INFO, "INFO: %s %d", req.head().username().c_str(), ret );

    return ret;
}

