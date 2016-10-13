
#include "cgi_base.h"

#include "phxrpc/file.h"
#include "phxrpc/qos.h"

#include "presence/presence_client.h"

#include "logic_server_config.h"

#include "crypt/crypt_utils.h"

CgiBase :: CgiBase( LogicServerConfig & config )
    : config_( config )
{
}

CgiBase :: ~CgiBase()
{
}

int CgiBase :: AESProcess( const presence::Session & session,
        const logic::MiniRequest & req, logic::MiniResponse * resp )
{
    phxrpc::log( LOG_INFO, "Call AESProcess" );

    std::string req_buff, resp_buff;
    {
        CryptUtils::AES128Decrypt( session.session_key().c_str(),
                req.req_buff(), &req_buff );
    }

    int ret = Process( req.head(), req_buff, &resp_buff );

    if( resp_buff.size() > 0 ) {
        CryptUtils::AES128Encrypt( session.session_key().c_str(),
                resp_buff.c_str(), resp->mutable_resp_buff() );
    }

    return ret;
}

void SetQosInfo( const char * business_name, const char * key )
{
    std::string qos_info = business_name;
    qos_info.append("_");
    qos_info.append(key);
    phxrpc::FastRejectQoSMgr::SetReqQoSInfo(qos_info.c_str());
}

int CgiBase :: Execute( const logic::MiniRequest & req, logic::MiniResponse * resp )
{
    int ret = 0;

    if( logic::ENC_RSA == req.head().enc_algo() ) {        // for auth cgi
        // always use user-grouping for auth
        SetQosInfo( business_name_.c_str(), req.head().username().c_str() );

        ret = Process( req.head(), req.req_buff(), resp->mutable_resp_buff() );
    } else {
        // always get presence, for enable_session_based
        PresenceClient client;
        presence::Session session;

        google::protobuf::StringValue username;
        username.set_value( req.head().username() );
        ret = client.Get( username, &session );

        if( 0 == ret ) {
            session.mutable_session_key()->resize( 16 );
            if( config_.EnableSessionBased() ) {
                SetQosInfo( business_name_.c_str(), session.session_key().c_str() );
            } else {
                SetQosInfo( business_name_.c_str(), req.head().username().c_str() );
            }

            if( logic::ENC_AES == req.head().enc_algo() ) {    // for normal cgi
                ret = AESProcess( session, req, resp );
            } else {                                           // WARNING: only for unit test
                ret = Process( req.head(), req.req_buff(), resp->mutable_resp_buff() );
            }
        }
    }

    resp->set_ret( ret );

    //phxrpc::log( LOG_INFO, "INFO: %s %d", req.head().username().c_str(), ret );

    return ret;
}

