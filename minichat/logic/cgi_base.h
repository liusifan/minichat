
#pragma once

#include "logic.pb.h"

#include <string>

class CgiBase {
public:
    CgiBase();
    virtual ~CgiBase();

    int Execute( const logic::MiniRequest & req, logic::MiniResponse * resp );

    virtual int Process( const logic::ReqHead & head,
            const std::string & req_buff, std::string * resp_buff ) = 0;
protected:

    std::string business_name_;

private:
    int AESProcess( const logic::MiniRequest & req, logic::MiniResponse * resp );
};

