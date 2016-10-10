
#include "cgi_sendmsg.h"

#include "logic.pb.h"

#include "msgbox/msgbox_client.h"
#include "seq/seq_client.h"
#include "account/account_client.h"
#include "addrbook/addrbook_client.h"
#include "profile/profile_client.h"

#include "common/push_client.h"
#include "common/push_client_factory.h"
#include "logic_monitor.h"

#include <string.h>

CgiSendMsg :: CgiSendMsg()
{
    business_name_ = std::string("Sendmsg");
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

    // 3. fill index by msg, add to msgbox
    msgbox::MsgIndex index;
    index.set_from( head.username() );
    index.set_to( msg.to() );
    index.set_content( msg.content() );
    index.set_createtime( msg.createtime() );
    index.set_uuid( msg.uuid() );

    MsgBoxClient msgbox_client;

    LogicMonitor::GetDefault()->ReportMsgClientCount();
    ret = msgbox_client.Add( index, result );
    if(0 == ret) {
        LogicMonitor::GetDefault()->ReportMsgClientFailCount();
    } else {
        LogicMonitor::GetDefault()->ReportMsgClientSuccCount();
    }
    return ret;
}

int NotifyReceiver( const char * from, const char * to,
        int new_count, const char * content )
{
    google::protobuf::StringValue req;
    profile::Setting setting;

    ProfileClient prof_client;
    req.set_value( to );

    // set push_without_detail as true when access profile fail
    if( 0 != prof_client.Get( req, &setting ) ) {
        setting.set_push_without_detail( true );
    }

    std::string channel;
    PushClient::Username2Channel( to, &channel );

    char push_msg[ 128 ] = { 0 };

    if( setting.push_without_detail() ) {
        snprintf( push_msg, sizeof( push_msg ), "%s newcount %d, last msg from %s",
                to, new_count, from );
    } else {
        char digest[ 16 ] = { 0 };
        strncpy( digest, content, sizeof( digest ) - 4 );
        // filter special char "
        for( size_t i = 0; i < sizeof( digest ); i++ ) if( '"' == digest[i] ) digest[i] = ' ' ;
        strncat( digest, "...", sizeof( digest ) - 1 );

        snprintf( push_msg, sizeof( push_msg ), "%s newcount %d, last msg from %s, digest %s",
                to, new_count, from, digest );
    }
    PushClientFactory::GetDefault()->Get().Pub( channel.c_str(), push_msg );

    return 0;
}

int CgiSendMsg :: Process( const logic::ReqHead & head,
        const std::string & req_buff, std::string * resp_buff )
{

    LogicMonitor::GetDefault()->ReportCgiSendmsgCount();
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

        // push result.newcount() to receiver
        if( 0 == ret ) {
            NotifyReceiver( head.username().c_str(), msg.to().c_str(),
                    result.newcount(), msg.content().c_str() );
        }
    }

    resp_obj.SerializeToString( resp_buff );

    return 0;
}

