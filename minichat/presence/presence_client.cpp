/* presence_client.cpp

 Generated by phxrpc_pb2client from presence.proto

*/

#include <iostream>
#include <memory>
#include <stdlib.h>
#include <mutex>

#include "presence_client.h"
#include "phxrpc_presence_stub.h"

#include "phxrpc/file.h"
#include "phxrpc/rpc.h"

static phxrpc::ClientMonitorPtr global_presenceclient_monitor_;

class PresenceClientRegister
{
public:
    PresenceClientRegister() {
        phxrpc::ClientConfigRegistry::GetDefault()->Register("presence");
    }
    ~PresenceClientRegister() {

    }
};

static PresenceClientRegister g_presenceclient_register;

PresenceClient :: PresenceClient()
{
    package_name_ = std::string("presence");
    config_ = phxrpc::ClientConfigRegistry::GetDefault()->GetConfig("presence");
    if(!config_) {
        return;
    }
    static std::mutex monitor_mutex;
    if ( !global_presenceclient_monitor_.get() ) { 
        monitor_mutex.lock();
        if ( !global_presenceclient_monitor_.get() ) {
            global_presenceclient_monitor_ = phxrpc::MonitorFactory::GetFactory()
                ->CreateClientMonitor(package_name_.c_str());
        }
        config_->SetClientMonitor( global_presenceclient_monitor_ );
        monitor_mutex.unlock();
    }
}

PresenceClient :: ~PresenceClient()
{
}

int PresenceClient :: PHXEcho( const google::protobuf::StringValue & req,
        google::protobuf::StringValue * resp )
{
    if(!config_) {
        phxrpc::log(LOG_ERR, "%s %s config is NULL", __func__, package_name_.c_str());
        return -1;
    }
    const phxrpc::Endpoint_t * ep = config_->GetRandom();

    if(ep != nullptr) {
        phxrpc::BlockTcpStream socket;
        bool open_ret = phxrpc::PhxrpcTcpUtils::Open(&socket, ep->ip, ep->port,
                    config_->GetConnectTimeoutMS(), NULL, 0, 
                    *(global_presenceclient_monitor_.get()));
        if ( open_ret ) {
            socket.SetTimeout(config_->GetSocketTimeoutMS());

            PresenceStub stub(socket, *(global_presenceclient_monitor_.get()));
            stub.SetConfig(config_);
            return stub.PHXEcho(req, resp);
        } 
    }

    return -1;
}

int PresenceClient :: PhxBatchEcho( const google::protobuf::StringValue & req,
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
                            config_->GetConnectTimeoutMS(), *(global_presenceclient_monitor_.get()))) { 
                    socket.SetTimeout(config_->GetSocketTimeoutMS());
                    PresenceStub stub(socket, *(global_presenceclient_monitor_.get()));
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

int PresenceClient :: Create( const google::protobuf::StringValue & req,
        presence::Session * resp )
{
    if(!config_) {
        phxrpc::log(LOG_ERR, "%s %s config is NULL", __func__, package_name_.c_str());
        return -1;
    }
    const phxrpc::Endpoint_t * ep = config_->GetRandom();

    if(ep != nullptr) {
        phxrpc::BlockTcpStream socket;
        bool open_ret = phxrpc::PhxrpcTcpUtils::Open(&socket, ep->ip, ep->port,
                    config_->GetConnectTimeoutMS(), NULL, 0, 
                    *(global_presenceclient_monitor_.get()));
        if ( open_ret ) {
            socket.SetTimeout(config_->GetSocketTimeoutMS());

            PresenceStub stub(socket, *(global_presenceclient_monitor_.get()));
            stub.SetConfig(config_);
            return stub.Create(req, resp);
        } 
    }

    return -1;
}

int PresenceClient :: Get( const google::protobuf::StringValue & req,
        presence::Session * resp )
{
    if(!config_) {
        phxrpc::log(LOG_ERR, "%s %s config is NULL", __func__, package_name_.c_str());
        return -1;
    }
    const phxrpc::Endpoint_t * ep = config_->GetRandom();

    if(ep != nullptr) {
        phxrpc::BlockTcpStream socket;
        bool open_ret = phxrpc::PhxrpcTcpUtils::Open(&socket, ep->ip, ep->port,
                    config_->GetConnectTimeoutMS(), NULL, 0, 
                    *(global_presenceclient_monitor_.get()));
        if ( open_ret ) {
            socket.SetTimeout(config_->GetSocketTimeoutMS());

            PresenceStub stub(socket, *(global_presenceclient_monitor_.get()));
            stub.SetConfig(config_);
            return stub.Get(req, resp);
        } 
    }

    return -1;
}

int PresenceClient :: Remove( const google::protobuf::StringValue & req,
        presence::Session * resp )
{
    if(!config_) {
        phxrpc::log(LOG_ERR, "%s %s config is NULL", __func__, package_name_.c_str());
        return -1;
    }
    const phxrpc::Endpoint_t * ep = config_->GetRandom();

    if(ep != nullptr) {
        phxrpc::BlockTcpStream socket;
        bool open_ret = phxrpc::PhxrpcTcpUtils::Open(&socket, ep->ip, ep->port,
                    config_->GetConnectTimeoutMS(), NULL, 0, 
                    *(global_presenceclient_monitor_.get()));
        if ( open_ret ) {
            socket.SetTimeout(config_->GetSocketTimeoutMS());

            PresenceStub stub(socket, *(global_presenceclient_monitor_.get()));
            stub.SetConfig(config_);
            return stub.Remove(req, resp);
        } 
    }

    return -1;
}


