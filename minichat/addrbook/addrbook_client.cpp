/* addrbook_client.cpp

 Generated by phxrpc_pb2client from addrbook.proto

*/

#include <iostream>
#include <memory>
#include <stdlib.h>
#include <mutex>

#include "addrbook_client.h"
#include "phxrpc_addrbook_stub.h"

#include "phxrpc/file.h"
#include "phxrpc/rpc.h"

static phxrpc::ClientMonitorPtr global_addrbookclient_monitor_;

class AddrbookClientRegister
{
public:
    AddrbookClientRegister() {
        phxrpc::ClientConfigRegistry::GetDefault()->Register("addrbook");
    }
    ~AddrbookClientRegister() {

    }
};

static AddrbookClientRegister g_addrbookclient_register;

AddrbookClient :: AddrbookClient()
{
    package_name_ = std::string("addrbook");
    config_ = phxrpc::ClientConfigRegistry::GetDefault()->GetConfig("addrbook");
    if(!config_) {
        return;
    }

    static std::mutex monitor_mutex;
    if ( !global_addrbookclient_monitor_.get() ) { 
        monitor_mutex.lock();
        if ( !global_addrbookclient_monitor_.get() ) {
            global_addrbookclient_monitor_ = phxrpc::MonitorFactory::GetFactory()
                ->CreateClientMonitor(config_->GetOssId());
        }
        config_->SetClientMonitor( global_addrbookclient_monitor_ );
        monitor_mutex.unlock();
    }
}

AddrbookClient :: ~AddrbookClient()
{
}

int AddrbookClient :: PHXEcho( const google::protobuf::StringValue & req,
        google::protobuf::StringValue * resp )
{
    if(!config_) {
        phxrpc::log(LOG_ERR, "%s %s config is NULL", __func__, package_name_.c_str());
        return -1;
    }

    int ret = phxrpc::ClientCall( *config_, *(global_addrbookclient_monitor_.get()),
            [=,&req,&resp]( phxrpc::BaseTcpStream & socket ) -> int {
                AddrbookStub stub( socket, *(global_addrbookclient_monitor_.get()) );
                stub.SetConfig( config_ );
                return stub.PHXEcho( req, resp );
            },
            [=]( phxrpc::ClientConfig & config ) -> const phxrpc::Endpoint_t * {
                return config.GetRandom();
            }
    );

    return ret;
}

int AddrbookClient :: PhxBatchEcho( const google::protobuf::StringValue & req,
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
                            config_->GetConnectTimeoutMS(), *(global_addrbookclient_monitor_.get()))) { 
                    socket.SetTimeout(config_->GetSocketTimeoutMS());
                    AddrbookStub stub(socket, *(global_addrbookclient_monitor_.get()));
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

int AddrbookClient :: Set( const addrbook::ContactReq & req,
        google::protobuf::Empty * resp )
{
    if(!config_) {
        phxrpc::log(LOG_ERR, "%s %s config is NULL", __func__, package_name_.c_str());
        return -1;
    }

    int ret = phxrpc::ClientCall( *config_, *(global_addrbookclient_monitor_.get()),
            [=,&req,&resp]( phxrpc::BaseTcpStream & socket ) -> int {
                AddrbookStub stub( socket, *(global_addrbookclient_monitor_.get()) );
                stub.SetConfig( config_ );
                return stub.Set( req, resp );
            },
            [=]( phxrpc::ClientConfig & config ) -> const phxrpc::Endpoint_t * {
                return config.GetRandom();
            }
    );

    return ret;
}

int AddrbookClient :: GetAll( const google::protobuf::StringValue & req,
        addrbook::ContactList * resp )
{
    if(!config_) {
        phxrpc::log(LOG_ERR, "%s %s config is NULL", __func__, package_name_.c_str());
        return -1;
    }

    int ret = phxrpc::ClientCall( *config_, *(global_addrbookclient_monitor_.get()),
            [=,&req,&resp]( phxrpc::BaseTcpStream & socket ) -> int {
                AddrbookStub stub( socket, *(global_addrbookclient_monitor_.get()) );
                stub.SetConfig( config_ );
                return stub.GetAll( req, resp );
            },
            [=]( phxrpc::ClientConfig & config ) -> const phxrpc::Endpoint_t * {
                return config.GetRandom();
            }
    );

    return ret;
}

int AddrbookClient :: GetOne( const addrbook::GetOneReq & req,
        addrbook::Contact * resp )
{
    if(!config_) {
        phxrpc::log(LOG_ERR, "%s %s config is NULL", __func__, package_name_.c_str());
        return -1;
    }

    int ret = phxrpc::ClientCall( *config_, *(global_addrbookclient_monitor_.get()),
            [=,&req,&resp]( phxrpc::BaseTcpStream & socket ) -> int {
                AddrbookStub stub( socket, *(global_addrbookclient_monitor_.get()) );
                stub.SetConfig( config_ );
                return stub.GetOne( req, resp );
            },
            [=]( phxrpc::ClientConfig & config ) -> const phxrpc::Endpoint_t * {
                return config.GetRandom();
            }
    );

    return ret;
}

int AddrbookClient :: GetBySeq( const addrbook::GetBySeqReq & req,
        addrbook::ContactList * resp )
{
    if(!config_) {
        phxrpc::log(LOG_ERR, "%s %s config is NULL", __func__, package_name_.c_str());
        return -1;
    }

    int ret = phxrpc::ClientCall( *config_, *(global_addrbookclient_monitor_.get()),
            [=,&req,&resp]( phxrpc::BaseTcpStream & socket ) -> int {
                AddrbookStub stub( socket, *(global_addrbookclient_monitor_.get()) );
                stub.SetConfig( config_ );
                return stub.GetBySeq( req, resp );
            },
            [=]( phxrpc::ClientConfig & config ) -> const phxrpc::Endpoint_t * {
                return config.GetRandom();
            }
    );

    return ret;
}


