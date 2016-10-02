
#include <iostream>
#include <algorithm>
#include <random>
#include <unordered_map>
#include <string>

#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>


#include "account/account_client.h"
#include "addrbook/addrbook_client.h"
#include "profile/profile_client.h"
#include "phxrpc/file.h"

using namespace phxrpc;
using namespace std;


int thread_cnt = 1;
int begin_user_idx = 0;
int end_user_idx = 0;
int most_contacted_num_lower_bound = 3000;
int most_contacted_num_upper_bound = 5000;
int second_most_contacted_num_lower_bound = 500;
int second_most_contacted_num_upper_bound = 3000;
int less_contacted_num_lower_bound = 1;
int less_contacted_num_upper_bound = 500;

typedef unordered_map<string, unordered_map<string, bool> * > ContactMap;

void ShowUsage() 
{
    printf ( "\n" ) ;
    printf ( "init_tools < [-f func] [-k thread cnt] [-v] >\n "
            "  func:\n"
            "       InitUser [-b begin_user_idx] [-e end_user_idx] \n" 
            "       InitContact [-b begin_user_idx] [-e end_user_idx] \n" 
            "                   [-u most_contacted_num_lower_bound] [-i most_contacted_num_upper_bound] \n" 
            "                   [-o second_most_contacted_num_lower_bound] [-p second_most_contacted_num_upper_bound] \n" 
            "                   [-r less_contacted_num_lower_bound] [-t less_contacted_num_upper_bound] \n" 
            "\n" );
}

void InitUserFunc(int begin_idx, int end_idx)
{
    cout << "begin_idx " << begin_idx << " end_idx " << end_idx << endl;
    char username[64];
    char pwd[64];

    for(int i = begin_idx; i < end_idx; i++) {

        snprintf(username, sizeof(username), "user%d", i);
        snprintf(pwd, sizeof(pwd), "pwd%d", i);

        AccountClient account_client;

        account::User user_req;
        google::protobuf::Empty user_resp;
        user_req.set_username(username);
        user_req.set_createtime( time( NULL ) );

        int k = 0;
        int ret = 0;
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
        pwd_req.set_pwd_md5(pwd);

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


        profile::Setting pro_req;
        google::protobuf::Empty pro_resp;

        pro_req.set_username(username);
        pro_req.set_avatar("user_avatar.jpg");
        pro_req.set_push_without_detail( 1 );

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
        } else {
            cout << "set profile " << username << " success " << endl;
        }
    }
}


void InitUser()
{
    if(0 == begin_user_idx ||
            0 == end_user_idx ||
            begin_user_idx >= end_user_idx) {
        return;
    }

    int interval = (end_user_idx - begin_user_idx) / thread_cnt;
    if(0 == interval) {
        thread_cnt = 1;
        interval = end_user_idx - begin_user_idx;
    }

    cout << "thread_cnt " << thread_cnt << " begin_user_idx "
        << begin_user_idx << " end_user_idx " << end_user_idx <<
        " interval " << interval << endl;

    std::thread threads[thread_cnt];

    for(int i = 0 ; i < thread_cnt; i++) {

        int tmp_begin_idx = begin_user_idx + i * interval;
        int tmp_end_idx = begin_user_idx + ( i + 1) * interval;

        if(tmp_end_idx > end_user_idx) {
            break;
        }

        if(i ==  thread_cnt - 1) {
            tmp_end_idx = end_user_idx;
        }

        threads[i] = std::thread(InitUserFunc, tmp_begin_idx, tmp_end_idx);
    }
    for(auto & t : threads) {
        try {
            t.join();
        } catch (const std::exception & ex) {
            cout << ex.what() << endl;
        }
    }
}

