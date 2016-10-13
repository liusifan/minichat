

#include "cgi_auth.h"

#include "logic.pb.h"

#include "account/account_client.h"
#include "profile/profile_client.h"
#include "presence/presence_client.h"
#include "cert/cert_client.h"

#include "phxrpc/file.h"

#include "crypt/crypt_utils.h"
#include "logic_monitor.h"


CgiAuth :: CgiAuth( LogicServerConfig & config )
    : CgiBase( config )
{
    business_name_ = std::string("Auth");
}

CgiAuth :: ~CgiAuth()
{
}

static int DoAutoAuth( const logic::ReqHead & head,
        const logic::AuthRequest & req_obj, std::string * rand_key )
{
    int ret = 0;

    logic::AutoAuthReq auth_req;

    // 1. decrypt auto_auth_req
    PresenceClient pres_client;
    {
        google::protobuf::StringValue username;
        presence::Session session;
        username.set_value( head.username() );

        int ret = pres_client.Get( username, &session );

        if( 0 != ret ) return ret;

        string tmp;

        CryptUtils::AES128Decrypt( session.auto_auth_aes_key().c_str(),
                req_obj.auto_auth_req(), &tmp );

        if( ! auth_req.ParseFromString( tmp ) ) {
            phxrpc::log( LOG_ERR, "Invalid auto_auth_req" );
            return -1;
        }

        *rand_key = auth_req.rand_key();
    }

    logic::AutoAuthTicket ticket;

    // 2. decrypt auto_auth_ticket
    CertClient cert_client;
    {
        cert::CodecBuff enc_buff;
        google::protobuf::BytesValue dec_buff;

        enc_buff.set_buff( auth_req.ticket() );
        ret = cert_client.AESDecrypt( enc_buff, &dec_buff );

        if( 0 != ret ) {
            phxrpc::log( LOG_ERR, "ERR: AESDecrypt %d, fail", ret );
            return ret;
        }

        if( ! ticket.ParseFromString( dec_buff.value() ) ) {
            phxrpc::log( LOG_ERR, "Invalid ticket" );
            return -1;
        }
    }

    // 3. verify ticket
    {
        if( 0 != strcasecmp( ticket.username().c_str(), head.username().c_str() ) ) {
            phxrpc::log( LOG_ERR, "Invalid ticket, username mismatch, %s, %s",
                    ticket.username().c_str(), head.username().c_str() );
            return -1;
        }

        if( ( time( NULL ) - ticket.createtime() ) > 86400 * 7 ) {
            phxrpc::log( LOG_ERR, "Invalid ticket, expired %u", ticket.createtime() );
            return -1;
        }
    }

    return 0;
}

static int DoManualAuth( const logic::ReqHead & head,
        const logic::AuthRequest & req_obj, std::string * rand_key )
{
    int ret = 0;

    logic::ManualAuthReq manual_auth_req;

    // 1. rsa decrypt
    CertClient cert_client;
    {
        if( logic::ENC_RSA == head.enc_algo() ) {
            cert::CodecBuff enc_buff;
            google::protobuf::BytesValue dec_buff;

            enc_buff.set_buff( req_obj.manual_auth_req() );
            ret = cert_client.RSADecrypt( enc_buff, &dec_buff );

            if( 0 != ret ) {
                phxrpc::log( LOG_ERR, "ERR: RSADecrypt %d, fail", ret );
                return ret;
            }

            if( ! manual_auth_req.ParseFromString( dec_buff.value() ) ) return -2;
        } else {
            if( ! manual_auth_req.ParseFromString( req_obj.manual_auth_req() ) ) return -2;
        }
    }

    // 2. auth
    AccountClient acct_client;
    {
        account::PwdReq pwd_req;
        google::protobuf::Empty empty;

        pwd_req.set_username( head.username() );
        pwd_req.set_pwd_md5( manual_auth_req.pwd_md5() );

        ret = acct_client.Auth( pwd_req, &empty );

        if( 0 != ret ) {
            phxrpc::log( LOG_ERR, "ERR: Auth %d, fail", ret );
            return ret;
        }
    }

    * rand_key = manual_auth_req.rand_key();

    return ret;
}

int CgiAuth :: Process( const logic::ReqHead & head,
            const std::string & req_buff, std::string * resp_buff )
{

    LogicMonitor::GetDefault()->ReportCgiAuthCount();

    int ret = -1;
    logic::AuthRequest req_obj;
    logic::AuthResponse resp_obj;

    std::string rand_key;

    req_obj.ParseFromString( req_buff );

    if( req_obj.auto_auth_req().size() > 0 ) {
        ret = DoAutoAuth( head, req_obj, &rand_key );
    }

    if( 0 != ret && req_obj.manual_auth_req().size() > 0 ) {
        ret = DoManualAuth( head, req_obj, &rand_key );
    }
    
    if( 0 != ret ) {
        LogicMonitor::GetDefault()->ReportCgiAuthFailCount();
        return ret;
    }

    // 3. create session
    PresenceClient pres_client;
    {
        google::protobuf::StringValue username;
        presence::Session session;

        username.set_value( head.username().c_str() );

        ret = pres_client.Create( username, &session );

        if( 0 != ret ) {
            phxrpc::log( LOG_ERR, "ERR: Create Session %d, fail", ret );
            LogicMonitor::GetDefault()->ReportCgiAuthFailCount();
            return ret;
        }

        resp_obj.set_session_key( session.session_key() );
        resp_obj.set_auto_auth_aes_key( session.auto_auth_aes_key() );
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

        phxrpc::log( LOG_INFO, "%s", resp_obj.DebugString().c_str() );
    }

    // 6. encrypt resp buff
    {
        std::string tmp_buff;
        resp_obj.SerializeToString( &tmp_buff );

        CryptUtils::AES128Encrypt( rand_key.c_str(), tmp_buff, resp_buff );
    }

    if(0 != ret) {
        LogicMonitor::GetDefault()->ReportCgiAuthFailCount();
    } else {
        LogicMonitor::GetDefault()->ReportCgiAuthSuccCount();
    }
    return ret;
}

