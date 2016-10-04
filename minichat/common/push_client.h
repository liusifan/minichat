
#pragma once

#include <string>

#include "phxrpc/network.h"
#include "phxrpc/rpc.h"

namespace phxrpc {
    class UThreadEpollScheduler;
    class RedisClientConfig;
    class BaseTcpStream;
};

class PushClient {
public:
    PushClient( phxrpc::UThreadEpollScheduler * scheduler = NULL );
    ~PushClient();

    bool Sub( const char * channel );
    
    bool Wait( std::string * msg );

    bool Pub( const char * channel, const char * msg );

private:
    bool Connect( const char * channel );

private:
    phxrpc::UThreadEpollScheduler * scheduler_;
    phxrpc::RedisClientConfig * config_;
    phxrpc::BaseTcpStream * socket_;
};

