
#pragma once

#include "cgi_base.h"

class CgiAuth : public CgiBase {
public:
    CgiAuth( LogicServerConfig & config );
    virtual ~CgiAuth();

    virtual int Process( const logic::ReqHead & head,
            const std::string & req_buff, std::string * resp_buff );
};

