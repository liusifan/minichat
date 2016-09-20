/* profile_tool_impl.h

 Generated by phxrpc_pb2tool from profile.proto

*/

#pragma once

#include <stdio.h>

#include "phxrpc_profile_tool.h"
namespace phxrpc {
    class OptMap;
}

class ProfileToolImpl : public ProfileTool
{
public:
    ProfileToolImpl();
    virtual ~ProfileToolImpl();

    virtual int PHXEcho( phxrpc::OptMap & opt_map );

    virtual int Set( phxrpc::OptMap & opt_map );

    virtual int Get( phxrpc::OptMap & opt_map );

};

