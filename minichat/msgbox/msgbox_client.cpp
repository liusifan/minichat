/* msgbox_client.cpp

 Generated by phxrpc_pb2client from msgbox.proto

*/

#include <iostream>
#include <memory>
#include <stdlib.h>
#include <mutex>

#include "msgbox_client.h"
#include "phxrpc_msgbox_stub.h"

#include "phxrpc/file.h"
#include "phxrpc/rpc.h"

static phxrpc::ClientMonitorPtr global_msgboxclient_monitor_;

class MsgBoxClientRegister
{
public:
    MsgBoxClientRegister() {
        phxrpc::ClientConfigRegistry::GetDefault()->Register("msgbox");
    }
    ~MsgBoxClientRegister() {

    }
};

static MsgBoxClientRegister g_msgboxclient_register;

MsgBoxClient :: MsgBoxClient()
{
    package_name_ = std::string("msgbox");
    config_ = phxrpc::ClientConfigRegistry::GetDefault()->GetConfig("msgbox");
    if(!config_) {
        return;
    }

    static std::mutex monitor_mutex;
    if ( !global_msgboxclient_monitor_.get() ) { 
        monitor_mutex.lock();
        if ( !global_msgboxclient_monitor_.get() ) {
            global_msgboxclient_monitor_ = phxrpc::MonitorFactory::GetFactory()
                ->CreateClientMonitor(config_->GetOssId());
        }
        config_->SetClientMonitor( global_msgboxclient_monitor_ );
        monitor_mutex.unlock();
    }
}

MsgBoxClient :: ~MsgBoxClient()
{
}

int MsgBoxClient :: PHXEcho( const google::protobuf::StringValue & req,
        google::protobuf::StringValue * resp )
{
    if(!config_) {
        phxrpc::log(LOG_ERR, "%s %s config is NULL", __func__, package_name_.c_str());
        return -1;
    }

    int ret = phxrpc::ClientCall( *config_, *(global_msgboxclient_monitor_.get()),
            [=,&req,&resp]( phxrpc::BaseTcpStream & socket ) -> int {
                MsgBoxStub stub( socket, *(global_msgboxclient_monitor_.get()) );
                stub.SetConfig( config_ );
                return stub.PHXEcho( req, resp );
            },
            [=]( phxrpc::ClientConfig & config ) -> const phxrpc::Endpoint_t * {
                return config.GetRandom();
            }
    );

    return ret;
}

int MsgBoxClient :: PhxBatchEcho( const google::protobuf::StringValue & req,
        google::protobuf::StringValue * resp )
{
    if(!config_) {
        phxrpc::log(LOG_ERR, "%s %s config is NULL", __func__, package_name_.c_str());
        return -1;
    }
    int ret = -1; 
    size_t echo_server_count = 2;
    uthread_begin;
    for (size_t i = 0; i < echo_server_count; i++) {
        uthread_t [=, &uthread_s, &ret](void *) {
            const phxrpc::Endpoint_t * ep = config_->GetByIndex(i);
            if (ep != nullptr) {
                phxrpc::UThreadTcpStream socket;
                if(phxrpc::PhxrpcTcpUtils::Open(&uthread_s, &socket, ep->ip, ep->port,
                            config_->GetConnectTimeoutMS(), *(global_msgboxclient_monitor_.get()))) { 
                    socket.SetTimeout(config_->GetSocketTimeoutMS());
                    MsgBoxStub stub(socket, *(global_msgboxclient_monitor_.get()));
                    stub.SetConfig(config_);
                    int this_ret = stub.PHXEcho(req, resp);
                    if (this_ret == 0) {
                        ret = this_ret;
                        uthread_s.Close();
                    }   
                }   
            }
        };  
    }   
    uthread_end;
    return ret;
}

int MsgBoxClient :: Add( const msgbox::MsgIndex & req,
        msgbox::AddMsgResp * resp )
{
    if(!config_) {
        phxrpc::log(LOG_ERR, "%s %s config is NULL", __func__, package_name_.c_str());
        return -1;
    }

    int ret = phxrpc::ClientCall( *config_, *(global_msgboxclient_monitor_.get()),
            [=,&req,&resp]( phxrpc::BaseTcpStream & socket ) -> int {
                MsgBoxStub stub( socket, *(global_msgboxclient_monitor_.get()) );
                stub.SetConfig( config_ );
                return stub.Add( req, resp );
            },
            [=]( phxrpc::ClientConfig & config ) -> const phxrpc::Endpoint_t * {
                return config.GetRandom();
            }
    );

    return ret;
}

int MsgBoxClient :: GetBySeq( const msgbox::GetBySeqReq & req,
        msgbox::MsgIndexList * resp )
{
    if(!config_) {
        phxrpc::log(LOG_ERR, "%s %s config is NULL", __func__, package_name_.c_str());
        return -1;
    }

    int ret = phxrpc::ClientCall( *config_, *(global_msgboxclient_monitor_.get()),
            [=,&req,&resp]( phxrpc::BaseTcpStream & socket ) -> int {
                MsgBoxStub stub( socket, *(global_msgboxclient_monitor_.get()) );
                stub.SetConfig( config_ );
                return stub.GetBySeq( req, resp );
            },
            [=]( phxrpc::ClientConfig & config ) -> const phxrpc::Endpoint_t * {
                return config.GetRandom();
            }
    );

    return ret;
}

int MsgBoxClient :: GetAll( const google::protobuf::StringValue & req,
        msgbox::MsgIndexList * resp )
{
    if(!config_) {
        phxrpc::log(LOG_ERR, "%s %s config is NULL", __func__, package_name_.c_str());
        return -1;
    }

    int ret = phxrpc::ClientCall( *config_, *(global_msgboxclient_monitor_.get()),
            [=,&req,&resp]( phxrpc::BaseTcpStream & socket ) -> int {
                MsgBoxStub stub( socket, *(global_msgboxclient_monitor_.get()) );
                stub.SetConfig( config_ );
                return stub.GetAll( req, resp );
            },
            [=]( phxrpc::ClientConfig & config ) -> const phxrpc::Endpoint_t * {
                return config.GetRandom();
            }
    );

    return ret;
}


