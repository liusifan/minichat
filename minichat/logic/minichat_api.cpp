
#include "minichat_api.h"

#include "logic_client.h"

#include <random>     // random_device, uniform_int_distribution
#include <sstream>

#include "taocrypt/include/config.h"
#include "taocrypt/include/rsa.hpp"

#include "crypt/pem_file.h"
#include "crypt/crypt_utils.h"

#include "phxrpc/rpc.h"

static phxrpc::ClientConfig global_logicclient_config_;
static phxrpc::ClientMonitorPtr global_logicclient_monitor_;

bool MiniChatAPI :: Init( const char * config_file )
{
    return global_logicclient_config_.Read( config_file );
}

const char * MiniChatAPI :: GetPackageName()
{
    const char * ret = global_logicclient_config_.GetPackageName();
    if (strlen(ret) == 0) {
        ret = "logic";
    }
    return ret;
}

MiniChatAPI :: MiniChatAPI( phxrpc::UThreadEpollScheduler * scheduler )
    : scheduler_( scheduler )
{
    static std::mutex monitor_mutex;
    if ( !global_logicclient_monitor_.get() ) { 
        monitor_mutex.lock();
        if ( !global_logicclient_monitor_.get() ) {
            global_logicclient_monitor_ = phxrpc::MonitorFactory::GetFactory()
                ->CreateClientMonitor( GetPackageName() );
        }
        global_logicclient_config_.SetClientMonitor( global_logicclient_monitor_ );
        monitor_mutex.unlock();
    }
}

MiniChatAPI :: ~MiniChatAPI()
{
}

int MiniChatAPI :: Auth( const char * username, const char * pwd_md5, logic::AuthResponse * resp_obj )
{
    logic::MiniRequest req;
    logic::MiniResponse resp;

    logic::ManualAuthReq manual_auth_req;;
    {
        req.mutable_head()->set_username( username );
        req.mutable_head()->set_enc_algo( logic::ENC_NONE );

        manual_auth_req.set_pwd_md5( pwd_md5 );

        std::random_device rd;
        std::stringstream fmt;
        fmt << rd() << rd();
        manual_auth_req.set_rand_key( fmt.str() );
    }

    logic::AuthRequest req_obj;
    {
        std::string tmp_buff;
        manual_auth_req.SerializeToString( &tmp_buff );

        TaoCrypt::RSA_PublicKey pub;
        PemFileUtils::LoadPubKey( "~/minichat/etc/client/minichat_pubkey.pem", &pub );

        TaoCrypt::RandomNumberGenerator rng;
        TaoCrypt::RSAES_Encryptor enc( pub );

        req_obj.mutable_manual_auth_req()->resize( pub.FixedCiphertextLength() );

        enc.Encrypt( (unsigned char*)tmp_buff.c_str(), tmp_buff.size(),
                (unsigned char*)req_obj.manual_auth_req().data(), rng);

        req_obj.SerializeToString( req.mutable_req_buff() );
    }

    int ret = Call_L0( "/logic/Auth", 1, req, &resp );

    if( 0 == ret ) {
        std::string tmp;

        CryptUtils::AES128Decrypt( manual_auth_req.rand_key().c_str(),
                resp.resp_buff(), &tmp );
        
        resp_obj->ParseFromString( tmp );

        session_key_ = resp_obj->session_key();
        username_ = resp_obj->username();
        auto_auth_ticket_ = resp_obj->auto_auth_ticket();
        auto_auth_aes_key_ = resp_obj->auto_auth_aes_key();
    }

    return ret;
}

int MiniChatAPI :: AutoAuth( logic::AuthResponse * resp_obj )
{
    logic::MiniRequest req;
    logic::MiniResponse resp;

    logic::AutoAuthReq auth_req;
    {
        req.mutable_head()->set_username( username_ );
        req.mutable_head()->set_enc_algo( logic::ENC_NONE );

        auth_req.set_ticket( auto_auth_ticket_ );

        std::random_device rd;
        std::stringstream fmt;
        fmt << rd() << rd();

        auth_req.set_rand_key( fmt.str() );
    }

    logic::AuthRequest req_obj;
    {
        std::string tmp;
        auth_req.SerializeToString( &tmp );

        CryptUtils::AES128Encrypt( auto_auth_aes_key_.c_str(),
                tmp, req_obj.mutable_auto_auth_req() );

        req_obj.SerializeToString( req.mutable_req_buff() );
    }

    int ret = Call_L0( "/logic/Auth", 1, req, &resp );

    if( 0 == ret ) {
        std::string tmp;

        CryptUtils::AES128Decrypt( auth_req.rand_key().c_str(),
                resp.resp_buff(), &tmp );
        
        resp_obj->ParseFromString( tmp );

        session_key_ = resp_obj->session_key();
        username_ = resp_obj->username();
        auto_auth_ticket_ = resp_obj->auto_auth_ticket();
        auto_auth_aes_key_ = resp_obj->auto_auth_aes_key();
    }

    return ret;
}

