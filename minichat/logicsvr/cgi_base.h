
#pragma once

#include "minichat.pb.h"

#include <string>

#include "minichat.pb.h"

class CgiBase {
public:
    CgiBase();
    virtual ~CgiBase();

    int Execute( const minichat::MiniRequest & req, minichat::MiniResponse * resp );

    virtual int Process( const minichat::ReqHead & head,
            const std::string & req_buff, std::string * resp_buff ) = 0;
};

