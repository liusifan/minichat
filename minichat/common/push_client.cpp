
#include "push_client.h"
#include "redis_client_config.h"

#include "r3c/r3c.h"

#include "phxrpc/rpc.h"

#include <functional>

#include "define.h"

void PushClient :: Username2Channel( const char * username, std::string * channel )
{
    char tmp[ 128 ] = { 0 };

    int index = atoi( username + 4 ) / USER_PER_CHANNEL;
    snprintf( tmp, sizeof( tmp ), "channel_%d", index );

    channel->append( tmp );
}

PushClient :: PushClient( phxrpc::UThreadEpollScheduler * scheduler )
{
    scheduler_ = scheduler;

    config_ = new RedisClientConfig();

    const char * path = MINI_REDIS_CLIENT_CONFIG;

    if( ! config_->Read( path ) ) {
        phxrpc::log( LOG_ERR, "read config %s failed", path );
    }
}

PushClient :: ~PushClient()
{
    if( NULL != config_ ) delete config_;

    for( auto & s : socket_map_ ) {
        delete s.second;
    }
}

phxrpc::BaseTcpStream * PushClient :: GetSocket( const char * channel )
{
    std::hash<std::string> h;
    size_t index = h( channel ) % config_->GetCount();

    const phxrpc::Endpoint_t * ep = config_->GetByIndex( index );

    if(ep == nullptr) return nullptr;

    char node[ 128 ] = { 0 };
    snprintf( node, sizeof( node ), "%s:%d", ep->ip, ep->port );

    if( socket_map_.end() != socket_map_.find( node ) ) {
        return socket_map_[ node ];
    }

    phxrpc::BaseTcpStream * socket = NULL;
    bool open_ret = false;

    if( NULL == scheduler_ ) {
        phxrpc::BlockTcpStream * tmp = new phxrpc::BlockTcpStream;
        open_ret = phxrpc::BlockTcpUtils::Open( tmp, ep->ip, ep->port,
                config_->GetConnectTimeoutMS(), NULL, 0 );
        socket = tmp;
    } else {
        phxrpc::UThreadTcpStream * tmp = new phxrpc::UThreadTcpStream;
        open_ret = phxrpc::UThreadTcpUtils::Open( scheduler_, tmp, ep->ip, ep->port,
                    config_->GetConnectTimeoutMS() );
        socket = tmp;
    }

    if ( ! open_ret ) {
        delete socket;
        return nullptr;
    }

    socket->SetTimeout( config_->GetSocketTimeoutMS() );

    socket_map_.insert( std::make_pair( node, socket ) );

    return socket;
}

bool PushClient :: EraseSocket( phxrpc::BaseTcpStream * socket )
{
    std::string key = "";

    for( auto & s : socket_map_ ) {
        if( s.second == socket ) {
            key = s.first;
            break;
        }
    }

    if( key.size() > 0 ) {
        socket_map_.erase( key );
        delete socket;
    }

    return key.size() > 0;
}

bool PushClient :: Sub( const char * channel )
{
    phxrpc::BaseTcpStream * socket = GetSocket( channel );

    if( nullptr == socket ) return false;

    auto deleter = [this]( phxrpc::BaseTcpStream * socket ) { EraseSocket( socket ); };
    std::unique_ptr< phxrpc::BaseTcpStream, decltype( deleter ) > to_erase( socket, deleter );

    socket->SetTimeout( 3600 * 1000 );

    (*socket) << "SUBSCRIBE " << channel << std::endl;

    if( ! socket->flush().good() ) return false;

    char line[1024] = {0};

    // read success response
    for( ; ; ) {
        if( ! socket->getlineWithTrimRight(line, sizeof(line)).good() ) return false;
        if( ':' == line[0] ) {
            to_erase.release();
            return true;
        }
    }

    return false;
}

bool PushClient :: Wait( const char * channel, std::string * msg )
{
    phxrpc::BaseTcpStream * socket = GetSocket( channel );
    if( NULL == socket ) return false;

    auto deleter = [this]( phxrpc::BaseTcpStream * socket ) { EraseSocket( socket ); };
    std::unique_ptr< phxrpc::BaseTcpStream, decltype( deleter ) > to_erase( socket, deleter );

    char line[1024] = {0};

    // read start line
    if( ! socket->getlineWithTrimRight(line, sizeof(line)).good() ) return false;
    if( '*' != line[0] ) return false;

    // read unused line
    for( int i = 0; i < 5; i++ ) {
        if( ! socket->getlineWithTrimRight(line, sizeof(line)).good() ) return false;
    }

    // read real msg
    if( ! socket->getlineWithTrimRight(line, sizeof(line)).good() ) return false;

    msg->append( line );

    to_erase.release();

    return true;
}

bool PushClient :: Pub( const char * channel, const char * msg )
{
    phxrpc::BaseTcpStream * socket = GetSocket( channel );
    if( NULL == socket ) return false;

    auto deleter = [this]( phxrpc::BaseTcpStream * socket ) { EraseSocket( socket ); };
    std::unique_ptr< phxrpc::BaseTcpStream, decltype( deleter ) > to_erase( socket, deleter );

    (*socket) << "PUBLISH " << channel << " \"" << msg << "\"" << std::endl;

    if( ! socket->flush().good() ) return false;

    char line[1024] = {0};

    if( ! socket->getlineWithTrimRight(line, sizeof(line)).good() ) return false;
    if( ':' == line[0] ) {
        to_erase.release();
        return true;
    }

    return false;
}