int MiniChatAPI :: SendMsg( const char * username, const char * to,
        const char * text, logic::SendMsgResponse * resp_obj )
{
    username_ = username;

    logic::SendMsgRequest req;
    {
        logic::MsgRequest * msg = req.add_msg();
        msg->set_to( to );
        msg->set_content( text );

        std::random_device rd;
        std::stringstream fmt;
        fmt << rd() << rd();
        msg->set_uuid( fmt.str() );
    }

    return Call_L1( "/logic/SendMsg", 3, req, resp_obj );
}

int MiniChatAPI :: Sync( const char * username, logic::SyncResponse * resp_obj )
{
    username_ = username;

    int ret = Call_L1( "/logic/Sync", 2, sync_key_, resp_obj );

    if( 0 == ret ) {
        sync_key_.ParseFromString( resp_obj->new_sync_key() );
    }

    return ret;
}

int MiniChatAPI :: Call_L1( const char * uri, int cmd_id,
        const google::protobuf::MessageLite & request,
        google::protobuf::MessageLite * response )
{
    logic::MiniRequest req_obj;
    logic::MiniResponse resp_obj;

    req_obj.mutable_head()->set_username( username_ );

    if( session_key_.size() > 0 ) {
        req_obj.mutable_head()->set_enc_algo( logic::ENC_AES );
    } else {
        // WARNING: just for unit test
        req_obj.mutable_head()->set_enc_algo( logic::ENC_NONE );
    }

    if( session_key_.size() > 0 ) {
        std::string tmp;

        request.SerializeToString( &tmp );
        CryptUtils::AES128Encrypt( session_key_.c_str(), tmp, req_obj.mutable_req_buff() );
    } else {
        request.SerializeToString( req_obj.mutable_req_buff() );
    }

    int ret = Call_L0( uri, cmd_id, req_obj, &resp_obj );

    if( session_key_.size() > 0 ) {
        std::string tmp;

        CryptUtils::AES128Decrypt( session_key_.c_str(), resp_obj.resp_buff(), &tmp );
        response->ParseFromString( tmp );
    } else {
        response->ParseFromString( resp_obj.resp_buff() );
    }

    return ret;
}

int MiniChatAPI :: Call_L0( const char * uri, int cmd_id,
        const logic::MiniRequest & req, logic::MiniResponse * resp )
{
    const phxrpc::Endpoint_t * ep = global_logicclient_config_.GetRandom();

    if(ep == nullptr) return -1;

    bool open_ret = false;

    std::shared_ptr<phxrpc::BaseTcpStream> socket;

    if( NULL == scheduler_ ) {
        std::shared_ptr<phxrpc::BlockTcpStream> tmp( new phxrpc::BlockTcpStream );
        open_ret = phxrpc::PhxrpcTcpUtils::Open( tmp.get(), ep->ip, ep->port,
                global_logicclient_config_.GetConnectTimeoutMS(), NULL, 0, 
                *(global_logicclient_monitor_.get()));

        socket = tmp;
    } else {
        std::shared_ptr<phxrpc::UThreadTcpStream> tmp( new phxrpc::UThreadTcpStream );
        open_ret = phxrpc::PhxrpcTcpUtils::Open( scheduler_, tmp.get(), ep->ip, ep->port,
                    global_logicclient_config_.GetConnectTimeoutMS(),
                    *(global_logicclient_monitor_.get()));

        socket = tmp;
    }

    if ( ! open_ret ) return -1;

    socket->SetTimeout(global_logicclient_config_.GetSocketTimeoutMS());

    phxrpc::HttpCaller caller( *( socket.get() ), *( global_logicclient_monitor_.get() ) );
    caller.SetURI( uri, cmd_id );
    // TODO: construct a connection pool, use keep_alive
    caller.SetKeepAlive( false );
    caller.SetIsEnableCliFr( global_logicclient_config_.IsEnableClientFastReject() );

    int ret = caller.Call( req, resp );

    phxrpc::log( LOG_DEBUG, "Call_L0( %s ) %d", uri, ret );

    return ret;
}

