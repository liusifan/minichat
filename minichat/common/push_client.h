
#pragma once

#include <string>
#include <map>

#include "phxrpc/network.h"
#include "phxrpc/rpc.h"

namespace phxrpc {
    class UThreadEpollScheduler;
    class BaseTcpStream;
};

class RedisClientConfig;

class PushClient {
public:
    static int const USER_PER_CHANNEL = 20;

public:
    static void Username2Channel( const char * username, std::string * channel );

public:
    PushClient( phxrpc::UThreadEpollScheduler * scheduler = NULL );
    ~PushClient();

    bool Sub( const char * channel );
    
    bool Wait( const char * channel, std::string * msg );

    bool Pub( const char * channel, const char * msg );

private:
    phxrpc::BaseTcpStream * GetSocket( const char * channel );

    bool EraseSocket( phxrpc::BaseTcpStream * socket );

private:
    phxrpc::UThreadEpollScheduler * scheduler_;
    RedisClientConfig * config_;

    std::map< string, phxrpc::BaseTcpStream * > socket_map_;
};

