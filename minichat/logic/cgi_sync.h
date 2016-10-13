
#pragma once

#include "cgi_base.h"

class CgiSync : public CgiBase {
public:
    CgiSync( LogicServerConfig & config );
    virtual ~CgiSync();

    virtual int Process( const logic::ReqHead & head,
            const std::string & req_buff, std::string * resp_buff );
};

