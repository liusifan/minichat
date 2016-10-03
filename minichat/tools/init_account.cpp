
#include "init_tools.h"

#include "account/account_client.h"

using namespace std;

int InitAccount( int begin_idx, int end_idx )
{
    char username[ 64 ] = { 0 };

    int k = 0;
    int ret = 0;

    for(int i = begin_idx; i < end_idx; i++) {

        snprintf(username, sizeof(username), "user%d", i);

        AccountClient account_client;

        account::User user_req;
        google::protobuf::Empty user_resp;
        user_req.set_username(username);
        user_req.set_createtime( time( NULL ) );

        for(k = 0; k < 3; k++) {
            ret = account_client.Set( user_req, &user_resp );
            if(0 != ret) {
                continue;
            } else {
                break;
            }
        }

        if(3 == k) {
            cout << "setuser " << username << " failed ret " << ret << endl;
            continue;
        } else {
            cout << "setuser " << username << " success " << endl;
        }

        account::PwdReq pwd_req;
        google::protobuf::Empty pwd_resp;
        pwd_req.set_username(username);
        pwd_req.set_pwd_md5(username);

        for(k = 0; k < 3; k++) {
            ret = account_client.SetPwd( pwd_req, &pwd_resp );
            if(0 != ret) {
                continue;
            } else {
                break;
            }
        }

        if(3 == k) {
            cout << "setpwd " << username << " failed ret " << ret << endl;
            continue;
        } else {
            cout << "setpwd " << username << " success " << endl;
        }
    }

    return 0;
}

