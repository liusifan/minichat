
#include "cgi_base.h"

CgiBase :: CgiBase()
{
}

CgiBase :: ~CgiBase()
{
}

int CgiBase :: Execute( const logic::MiniRequest & req, logic::MiniResponse * resp )
{
    int ret = 0;

    //TODO: process enc_algo

    ret = Process( req.head(), req.req_buff(), resp->mutable_resp_buff() );
    resp->set_ret( ret );

    return ret;
}

