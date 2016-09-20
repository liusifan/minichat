/* profile_server_config.h

 Generated by phxrpc_pb2server from profile.proto

*/

#pragma once

#include "phxrpc/rpc.h"

class ProfileServerConfig
{
public:
    ProfileServerConfig();

    ~ProfileServerConfig();

    bool Read( const char * config_file );

    const phxrpc::HshaServerConfig & GetHshaServerConfig();

private:
    phxrpc::HshaServerConfig ep_server_config_;
};