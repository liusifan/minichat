
#pragma once

#include <string>
#include <vector>

#include "logic.pb.h"

#include "phxrpc/network.h"
#include "phxrpc/rpc.h"

namespace phxrpc {
    class UThreadEpollScheduler;
};

class MiniChatAPI {
public:
    MiniChatAPI( phxrpc::UThreadEpollScheduler * scheduler = NULL );
    ~MiniChatAPI();

    int Auth( const char * username, const char * pwd_md5, logic::AuthResponse * resp_obj );

    int AutoAuth( logic::AuthResponse * resp_obj );

    bool IsAuthOK();

    int SendMsg( const char * to,
            const char * text, logic::SendMsgResponse * resp_obj );

    int Sync( logic::SyncResponse * resp_obj );

public:

    const std::vector<std::string> & GetContacts() const;

    void SetUsername( const char * username );
    const char * GetUsername();

private:
    int Call_L0( const char * uri, int cmd_id,
            const logic::MiniRequest & req, logic::MiniResponse * resp );

    int Call_L1( const char * uri, int cmd_id,
            const google::protobuf::MessageLite & request,
            google::protobuf::MessageLite * response );

private:
    std::string username_;
    std::string session_key_;
    std::vector< std::string> contacts_;

    std::string auto_auth_ticket_;
    std::string auto_auth_aes_key_;

    logic::SyncKey sync_key_;

    phxrpc::UThreadEpollScheduler * scheduler_;

    std::string package_name_;
    phxrpc::ClientConfig * config_;
};

