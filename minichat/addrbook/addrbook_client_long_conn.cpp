/* addrbook_client.cpp

 Generated by phxrpc_pb2client from addrbook.proto

*/

#include <iostream>
#include <memory>
#include <stdlib.h>
#include <mutex>

#include "addrbook_client_long_conn.h"
#include "phxrpc_addrbook_stub.h"

#include "phxrpc/file.h"
#include "phxrpc/rpc.h"

static phxrpc::ClientMonitorPtr global_addrbookclient_monitor_;

class AddrbookClientLongConnRegister
{
public:
    AddrbookClientLongConnRegister() {
        phxrpc::ClientConfigRegistry::GetDefault()->Register("addrbook");
    }
    ~AddrbookClientLongConnRegister() {

    }
};

static AddrbookClientLongConnRegister g_addrbookclient_register;

AddrbookClientLongConn :: AddrbookClientLongConn()
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

AddrbookClientLongConn :: ~AddrbookClientLongConn()
{
}

phxrpc::BaseTcpStream * AddrbookClientLongConn :: GetSocket()
{
    const phxrpc::Endpoint_t * ep = config_->GetRandom();
    if(ep == nullptr) return nullptr;

    char node[ 128 ] = { 0 };
    snprintf( node, sizeof( node ), "%s:%d", ep->ip, ep->port );

    if( socket_map_.end() != socket_map_.find( node ) ) {
        return socket_map_[ node ];
    }

    phxrpc::BaseTcpStream * socket = NULL;
    bool open_ret = false;

    phxrpc::BlockTcpStream * tmp = new phxrpc::BlockTcpStream;
    open_ret = phxrpc::BlockTcpUtils::Open( tmp, ep->ip, ep->port,
            config_->GetConnectTimeoutMS(), NULL, 0 );
    socket = tmp;

    if ( ! open_ret ) {
        delete socket;
        return nullptr;
    }

    socket->SetTimeout( config_->GetSocketTimeoutMS() );

    socket_map_.insert( std::make_pair( node, socket ) );

    return socket;
}

int AddrbookClientLongConn :: PHXEcho( const google::protobuf::StringValue & req,
        google::protobuf::StringValue * resp )
{
    if(!config_) {
        phxrpc::log(LOG_ERR, "%s %s config is NULL", __func__, package_name_.c_str());
        return -1;
    }

    phxrpc::BaseTcpStream * socket = GetSocket();
    if( NULL == socket ) return -1;
    AddrbookStub stub(*socket, *(global_addrbookclient_monitor_.get()));
    stub.SetConfig(config_);
    stub.SetKeepAlive(true);
    return stub.PHXEcho(req, resp);
}


int AddrbookClientLongConn :: Set( const addrbook::ContactReq & req,
        google::protobuf::Empty * resp )
{
    if(!config_) {
        phxrpc::log(LOG_ERR, "%s %s config is NULL", __func__, package_name_.c_str());
        return -1;
    }

    phxrpc::BaseTcpStream * socket = GetSocket();
    if( NULL == socket ) return -1;
    AddrbookStub stub(*socket, *(global_addrbookclient_monitor_.get()));
    stub.SetConfig(config_);
    stub.SetKeepAlive(true);
    return stub.Set(req, resp);
}

int AddrbookClientLongConn :: GetAll( const google::protobuf::StringValue & req,
        addrbook::ContactList * resp )
{
    if(!config_) {
        phxrpc::log(LOG_ERR, "%s %s config is NULL", __func__, package_name_.c_str());
        return -1;
    }


    phxrpc::BaseTcpStream * socket = GetSocket();
    if( NULL == socket ) return -1;
    AddrbookStub stub(*socket, *(global_addrbookclient_monitor_.get()));
    stub.SetConfig(config_);
    stub.SetKeepAlive(true);
    return stub.GetAll(req, resp);
}

int AddrbookClientLongConn :: GetOne( const addrbook::GetOneReq & req,
        addrbook::Contact * resp )
{
    if(!config_) {
        phxrpc::log(LOG_ERR, "%s %s config is NULL", __func__, package_name_.c_str());
        return -1;
    }

    phxrpc::BaseTcpStream * socket = GetSocket();
    if( NULL == socket ) return -1;
    AddrbookStub stub(*socket, *(global_addrbookclient_monitor_.get()));
    stub.SetConfig(config_);
    stub.SetKeepAlive(true);
    return stub.GetOne(req, resp);
}

int AddrbookClientLongConn :: GetBySeq( const addrbook::GetBySeqReq & req,
        addrbook::ContactList * resp )
{
    if(!config_) {
        phxrpc::log(LOG_ERR, "%s %s config is NULL", __func__, package_name_.c_str());
        return -1;
    }

    phxrpc::BaseTcpStream * socket = GetSocket();
    if( NULL == socket ) return -1;
    AddrbookStub stub(*socket, *(global_addrbookclient_monitor_.get()));
    stub.SetConfig(config_);
    stub.SetKeepAlive(true);
    return stub.GetBySeq(req, resp);
}

