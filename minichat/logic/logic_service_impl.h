/* logic_service_impl.h

 Generated by phxrpc_pb2service from logic.proto

*/

#pragma once

#include "logic.pb.h"
#include "phxrpc_logic_service.h"

class LogicServerConfig;

typedef struct tagServiceArgs {
    LogicServerConfig * config;
    //You can add other arguments here and initiate in main().
}ServiceArgs_t;

class LogicServiceImpl : public LogicService
{
public:
    LogicServiceImpl( ServiceArgs_t & args );
    virtual ~LogicServiceImpl();

    virtual int PHXEcho( const google::protobuf::StringValue & req,
        google::protobuf::StringValue * resp );

    virtual int Auth( const logic::MiniRequest & req,
        logic::MiniResponse * resp );

    virtual int Sync( const logic::MiniRequest & req,
        logic::MiniResponse * resp );

    virtual int SendMsg( const logic::MiniRequest & req,
        logic::MiniResponse * resp );

private:
    ServiceArgs_t & args_;
};

