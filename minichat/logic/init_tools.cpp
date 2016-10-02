
#include <iostream>
#include <algorithm>
#include <random>
#include <unordered_map>
#include <map>
#include <string>
#include <atomic>

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

typedef unordered_map<string, unordered_map<string, bool> * > ContactMap;

typedef struct _ContactInfo {
    int dis_idx;
    unordered_map<string, bool> *  contact_map;
} ContactInfo;

typedef unordered_map<string, ContactInfo> AllContactMap;

typedef struct _UserDistributeInfo {
    int lower_bound;
    int upper_bound;
    double ratio;
} UserDisInfo;

int user_dis_info_cnt = 11;
UserDisInfo user_dis_info[] = {
    {0,9,0.1104},
    {10,29,0.2203},
    {30,59,0.1567},
    {60,89,0.1150},
    {90,199,0.3330},
    {200,699,0.0390},
    {700,999,0.0116},
    {1000,2000,0.0098},
    {2000,3000,0.0022},
    {3000,4000,0.0017},
    {4000,5000,0.0002}

/*
 *    {0,9,0.1104},
 *    {10,39,0.2203},
 *    {40,69,0.1567},
 *    {70,99,0.1150},
 *    {100,399,0.3330},
 *    {400,699,0.0390},
 *    {700,999,0.0116},
 *    {1000,2000,0.0098},
 *    {2000,3000,0.0022},
 *    {3000,4000,0.0017},
 *    {4000,5000,0.0002}
 *
 */
};

