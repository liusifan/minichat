
#pragma once

#include "cgi_base.h"

class CgiSendMsg : public CgiBase {
public:
    CgiSendMsg( LogicServerConfig & config );
    virtual ~CgiSendMsg();

    virtual int Process( const logic::ReqHead & head,
            const std::string & req_buff, std::string * resp_buff );
};

