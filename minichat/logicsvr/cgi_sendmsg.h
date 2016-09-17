
#pragma once

#include "cgi_base.h"

#include "minichat.pb.h"

class CgiSendMsg : public CgiBase {
public:
    CgiSendMsg();
    virtual ~CgiSendMsg();

    virtual int Process( const minichat::ReqHead & head,
            const std::string & req_buff, std::string * resp_buff );
};

