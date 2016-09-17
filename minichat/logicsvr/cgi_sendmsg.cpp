
#include "cgi_sendmsg.h"

#include "minichat.pb.h"

#include "msgbox/msgbox_client.h"

CgiSendMsg :: CgiSendMsg()
{
    MsgBoxClient::Init( "~/minichat/etc/client/msgbox_client.conf" );
}

CgiSendMsg :: ~CgiSendMsg()
{
}

int CgiSendMsg :: Process( const minichat::ReqHead & head,
        const std::string & req_buff, std::string * resp_buff )
{
    minichat::SendMsgRequest req_obj;
    minichat::SendMsgResponse resp_obj;

    req_obj.ParseFromString( req_buff );

    // TODO: check req_obj.from is friend of req_obj.to

    MsgBoxClient client;

    for( int i = 0; i < req_obj.msg_size(); i++ ) {
        msgbox::MsgIndex index;
        msgbox::AddMsgResp result;

        const minichat::MsgRequest & msg = req_obj.msg(i);

        // fill index by msg
        index.set_from( head.username() );
        index.set_to( msg.to() );
        index.set_content( msg.content() );
        index.set_createtime( msg.createtime() );
        index.set_uuid( msg.uuid() );
        index.set_seq( time( NULL ) );

        int ret = client.Add( index, &result );

        // append result to resp_obj
        minichat::MsgResponse * resp_msg = resp_obj.add_msg();
        resp_msg->set_uuid( msg.uuid() );
        resp_msg->set_ret( ret );
        resp_msg->set_id( result.id() );

        // TODO: push result.newcount() to receiver
    }

    resp_obj.SerializeToString( resp_buff );

    return 0;
}

