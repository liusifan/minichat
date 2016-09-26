
#include "cgi_auth.h"

#include "logic.pb.h"

#include "account/account_client.h"
#include "profile/profile_client.h"
#include "presence/presence_client.h"
#include "cert/cert_client.h"

#include "phxrpc/file.h"

#include "crypt/crypt_utils.h"

CgiAuth :: CgiAuth()
{
    AccountClient::Init( "~/minichat/etc/client/account_client.conf" );
    ProfileClient::Init( "~/minichat/etc/client/profile_client.conf" );
    PresenceClient::Init( "~/minichat/etc/client/presence_client.conf" );
    CertClient::Init( "~/minichat/etc/client/cert_client.conf" );
}

CgiAuth :: ~CgiAuth()
{
}

static int DoManualAuth( const logic::ReqHead & head,
        const logic::AuthRequest & req_obj, std::string * rand_key )
{
    int ret = 0;

    logic::ManualAuthReq auth_req;

    // 1. rsa decrypt
    CertClient cert_client;
    {
        cert::CodecBuff enc_buff;
        google::protobuf::BytesValue dec_buff;

        enc_buff.set_buff( req_obj.manual_auth_req() );
        ret = cert_client.RSADecrypt( enc_buff, &dec_buff );

        if( 0 != ret ) {
            phxrpc::log( LOG_ERR, "ERR: RSADecrypt %d, fail", ret );
            return ret;
        }

        if( ! auth_req.ParseFromString( dec_buff.value() ) ) return -2;
    }

    // 2. auth
    AccountClient acct_client;
    {
        account::PwdReq pwd_req;
        google::protobuf::Empty empty;

        pwd_req.set_username( head.username() );
        pwd_req.set_pwd_md5( auth_req.pwd_md5() );

        ret = acct_client.Auth( pwd_req, &empty );

        if( 0 != ret ) {
            phxrpc::log( LOG_ERR, "ERR: Auth %d, fail", ret );
            return ret;
        }
    }

    * rand_key = auth_req.rand_key();

    return ret;
}

int CgiAuth :: Process( const logic::ReqHead & head,
            const std::string & req_buff, std::string * resp_buff )
{
    int ret = 0;

    logic::AuthRequest req_obj;
    logic::AuthResponse resp_obj;

    std::string rand_key;

    req_obj.ParseFromString( req_buff );

    ret = DoManualAuth( head, req_obj, &rand_key );
    
    if( 0 != ret ) return ret;

    // 3. create session
    PresenceClient pres_client;
    {
        google::protobuf::StringValue username;
        presence::Session session;

        username.set_value( head.username().c_str() );

        ret = pres_client.Create( username, &session );

        if( 0 != ret ) {
            phxrpc::log( LOG_ERR, "ERR: Create Session %d, fail", ret );
            return ret;
        }

        resp_obj.set_session_key( session.session_key() );
    }

    // 4. get profile
    ProfileClient prof_client;
    {
        google::protobuf::StringValue username;
        profile::Setting setting;

        username.set_value( head.username().c_str() );

        int tmp_ret = prof_client.Get( username, &setting );

        if( 0 == tmp_ret ) {
            resp_obj.set_username( head.username() );
            resp_obj.set_avatar( setting.avatar() );
            resp_obj.set_nickname( setting.nickname() );
        } else {
            phxrpc::log( LOG_ERR, "ERR: Get Profile %d, fail", tmp_ret );
        }
    }

    // 5. gen auto_auth_ticket
    CertClient cert_client;
    {
        logic::AutoAuthTicket ticket;

        ticket.set_username( head.username() );
        ticket.set_createtime( time( NULL ) );

        cert::CodecBuff buff;
        ticket.SerializeToString( buff.mutable_buff() );

        google::protobuf::BytesValue enc_buff;

        if( 0 == cert_client.AESEncrypt( buff, &enc_buff ) ) {
            resp_obj.set_auto_auth_ticket( enc_buff.value() ); 
        }
    }

    // 6. encrypt resp buff
    {
        std::string tmp_buff;
        resp_obj.SerializeToString( &tmp_buff );

        CryptUtils::AES128Encrypt( rand_key.c_str(), tmp_buff, resp_buff );
    }

    return ret;
}

