/* account_client.cpp

 Generated by phxrpc_pb2client from account.proto

*/

#include <iostream>
#include <memory>
#include <stdlib.h>
#include <mutex>

#include "account_client.h"
#include "phxrpc_account_stub.h"

#include "phxrpc/file.h"
#include "phxrpc/rpc.h"

static phxrpc::ClientMonitorPtr global_accountclient_monitor_;

class AccountClientRegister
{
public:
    AccountClientRegister() {
        phxrpc::ClientConfigRegistry::GetDefault()->Register("account");
    }
    ~AccountClientRegister() {

    }
};

static AccountClientRegister g_accountclient_register;

AccountClient :: AccountClient()
{
    package_name_ = std::string("account");
    config_ = phxrpc::ClientConfigRegistry::GetDefault()->GetConfig("account");
    if(!config_) {
        return;
    }

    static std::mutex monitor_mutex;
    if ( !global_accountclient_monitor_.get() ) { 
        monitor_mutex.lock();
        if ( !global_accountclient_monitor_.get() ) {
            global_accountclient_monitor_ = phxrpc::MonitorFactory::GetFactory()
                ->CreateClientMonitor(config_->GetOssId());
        }
        config_->SetClientMonitor( global_accountclient_monitor_ );
        monitor_mutex.unlock();
    }
}

AccountClient :: ~AccountClient()
{
}

int AccountClient :: PHXEcho( const google::protobuf::StringValue & req,
        google::protobuf::StringValue * resp )
{
    if(!config_) {
        phxrpc::log(LOG_ERR, "%s %s config is NULL", __func__, package_name_.c_str());
        return -1;
    }

    int ret = phxrpc::ClientCall( *config_, *(global_accountclient_monitor_.get()),
            [=,&req,&resp]( phxrpc::BaseTcpStream & socket ) -> int {
                AccountStub stub( socket, *(global_accountclient_monitor_.get()) );
                stub.SetConfig( config_ );
                return stub.PHXEcho( req, resp );
            },
            [=]( phxrpc::ClientConfig & config ) -> const phxrpc::Endpoint_t * {
                return config.GetRandom();
            }
    );

    return ret;
}

int AccountClient :: PhxBatchEcho( const google::protobuf::StringValue & req,
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
                            config_->GetConnectTimeoutMS(), *(global_accountclient_monitor_.get()))) { 
                    socket.SetTimeout(config_->GetSocketTimeoutMS());
                    AccountStub stub(socket, *(global_accountclient_monitor_.get()));
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

int AccountClient :: Set( const account::User & req,
        google::protobuf::Empty * resp )
{
    if(!config_) {
        phxrpc::log(LOG_ERR, "%s %s config is NULL", __func__, package_name_.c_str());
        return -1;
    }

    int ret = phxrpc::ClientCall( *config_, *(global_accountclient_monitor_.get()),
            [=,&req,&resp]( phxrpc::BaseTcpStream & socket ) -> int {
                AccountStub stub( socket, *(global_accountclient_monitor_.get()) );
                stub.SetConfig( config_ );
                return stub.Set( req, resp );
            },
            [=]( phxrpc::ClientConfig & config ) -> const phxrpc::Endpoint_t * {
                return config.GetRandom();
            }
    );

    return ret;
}

int AccountClient :: SetPwd( const account::PwdReq & req,
        google::protobuf::Empty * resp )
{
    if(!config_) {
        phxrpc::log(LOG_ERR, "%s %s config is NULL", __func__, package_name_.c_str());
        return -1;
    }

    int ret = phxrpc::ClientCall( *config_, *(global_accountclient_monitor_.get()),
            [=,&req,&resp]( phxrpc::BaseTcpStream & socket ) -> int {
                AccountStub stub( socket, *(global_accountclient_monitor_.get()) );
                stub.SetConfig( config_ );
                return stub.SetPwd( req, resp );
            },
            [=]( phxrpc::ClientConfig & config ) -> const phxrpc::Endpoint_t * {
                return config.GetRandom();
            }
    );

    return ret;
}

int AccountClient :: Get( const google::protobuf::StringValue & req,
        account::User * resp )
{
    if(!config_) {
        phxrpc::log(LOG_ERR, "%s %s config is NULL", __func__, package_name_.c_str());
        return -1;
    }

    int ret = phxrpc::ClientCall( *config_, *(global_accountclient_monitor_.get()),
            [=,&req,&resp]( phxrpc::BaseTcpStream & socket ) -> int {
                AccountStub stub( socket, *(global_accountclient_monitor_.get()) );
                stub.SetConfig( config_ );
                return stub.Get( req, resp );
            },
            [=]( phxrpc::ClientConfig & config ) -> const phxrpc::Endpoint_t * {
                return config.GetRandom();
            }
    );

    return ret;
}

int AccountClient :: Auth( const account::PwdReq & req,
        google::protobuf::Empty * resp )
{
    if(!config_) {
        phxrpc::log(LOG_ERR, "%s %s config is NULL", __func__, package_name_.c_str());
        return -1;
    }

    int ret = phxrpc::ClientCall( *config_, *(global_accountclient_monitor_.get()),
            [=,&req,&resp]( phxrpc::BaseTcpStream & socket ) -> int {
                AccountStub stub( socket, *(global_accountclient_monitor_.get()) );
                stub.SetConfig( config_ );
                return stub.Auth( req, resp );
            },
            [=]( phxrpc::ClientConfig & config ) -> const phxrpc::Endpoint_t * {
                return config.GetRandom();
            }
    );

    return ret;
}


