/* logic_client.cpp

 Generated by phxrpc_pb2client from logic.proto

*/

#include <iostream>
#include <memory>
#include <stdlib.h>
#include <mutex>

#include "logic_client.h"
#include "phxrpc_logic_stub.h"

#include "phxrpc/file.h"
#include "phxrpc/rpc.h"

static phxrpc::ClientMonitorPtr global_logicclient_monitor_;

class LogicClientRegister
{
public:
    LogicClientRegister() {
        phxrpc::ClientConfigRegistry::GetDefault()->Register("logic");
    }
    ~LogicClientRegister() {

    }
};

static LogicClientRegister g_logicclient_register;

LogicClient :: LogicClient()
{
    package_name_ = std::string("logic");
    config_ = phxrpc::ClientConfigRegistry::GetDefault()->GetConfig("logic");
    if(!config_) {
        return;
    }

    static std::mutex monitor_mutex;
    if ( !global_logicclient_monitor_.get() ) { 
        monitor_mutex.lock();
        if ( !global_logicclient_monitor_.get() ) {
            global_logicclient_monitor_ = phxrpc::MonitorFactory::GetFactory()
                ->CreateClientMonitor(config_->GetOssId());
        }
        config_->SetClientMonitor( global_logicclient_monitor_ );
        monitor_mutex.unlock();
    }
}

LogicClient :: ~LogicClient()
{
}

int LogicClient :: PHXEcho( const google::protobuf::StringValue & req,
        google::protobuf::StringValue * resp )
{
    if(!config_) {
        phxrpc::log(LOG_ERR, "%s %s config is NULL", __func__, package_name_.c_str());
        return -1;
    }

    int ret = phxrpc::ClientCall( *config_, *(global_logicclient_monitor_.get()),
            [=,&req,&resp]( phxrpc::BaseTcpStream & socket ) -> int {
                LogicStub stub( socket, *(global_logicclient_monitor_.get()) );
                stub.SetConfig( config_ );
                return stub.PHXEcho( req, resp );
            },
            [=]( phxrpc::ClientConfig config ) -> const phxrpc::Endpoint_t * {
                return config.GetRandom();
            }
    );

    return ret;
}

int LogicClient :: PhxBatchEcho( const google::protobuf::StringValue & req,
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
                            config_->GetConnectTimeoutMS(), *(global_logicclient_monitor_.get()))) { 
                    socket.SetTimeout(config_->GetSocketTimeoutMS());
                    LogicStub stub(socket, *(global_logicclient_monitor_.get()));
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

int LogicClient :: Auth( const logic::MiniRequest & req,
        logic::MiniResponse * resp )
{
    if(!config_) {
        phxrpc::log(LOG_ERR, "%s %s config is NULL", __func__, package_name_.c_str());
        return -1;
    }

    int ret = phxrpc::ClientCall( *config_, *(global_logicclient_monitor_.get()),
            [=,&req,&resp]( phxrpc::BaseTcpStream & socket ) -> int {
                LogicStub stub( socket, *(global_logicclient_monitor_.get()) );
                stub.SetConfig( config_ );
                return stub.Auth( req, resp );
            },
            [=]( phxrpc::ClientConfig config ) -> const phxrpc::Endpoint_t * {
                return config.GetRandom();
            }
    );

    return ret;
}

int LogicClient :: Sync( const logic::MiniRequest & req,
        logic::MiniResponse * resp )
{
    if(!config_) {
        phxrpc::log(LOG_ERR, "%s %s config is NULL", __func__, package_name_.c_str());
        return -1;
    }

    int ret = phxrpc::ClientCall( *config_, *(global_logicclient_monitor_.get()),
            [=,&req,&resp]( phxrpc::BaseTcpStream & socket ) -> int {
                LogicStub stub( socket, *(global_logicclient_monitor_.get()) );
                stub.SetConfig( config_ );
                return stub.Sync( req, resp );
            },
            [=]( phxrpc::ClientConfig config ) -> const phxrpc::Endpoint_t * {
                return config.GetRandom();
            }
    );

    return ret;
}

int LogicClient :: SendMsg( const logic::MiniRequest & req,
        logic::MiniResponse * resp )
{
    if(!config_) {
        phxrpc::log(LOG_ERR, "%s %s config is NULL", __func__, package_name_.c_str());
        return -1;
    }

    int ret = phxrpc::ClientCall( *config_, *(global_logicclient_monitor_.get()),
            [=,&req,&resp]( phxrpc::BaseTcpStream & socket ) -> int {
                LogicStub stub( socket, *(global_logicclient_monitor_.get()) );
                stub.SetConfig( config_ );
                return stub.SendMsg( req, resp );
            },
            [=]( phxrpc::ClientConfig config ) -> const phxrpc::Endpoint_t * {
                return config.GetRandom();
            }
    );

    return ret;
}

int LogicClient :: FakeDoAll( const logic::MiniRequest & req,
        logic::MiniResponse * resp )
{
    if(!config_) {
        phxrpc::log(LOG_ERR, "%s %s config is NULL", __func__, package_name_.c_str());
        return -1;
    }

    int ret = phxrpc::ClientCall( *config_, *(global_logicclient_monitor_.get()),
            [=,&req,&resp]( phxrpc::BaseTcpStream & socket ) -> int {
                LogicStub stub( socket, *(global_logicclient_monitor_.get()) );
                stub.SetConfig( config_ );
                return stub.FakeDoAll( req, resp );
            },
            [=]( phxrpc::ClientConfig config ) -> const phxrpc::Endpoint_t * {
                return config.GetRandom();
            }
    );

    return ret;
}


