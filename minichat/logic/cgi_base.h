
#pragma once

#include "logic.pb.h"

#include <string>

class LogicServerConfig;

namespace presence {
    class Session;
};

class CgiBase {
public:
    CgiBase( LogicServerConfig & config );
    virtual ~CgiBase();

    int Execute( const logic::MiniRequest & req, logic::MiniResponse * resp );

    virtual int Process( const logic::ReqHead & head,
            const std::string & req_buff, std::string * resp_buff ) = 0;
protected:

    std::string business_name_;
    LogicServerConfig & config_;

private:
    int AESProcess( const presence::Session & session,
            const logic::MiniRequest & req, logic::MiniResponse * resp );
};