void SetContact(int contact_num_lower_bound,
        int contact_num_upper_bound,
        ContactMap & contact_map,
        ContactMap & all_map) 
{


    cout << "SetContact contact_num_lower_bound " << contact_num_lower_bound 
        << " contact_num_upper_bound " << contact_num_upper_bound 
        << " contact_map size " << contact_map.size() << endl;

    std::default_random_engine ran_eng(time(NULL)); 
    std::uniform_int_distribution<int> all_dis(begin_user_idx, end_user_idx - 1);
    std::uniform_int_distribution<int> dis(contact_num_lower_bound, contact_num_upper_bound);
    char username[64];
    for(auto & it : contact_map) {
        int contact_num = dis(ran_eng);
        cout << "contact_num " << contact_num << endl;

        if(contact_num > (end_user_idx - begin_user_idx)) {
            contact_num = end_user_idx - begin_user_idx;
        }
        if(it.second->size() >= (size_t)contact_num) {
            continue;
        }

        for(int i = 0; i < contact_num; i++) {
            while(true) {
                int user_idx = all_dis(ran_eng);
                snprintf(username, sizeof(username), "user%d", user_idx);

                if(it.second->find(username) != it.second->end()) {
                    continue;
                }

                (*it.second)[username] = true;

                unordered_map<string, bool> * m = NULL;
                if(all_map.find(username) == all_map.end()) {
                    m = new unordered_map<string, bool>;
                    all_map[username] = m;
                } else {
                    m = all_map[username];
                }
                (*m)[it.first] = true;
                break;
            }
        }
    }

}

void CallProfile(int interval, int idx, 
        ContactMap * contact_map) 
{

    int begin_idx = interval * idx;
    int end_idx = interval * (idx + 1);

    if((size_t)begin_idx >= contact_map->size()) {
        return;
    }

    if((size_t)end_idx >= contact_map->size()) {
        end_idx = contact_map->size();
    }


    cout << "CallProfile begin_idx " << begin_idx << " end_idx " << end_idx << endl;

    auto begin_it = contact_map->begin();
    std::advance(begin_it, begin_idx);
    auto end_it = contact_map->begin();
    std::advance(end_it, end_idx);

    AddrbookClient client;

    for(; begin_it != end_it; begin_it++) {

        //cout << "---------- " << begin_it.first << "-----------------" << endl;
        unordered_map<string, bool> * m = begin_it->second;
        for(auto & mit : *m) {
            //cout << "        " << mit.first << endl;

            addrbook::ContactReq req;
            google::protobuf::Empty resp;

            req.set_username(begin_it->first);
            req.mutable_contact()->set_username(mit.first);
            req.mutable_contact()->set_remark(mit.first);

            int k = 0;
            int ret = 0;
            for(k = 0; k < 3; k++) {
                ret = client.Set( req, &resp );
                if(0 != ret) {
                    continue;
                } else {
                    break;
                }
            }
            if(k == 3) {
                cout << "user " << begin_it->first << " set contact to " << mit.first << " failed ret " << ret << endl;
            } else {
                cout << "user " << begin_it->first << " set contact to " << mit.first << " success" << endl;
            }
        }
    }
}

