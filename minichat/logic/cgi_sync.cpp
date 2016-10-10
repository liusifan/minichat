
#include "cgi_sync.h"

#include "logic.pb.h"

#include "msgbox/msgbox_client.h"
#include "seq/seq_client.h"
#include "account/account_client.h"
#include "addrbook/addrbook_client.h"
#include "logic_monitor.h"

CgiSync :: CgiSync()
{
    business_name_ = std::string("Sync");
}

CgiSync :: ~CgiSync()
{
}

static int SyncContact( const logic::ReqHead & head,
       const  seq::SyncKey & client_sync_key,
       seq::SyncKey * resp_sync_key,
       logic::SyncResponse * resp_obj )
{
    AddrbookClient addr_client;

    addrbook::GetBySeqReq get_by_seq_req;
    addrbook::ContactList list;

    get_by_seq_req.set_username( head.username() );
    get_by_seq_req.set_seq( client_sync_key.contact_seq() );

    int ret = addr_client.GetBySeq( get_by_seq_req, &list );

    for( int i = 0; i < list.contact_size(); i++ ) {
        const addrbook::Contact & contact = list.contact(i);

        if( contact.is_deleted() ) {
            logic::CmdDelContact del_contact;
            del_contact.set_username( contact.username() );

            logic::CmdItem * item = resp_obj->add_oplog();
            item->set_type( logic::CMD_DEL_CONTACT );
            del_contact.SerializeToString( item->mutable_buff() );
        } else {
            logic::CmdModContact mod_contact;
            mod_contact.set_username( contact.username() );
            mod_contact.set_updatetime( contact.updatetime() );

            logic::CmdItem * item = resp_obj->add_oplog();

            item->set_type( logic::CMD_MOD_CONTACT );
            mod_contact.SerializeToString( item->mutable_buff() );
        }

        if( contact.seq() > resp_sync_key->contact_seq() ) {
            resp_sync_key->set_contact_seq( contact.seq() );
        }
    }

    return ret;
}

static int SyncMsg( const logic::ReqHead & head,
       const  seq::SyncKey & client_sync_key,
       seq::SyncKey * resp_sync_key,
       logic::SyncResponse * resp_obj )
{
    MsgBoxClient msgbox_client;

    msgbox::GetBySeqReq get_by_seq_req;
    msgbox::MsgIndexList list;

    get_by_seq_req.set_username( head.username() );
    get_by_seq_req.set_seq( client_sync_key.msg_seq() );

    LogicMonitor::GetDefault()->ReportMsgClientCount();
    int ret = msgbox_client.GetBySeq( get_by_seq_req, &list );
    if(0 == ret) {
        LogicMonitor::GetDefault()->ReportMsgClientSuccCount();
    } else {
        LogicMonitor::GetDefault()->ReportMsgClientFailCount();
    }


    for( int i = 0; i < list.msg_size(); i++ ) {
        const msgbox::MsgIndex & msg = list.msg( i );

        logic::CmdAddMsg add_msg;
        add_msg.set_id( msg.id() );
        add_msg.set_from( msg.from() );
        add_msg.set_to( msg.to() );
        add_msg.set_content( msg.content() );
        add_msg.set_createtime( msg.createtime() );

        logic::CmdItem * item = resp_obj->add_oplog();

        item->set_type( logic::CMD_ADD_MSG );
        add_msg.SerializeToString( item->mutable_buff() );

        if( msg.seq() > resp_sync_key->msg_seq() ) {
            resp_sync_key->set_msg_seq( msg.seq() );
        }
    }

    return ret;
}

int CgiSync :: Process( const logic::ReqHead & head,
            const std::string & req_buff, std::string * resp_buff )
{
    LogicMonitor::GetDefault()->ReportCgiSyncCount();
    int ret = 0;
    logic::SyncRequest req_obj;
    logic::SyncResponse resp_obj;

    req_obj.ParseFromString( req_buff );

    seq::SyncKey client_sync_key, last_sync_key, resp_sync_key;

    client_sync_key.ParseFromString( req_obj.sync_key() );
    resp_sync_key = client_sync_key;

    //1. get last_sync_key
    SeqClient seq_client;
    {
        google::protobuf::StringValue username;

        username.set_value( head.username() );
        ret = seq_client.Get( username, &last_sync_key );

        if( 0 != ret ) {
            LogicMonitor::GetDefault()->ReportCgiSyncFailCount();
            return ret;
        }
    }

    //2. sync contact
    if( last_sync_key.contact_seq() > client_sync_key.contact_seq() ) {
        ret = SyncContact( head, client_sync_key, &resp_sync_key, &resp_obj );
    }

    //3. sync msg
    if( last_sync_key.msg_seq() > client_sync_key.msg_seq() ) {
        ret = SyncMsg( head, client_sync_key, &resp_sync_key, &resp_obj );
    }

    //4. return new_sync_key
    resp_sync_key.SerializeToString( resp_obj.mutable_new_sync_key() );

    resp_obj.SerializeToString( resp_buff );

    if(0 == ret) {
        LogicMonitor::GetDefault()->ReportCgiSyncSuccCount();
    } else {
        LogicMonitor::GetDefault()->ReportCgiSyncFailCount();
    }

    return ret;
}

