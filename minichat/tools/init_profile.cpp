
#include "init_tools.h"

#include "profile/profile_client.h"

#include <random>

using namespace std;

int InitProfile( int begin_idx, int end_idx )
{
    std::random_device rd;

    int unit = ( end_idx - begin_idx ) / 100;
    if( unit <= 0 ) unit = 1;

    char username[ 64 ] = { 0 };

    int k = 0;
    int ret = 0;

    for(int i = begin_idx; i < end_idx; i++) {

        snprintf(username, sizeof(username), "user%d", i);

        profile::Setting pro_req;
        google::protobuf::Empty pro_resp;

        pro_req.set_username(username);
        pro_req.set_avatar("user_avatar.jpg");

        pro_req.set_push_without_detail( ( 0 == ( rd() % 100 ) ) ? 1 : 0 );

        ProfileClient profile_client;

        for(k = 0; k < 3; k++) {
            ret = profile_client.Set( pro_req, &pro_resp );
            if(0 != ret) {
                continue;
            } else {
                break;
            }
        }

        if(3 == k) {
            cout << "set profile " << username << " failed ret " << ret << endl;
            continue;
        }

        if( i > 0 && ( 0 == i % unit ) ) {
            printf( "#" );
            fflush( stdout );
        }
    }

    printf( "\nprofile %d - %d done\n", begin_idx, end_idx );

    return 0;
}

