
#include "cgi_sendmsg.h"

#include "logic.pb.h"

#include "msgbox/msgbox_client.h"
#include "seq/seq_client.h"
#include "account/account_client.h"
#include "addrbook/addrbook_client.h"

CgiSendMsg :: CgiSendMsg()
{
    MsgBoxClient::Init( "~/minichat/etc/client/msgbox_client.conf" );
    SeqClient::Init( "~/minichat/etc/client/seq_client.conf" );
    AccountClient::Init( "~/minichat/etc/client/account_client.conf" );
    AddrbookClient::Init( "~/minichat/etc/client/addrbook_client.conf" );
}

CgiSendMsg :: ~CgiSendMsg()
{
}

static int AddOneMsg( const logic::ReqHead & head, const logic::MsgRequest & msg,
        msgbox::AddMsgResp * result )
{
    int ret = 0;

    // 1. check receiver
    AccountClient acct_client;
    {
        google::protobuf::StringValue username;
        username.set_value( msg.to() );

        account::User user;
        ret = acct_client.Get( username, &user );

        if( 0 != ret ) return -2;
    }

    // 2. check relation
    AddrbookClient addr_client;
    {
        addrbook::Contact contact;

        addrbook::GetOneReq one_req;
        one_req.set_username( msg.to() );
        one_req.set_contact( head.username() );

        ret = addr_client.GetOne( one_req, &contact );

        if( 0 != ret ) return -3;
    }

    msgbox::MsgIndex index;

    // 3. alloc seq
    SeqClient seq_client;
    {
        seq::AllocReq seq_req;
        google::protobuf::UInt32Value seq_resp;

        seq_req.set_username( head.username() );
        seq_req.set_type( seq::TYPE_MSG );

        seq_client.Alloc( seq_req, &seq_resp );

        index.set_seq( seq_resp.value() );
    }

    // 4. fill index by msg, add to msgbox
    index.set_from( head.username() );
    index.set_to( msg.to() );
    index.set_content( msg.content() );
    index.set_createtime( msg.createtime() );
    index.set_uuid( msg.uuid() );

    MsgBoxClient msgbox_client;

    return msgbox_client.Add( index, result );
}

int CgiSendMsg :: Process( const logic::ReqHead & head,
        const std::string & req_buff, std::string * resp_buff )
{
    logic::SendMsgRequest req_obj;
    logic::SendMsgResponse resp_obj;

    req_obj.ParseFromString( req_buff );
 
    for( int i = 0; i < req_obj.msg_size(); i++ ) {
        msgbox::MsgIndex index;
        msgbox::AddMsgResp result;

        const logic::MsgRequest & msg = req_obj.msg(i);

        int ret = AddOneMsg( head, msg, &result );

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

