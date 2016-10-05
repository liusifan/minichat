
#pragma once

#include <string>
#include <map>

#include "phxrpc/network.h"
#include "phxrpc/rpc.h"

namespace phxrpc {
    class UThreadEpollScheduler;
    class RedisClientConfig;
    class BaseTcpStream;
};

class PushClient {
public:
    static int const USER_PER_CHANNEL = 100;

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

private:
    phxrpc::UThreadEpollScheduler * scheduler_;
    phxrpc::RedisClientConfig * config_;

    std::map< string, phxrpc::BaseTcpStream * > socket_map_;
};
