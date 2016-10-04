
#include "push_client.h"

#include "r3c/r3c.h"

#include "phxrpc/rpc.h"

#include <functional>

PushClient :: PushClient( phxrpc::UThreadEpollScheduler * scheduler )
{
    scheduler_ = scheduler;

    config_ = new phxrpc::RedisClientConfig();

    const char * path = "~/etc/route/shanghai/mmminichat_route.conf";

    if( ! config_->Read( path ) ) {
        phxrpc::log( LOG_ERR, "read config %s failed", path );
    }

    socket_ = NULL;
}

PushClient :: ~PushClient()
{
    if( NULL != config_ ) delete config_;
    if( NULL != socket_ ) delete socket_;
}

bool PushClient :: Connect( const char * channel )
{
    if( NULL != socket_ ) return true;

    std::hash<std::string> h;
    size_t index = h( channel ) % config_->GetCount();

    const phxrpc::Endpoint_t * ep = config_->GetByIndex( index );

    if(ep == nullptr) return false;

    bool open_ret = false;

    if( NULL == scheduler_ ) {
        phxrpc::BlockTcpStream * tmp = new phxrpc::BlockTcpStream;
        open_ret = phxrpc::BlockTcpUtils::Open( tmp, ep->ip, ep->port,
                config_->GetConnectTimeoutMS(), NULL, 0 );
        socket_ = tmp;
    } else {
        phxrpc::UThreadTcpStream * tmp = new phxrpc::UThreadTcpStream;
        open_ret = phxrpc::UThreadTcpUtils::Open( scheduler_, tmp, ep->ip, ep->port,
                    config_->GetConnectTimeoutMS() );
        socket_ = tmp;
    }

    if ( ! open_ret ) {
        delete socket_;
        return false;
    }

    socket_->SetTimeout( config_->GetSocketTimeoutMS() );

    return true;
}

bool PushClient :: Sub( const char * channel )
{
    if( ! Connect( channel ) ) return false;

    socket_->SetTimeout( 3600 * 1000 );

    (*socket_) << "SUBSCRIBE " << channel << std::endl;

    if( ! socket_->flush().good() ) return false;

    char line[1024] = {0};

    // read success response
    for( ; ; ) {
        if( ! socket_->getlineWithTrimRight(line, sizeof(line)).good() ) return false;
        if( ':' == line[0] ) return true;
    }

    return false;
}

bool PushClient :: Wait( std::string * msg )
{
    if( NULL == socket_ ) return false;

    char line[1024] = {0};

    // read start line
    if( ! socket_->getlineWithTrimRight(line, sizeof(line)).good() ) return false;
    if( '*' != line[0] ) return false;

    // read unused line
    for( int i = 0; i < 5; i++ ) {
        if( ! socket_->getlineWithTrimRight(line, sizeof(line)).good() ) return false;
    }

    // read real msg
    if( ! socket_->getlineWithTrimRight(line, sizeof(line)).good() ) return false;

    msg->append( line );

    return false;
}

bool PushClient :: Pub( const char * channel, const char * msg )
{
    if( ! Connect( channel ) ) return false;

    (*socket_) << "PUBLISH " << channel << " " << msg << std::endl;

    if( ! socket_->flush().good() ) return false;

    char line[1024] = {0};

    if( ! socket_->getlineWithTrimRight(line, sizeof(line)).good() ) return false;
    if( ':' == line[0] ) return true;

    return false;
}

