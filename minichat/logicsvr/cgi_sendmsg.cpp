
#include "cgi_sendmsg.h"

#include "logic.pb.h"

#include "msgbox/msgbox_client.h"
#include "seq/seq_client.h"

CgiSendMsg :: CgiSendMsg()
{
    MsgBoxClient::Init( "~/minichat/etc/client/msgbox_client.conf" );
    SeqClient::Init( "~/minichat/etc/client/seq_client.conf" );
}

CgiSendMsg :: ~CgiSendMsg()
{
}

int CgiSendMsg :: Process( const logic::ReqHead & head,
        const std::string & req_buff, std::string * resp_buff )
{
    logic::SendMsgRequest req_obj;
    logic::SendMsgResponse resp_obj;

    req_obj.ParseFromString( req_buff );

    // TODO: check req_obj.from is friend of req_obj.to

    MsgBoxClient msgbox_client;

    for( int i = 0; i < req_obj.msg_size(); i++ ) {
        msgbox::MsgIndex index;
        msgbox::AddMsgResp result;

        const logic::MsgRequest & msg = req_obj.msg(i);

        SeqClient seq_client;
        {
            seq::AllocReq seq_req;
            google::protobuf::UInt32Value seq_resp;

            seq_req.set_username( head.username() );
            seq_req.set_type( seq::TYPE_MSG );

            seq_client.Alloc( seq_req, &seq_resp );

            index.set_seq( seq_resp.value() );
        }

        // fill index by msg
        index.set_from( head.username() );
        index.set_to( msg.to() );
        index.set_content( msg.content() );
        index.set_createtime( msg.createtime() );
        index.set_uuid( msg.uuid() );

        int ret = msgbox_client.Add( index, &result );

        printf( "AddMsg %d\n", ret );

        // append result to resp_obj
        logic::MsgResponse * resp_msg = resp_obj.add_msg();
        resp_msg->set_uuid( msg.uuid() );
        resp_msg->set_ret( ret );
        resp_msg->set_id( result.id() );

        // TODO: push result.newcount() to receiver
    }

    resp_obj.SerializeToString( resp_buff );

    return 0;
}

