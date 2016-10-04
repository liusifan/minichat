#include <string>
#include <fstream>
#include <streambuf>
#include <vector>
#include <algorithm>
#include <map>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <random>
#include <thread>

#include <sys/syscall.h>  
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sched.h>

#define gettid() syscall(__NR_gettid)  

#include "phxrpc/file.h"
#include "phxrpc/network.h"
#include "logic_client.h"
#include "minichat_api.h"

using namespace std;
using namespace phxrpc;

typedef struct _Result {
    int ret;
    unsigned long long run_time;
    unsigned int qps;
    struct timeval send_time;
    struct timeval receive_time;

    bool operator < (const _Result & other)
    {
        return run_time < other.run_time;
    }
} Result;

typedef vector<Result> RetVec;

unsigned int begin_qps = 1;
unsigned int max_qps = 1;
unsigned int qps_time_interval = 1;
unsigned int qps_increment = 1;
unsigned int run_cnt_per_uthread = 1;
int thread_cnt = 1;
int uthread_cnt = 1;

RetVec * ret_array[1024];

void ShowUsage()
{
    printf ( "\n" ) ;
    printf ( "fj_tools < [-k thread_cnt] [-w uthread_cnt] [-m run_cnt_per_uthread] "
            "[-q begin_qps] [-e max_qps] [-t qps_time_interval] [-n qps_increment] "
            "[-v] >\n" );
    return;

}

int Auth(phxrpc::UThreadEpollScheduler * scheduler)
{
    MiniChatAPI api(scheduler);

    logic::AuthResponse resp_obj, auth_resp_obj;

    int ret = api.Auth( "ssolid", "123", &resp_obj );

    printf( "%s return %d\n", __func__, ret );
    printf( "resp: {\n%s}\n", resp_obj.DebugString().c_str() );

    ret = api.AutoAuth( &auth_resp_obj );

    printf( "AutoAuth return %d\n", ret );
    printf( "resp: {\n%s}\n", auth_resp_obj.DebugString().c_str() );
    return ret;
}

int WorkerRun(phxrpc::UThreadEpollScheduler * scheduler)
{
    return Auth(scheduler);
}


int UThreadFunc(phxrpc::UThreadEpollScheduler * scheduler,
        phxrpc::UThreadSocket_t * socket,  
        unsigned int qps,
        unsigned int & curr_cnt,
        RetVec * ret_vec)
{
    double time_interval_ms = 1000.0 / (double)qps;
    int seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);
    std::exponential_distribution<double> distribution((double)1.0/(double)time_interval_ms);

    int ret = 0;

    struct timeval begin, end;
    struct timeval begin_time;
    gettimeofday(&begin_time, NULL);

    for(; curr_cnt < run_cnt_per_uthread; ) {

        curr_cnt += 1;

        gettimeofday(&begin, NULL);

        ret = WorkerRun(scheduler);

        gettimeofday(&end, NULL);

        unsigned long long usec = (end.tv_sec - begin.tv_sec) * 1000000 + (end.tv_usec - begin.tv_usec);

        Result result;
        result.ret = ret;
        result.run_time = usec;
        result.qps = qps;
        result.send_time = begin;
        result.receive_time = end;
        ret_vec->push_back(result);

        usec = (end.tv_sec - begin_time.tv_sec) * 1000000 + (end.tv_usec - begin_time.tv_usec);
        if(usec >= qps_time_interval * 1000000) {
            return 1;
        }
        double interval = distribution(generator);;

        UThreadWait(*socket, interval);
    }
    return 0;
}

void * UThreadRun(phxrpc::UThreadEpollScheduler * scheduler,
        RetVec * ret_vec)
{
    int ret = 0;

    int fd = ::socket(AF_INET, SOCK_STREAM, IPPROTO_IP); 
    phxrpc::UThreadSocket_t * socket = scheduler->CreateSocket(fd);

    unsigned int curr_qps = begin_qps;
    unsigned int curr_cnt = 0;
    while(true) {

        ret =  UThreadFunc(scheduler,
                socket,
                curr_qps,
                curr_cnt,
                ret_vec);

        if(1 == ret) {
            curr_qps += qps_increment;
            if(curr_qps > max_qps) {
                curr_qps = max_qps;
            }
        } else {
            break;
        }
    }
    free(socket);
    return NULL;
}


void * ThreadRun(int thread_idx)
{
    RetVec * ret_vec = ret_array[thread_idx];
    ret_vec->reserve(run_cnt_per_uthread * uthread_cnt);

    phxrpc::UThreadEpollScheduler scheduler(64 * 1024, 1000000, false);
    for(int i = 0; i < uthread_cnt; i++) {
        scheduler.AddTask(std::bind(UThreadRun, &scheduler, ret_vec), nullptr);
    }
    scheduler.Run();
    return NULL;
}


int main( int argc, char * *argv )
{

    phxrpc::ClientConfigRegistry::GetDefault()->Stop();
    phxrpc::openlog( argv[0], "~/log/error", LOG_DEBUG);

    extern char *optarg ;
    int c ;
    while ( (c = getopt ( argc, argv, "q:m:t:e:w:n:k:v:?:" )) != EOF ) {
        switch ( c ) {
            case 'k' :
                thread_cnt = atoi(optarg);
                break;
            case 'w' :
                uthread_cnt = atoi(optarg);
                break;
            case 'n' :
                qps_increment = strtoul(optarg, NULL, 10);
                break;
            case 't' :
                qps_time_interval = strtoul(optarg, NULL, 10);
                break;
            case 'e' :
                max_qps = strtoul(optarg, NULL, 10);
                break;
            case 'q' :
                begin_qps = strtoul(optarg, NULL, 10);
                break;
            case 'm' :
                run_cnt_per_uthread = strtoul(optarg, NULL, 10);
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

    srand(time(NULL));

    std::thread * threads[thread_cnt];
    for(int i = 0 ; i < thread_cnt; i++) {
        threads[i] = new std::thread(ThreadRun, i);
        ret_array[i] = new RetVec();
        if(!ret_array[i]) {
            printf("new RetVec failed errno %d\n", errno);
            return 0;
        }
    }


    for(auto & t : threads) {
        t->join();
    }

    for(int i = 0; i < thread_cnt; i++) {

        printf("---------- thread %d ----------\n", i + 1);

        RetVec * tmp_vec = ret_array[i];

        map<int, int> ret_map;

        for(auto & it : *tmp_vec) {
            ret_map[it.ret] += 1;
            printf("ret %d runtime %llu qps %u send time %llu %llu recevie time %llu %llu\n", it.ret, it.run_time, it.qps,
                    (unsigned long long)it.send_time.tv_sec, (unsigned long long)it.send_time.tv_usec,
                    (unsigned long long)it.receive_time.tv_sec, (unsigned long long)it.receive_time.tv_usec);
        }

        for(auto result : ret_map) {
            printf("ret %d count %d\n", result.first,
                    result.second);
        }
    }

    for(int i = 0 ; i < thread_cnt; i++) {
        delete ret_array[i];
        ret_array[i] = NULL;
    }

    return 0;
}