void InitContact() 
{
    ContactMap all_map;
    ContactMap most_contacted_map;
    ContactMap second_most_contacted_map;
    ContactMap less_contacted_map;
    std::default_random_engine ran_eng(time(NULL)); 
    std::uniform_int_distribution<int> dis1(begin_user_idx, end_user_idx - 1);

    int most_contacted_user_num = (int)((double)(end_user_idx - begin_user_idx) * 0.05);
    int second_most_contacted_user_num = (int)((double)(end_user_idx - begin_user_idx) * 0.1);
    int less_contacted_user_num = (int)((double)(end_user_idx - begin_user_idx) * 0.85);


    cout << "most_contacted_user_num " << most_contacted_user_num << " second_most_contacted_user_num"
        << second_most_contacted_user_num << " less_contacted_user_num " << less_contacted_user_num << endl;

    char username[64];

    for(int i = 0; i < most_contacted_user_num; i++) {
        while(true) {
            int user_idx = dis1(ran_eng);
            snprintf(username, sizeof(username), "user%d", user_idx);
            if(most_contacted_map.find(username) != most_contacted_map.end()) {
                continue;
            } else {
                unordered_map<string, bool> * m = new unordered_map<string, bool>;
                most_contacted_map[username] = m;
                all_map[username] = m;
                break;
            }
        }
    }

    SetContact(most_contacted_num_lower_bound,
            most_contacted_num_upper_bound,
            most_contacted_map,
            all_map); 

    for(int i = 0; i < second_most_contacted_user_num; i++) {
        while(true) {
            int user_idx = dis1(ran_eng);
            snprintf(username, sizeof(username), "user%d", user_idx);
            if(second_most_contacted_map.find(username) != second_most_contacted_map.end() ||
                    most_contacted_map.find(username) != most_contacted_map.end()) {
                continue;
            } else {
                unordered_map<string, bool> * m = NULL;
                if(all_map.find(username) == all_map.end()) {
                    m = new unordered_map<string, bool>;
                    all_map[username] = m;
                } else {
                    m = all_map[username];
                }
                second_most_contacted_map[username] = m;
                break;
            }
        }
    }

    SetContact(second_most_contacted_num_lower_bound,
            second_most_contacted_num_upper_bound,
            second_most_contacted_map,
            all_map); 

    for(int i = 0; i < less_contacted_user_num; i++) {
        while(true) {
            int user_idx = dis1(ran_eng);
            snprintf(username, sizeof(username), "user%d", user_idx);
            if(less_contacted_map.find(username) != less_contacted_map.end() ||
                    second_most_contacted_map.find(username) != second_most_contacted_map.end() ||
                    most_contacted_map.find(username) != most_contacted_map.end()) {
                continue;
            } else {
                unordered_map<string, bool> * m = NULL;
                if(all_map.find(username) == all_map.end()) {
                    m = new unordered_map<string, bool>;
                    all_map[username] = m;
                } else {
                    m = all_map[username];
                }
                less_contacted_map[username] = m;
                break;
            }
        }
    }

    SetContact(less_contacted_num_lower_bound,
            less_contacted_num_upper_bound,
            less_contacted_map,
            all_map); 




    int interval = all_map.size() / thread_cnt;
    if(0 == interval) {
        thread_cnt = 1;
        interval = all_map.size();
    } else if(0 != all_map.size() % thread_cnt) {
        thread_cnt += 1;
    }

    cout << "thread_cnt " << thread_cnt << " all_map size "
        << all_map.size() <<
        " interval " << interval << endl;

    std::thread threads[thread_cnt];

    for(int i = 0 ; i < thread_cnt; i++) {
        threads[i] = std::thread(CallProfile, interval, i, &all_map);
    }


    for(auto & t : threads) {
        t.join();
    }

    for(auto & it : all_map) {
        delete it.second;
        it.second = NULL;
    }
}

int main( int argc, char * *argv )
{

    phxrpc::ClientConfigRegistry::GetDefault()->Stop();

    phxrpc::openlog( argv[0], "~/log/error", LOG_DEBUG);

    if(argc < 2) {
        ShowUsage();
        return -1;
    }

    extern char *optarg ;
    int c ;
    const char * func = NULL;
    while ( (c = getopt ( argc, argv, "u:i:o:p:r:t:b:e:f:k:v:?:" )) != EOF ) {
        switch ( c ) {

            case 'u' :
                most_contacted_num_lower_bound = atoi(optarg);
                break;
            case 'i' :
                most_contacted_num_upper_bound = atoi(optarg);
                break;
            case 'o' :
                second_most_contacted_num_lower_bound = atoi(optarg);
                break;
            case 'p' :
                second_most_contacted_num_upper_bound = atoi(optarg);
                break;
            case 'r' :
                less_contacted_num_lower_bound = atoi(optarg);
                break;
            case 't' :
                less_contacted_num_upper_bound = atoi(optarg);
                break;

            case 'b' :
                begin_user_idx = atoi(optarg);
                break;
            case 'e' :
                end_user_idx = atoi(optarg);
                break;
            case 'f':
                func = optarg;
                break;
            case 'k' :
                thread_cnt = atoi(optarg);
                break;
            case 'v' : 
            case '?' :
                ShowUsage() ;
                return -1 ;
                break ;
            default :
                ShowUsage() ;
                return -1 ;
                break ;
        }
    }

    if(thread_cnt > 1024) {
        thread_cnt = 1024;
    }


    if(0 == strcasecmp("InitUser", func)) {
        InitUser();
    } else if(0 == strcasecmp("InitContact", func)) {
        InitContact();
    } else {
        cout << "func " << func << " is not defined " << endl;
    } 
    return 0;
}