void ShowUsage() 
{
    printf ( "\n" ) ;
    printf ( "init_tools < [-f func] [-k thread cnt] [-v] >\n "
            "  func:\n"
            "       InitUser [-b begin_user_idx] [-e end_user_idx] \n" 
            "       InitContact [-b begin_user_idx] [-e end_user_idx] \n" 
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
        ContactMap ** range_contact_map,
        ContactMap * contact_map,
        AllContactMap & all_map)
{
    cout << "SetContact contact_num_lower_bound " << contact_num_lower_bound 
        << " contact_num_upper_bound " << contact_num_upper_bound 
        << " contact_map size " << contact_map->size() << endl;

    int seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine ran_eng(seed); 
    std::uniform_int_distribution<int> all_dis(begin_user_idx, end_user_idx - 1);
    std::uniform_int_distribution<int> dis(contact_num_lower_bound, contact_num_upper_bound);

    char username[64];
    for(auto & it : *contact_map) {
        int contact_num = dis(ran_eng);
        int total_user_cnt = end_user_idx - begin_user_idx;
        if(contact_num >= total_user_cnt) {
            contact_num = total_user_cnt;
        }
        while(it.second->size() < (size_t)contact_num) {
                int user_idx = all_dis(ran_eng);
                snprintf(username, sizeof(username), "user%d", user_idx);
                if(it.second->find(username) != it.second->end()) {
                    continue;
                }

                if(all_map.find(username) == all_map.end()) {

                    int idx = 0;
                    unordered_map<string, bool> * m = new unordered_map<string, bool>;
                    (*range_contact_map)[idx][username] = m;

                    ContactInfo contact_info;
                    contact_info.dis_idx = idx;
                    contact_info.contact_map = m;
                    all_map[username] = contact_info;

                    (*it.second)[username] = true;
                    (*m)[it.first] = true;

                } else {
                    ContactInfo & info = all_map[username];
                    if(info.contact_map->size() >= (size_t)user_dis_info[info.dis_idx].upper_bound) {
                        --total_user_cnt;
                        if(contact_num >= total_user_cnt) {
                            contact_num = total_user_cnt;
                        }
                    } else {
                        (*it.second)[username] = true;
                        (*info.contact_map)[it.first] = true;
                    }

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

    auto begin_it = contact_map->begin();
    std::advance(begin_it, begin_idx);
    auto end_it = contact_map->begin();
    std::advance(end_it, end_idx);

    AddrbookClient client;

    for(; begin_it != end_it; begin_it++) {

        unordered_map<string, bool> * m = begin_it->second;

        for(auto & mit : *m) {


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
                cout << begin_it->first << " set contact to " << mit.first << " failed ret " << ret << endl;
            } else {
                cout << begin_it->first << " set contact to " << mit.first << " success" << endl;
            }
        }
    }
}

void InitContact() 
{
    AllContactMap all_map;
    ContactMap * range_contact_map = new ContactMap[user_dis_info_cnt];

    std::default_random_engine ran_eng(time(NULL)); 
    std::uniform_int_distribution<int> dis1(begin_user_idx, end_user_idx - 1);

    char username[64];
    unordered_map<int, bool> exist_idx_map;

    for(int i = 0; i < user_dis_info_cnt; i++) {
        UserDisInfo dis_info = user_dis_info[i]; 
        int contacted_user_num = (int)((double)(end_user_idx - begin_user_idx) * dis_info.ratio);

        if(contacted_user_num == 0) {
            cout << "contacted_user_num == 0 lower_bound " << dis_info.lower_bound 
                << " upper_bound " << dis_info.upper_bound << endl;
        }

        for(int k = 0; k < contacted_user_num; k++) {
            while(true) {
                int user_idx = dis1(ran_eng);
                if(exist_idx_map.find(user_idx) != exist_idx_map.end()) {
                    continue;
                }
                exist_idx_map[user_idx] = true;

                snprintf(username, sizeof(username), "user%d", user_idx);
                unordered_map<string, bool> * m = new unordered_map<string, bool>;
                range_contact_map[i][username] = m;

                ContactInfo contact_info;
                contact_info.dis_idx = i;
                contact_info.contact_map = m;
                all_map[username] = contact_info;
                break;
            }
        }
    }

    for(int i = user_dis_info_cnt - 1; i >= 0; i--) {
        UserDisInfo dis_info = user_dis_info[i]; 
        SetContact(dis_info.lower_bound,
                dis_info.upper_bound,
                &range_contact_map,
                &range_contact_map[i],
                all_map);
    }


    long long total_cnt = 0;
    int total_user_cnt = 0;
    for(int i = 0; i < user_dis_info_cnt; i++) {

        UserDisInfo dis_info = user_dis_info[i]; 

        ContactMap & m = range_contact_map[i];
        long long range_total_cnt = 0;

        for(auto & mit : m) {
            range_total_cnt += mit.second->size();
            
        }

        total_cnt += range_total_cnt;

        printf("[%d %d] user cnt %zu avg contact cnt %lld\n",
                dis_info.lower_bound,
                dis_info.upper_bound,
                m.size(),
                m.empty()?0:range_total_cnt / m.size() );
        total_user_cnt += m.size();
    }

    printf("total user cnt %d total contact cnt %lld avr contact cnt %lld\n",
            total_user_cnt, total_cnt, total_cnt / (end_user_idx - begin_user_idx + 1));

/*
 *    int interval = all_map.size() / thread_cnt;
 *    if(0 == interval) {
 *        thread_cnt = 1;
 *        interval = all_map.size();
 *    } else if(0 != all_map.size() % thread_cnt) {
 *        thread_cnt += 1;
 *    }
 *
 *    cout << "thread_cnt " << thread_cnt << " all_map size "
 *        << all_map.size() <<
 *        " interval " << interval << endl;
 *    
 *    for(auto & it : all_map) {
 *        cout << it.first << " has " << it.second->size() << endl;
 *    }
 *
 *    std::thread threads[thread_cnt];
 *
 *    for(int i = 0 ; i < thread_cnt; i++) {
 *        threads[i] = std::thread(CallProfile, interval, i, &all_map);
 *    }
 *
 *
 *    for(auto & t : threads) {
 *        t.join();
 *    }
 *
 *    for(auto & it : all_map) {
 *        delete it.second;
 *        it.second = NULL;
 *    }
 *
 *    delete [] range_contact_map;
 */
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
    while ( (c = getopt ( argc, argv, "b:e:f:k:v:?:" )) != EOF ) {
        switch ( c ) {
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



