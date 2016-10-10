
#include <thread>
#include <random>
#include <string>
#include <vector>
#include <sys/time.h>

#include "benchmark.h"

#include "phxrpc/network.h"

#include "logic/logic_client.h"
#include "logic/minichat_api.h"

#include "common/push_client.h"
#include "logic_monitor.h"


using namespace std;

typedef struct _MsgFromTo {
    int from;
    string to;
} MsgFromTo;

typedef struct tagBMStat {
    int auth_count, auth_ok_count, auth_fail_count;
    int msg_count;
    int msg_ok_cout;
    int msg_fail_count;

    void Reset() 
    {
        auth_count = 0;
        auth_ok_count = 0;
        auth_fail_count = 0;
        msg_count = 0;
        msg_ok_cout = 0;
        msg_fail_count = 0;
    }
} BMStat_t;

__thread BMStat_t global_bm_stat;

class BMUThread {
public:
    BMUThread( int tag, phxrpc::UThreadEpollScheduler * scheduler,
            int begin_index, const BMArgs_t & args );
    ~BMUThread();

    int StartAuth();

    int StartLoad();

    int Stop();

private:
    int LoadFunc(vector<MsgFromTo> & msg_from_to,
            phxrpc::UThreadSocket_t * socket,  
            double qps,
            time_t begin_time,
            int qps_var_interval_sec,
            int duration_sec); 

private:
    int tag_;
    phxrpc::UThreadEpollScheduler * scheduler_;
    int begin_index_;
    MiniChatAPI ** apis_;

    const BMArgs_t & args_;
    int auth_ok_count_;

    enum { RUNNING = 0, STOPPING = 1, STOPPED = 2 };
    int status_;
};

BMUThread :: BMUThread( int tag, phxrpc::UThreadEpollScheduler * scheduler,
        int begin_index, const BMArgs_t & args )
    : args_( args )
{
    tag_ = tag;

    scheduler_ = scheduler;
    begin_index_ = begin_index;

    apis_ = ( MiniChatAPI ** ) calloc( args_.user_per_uthread, sizeof( MiniChatAPI * ) );

    for( int i = 0; i < args_.user_per_uthread; i++ ) {
        apis_[i] = new MiniChatAPI( scheduler_ );
    }

    auth_ok_count_ = 0;
    status_ = RUNNING;
}

BMUThread :: ~BMUThread()
{
    Stop();

    // wait for stopped
    for( ; STOPPED != status_; ) sleep( 1 );

    for( int i = 0; i < args_.user_per_uthread; i++ ) {
        if( NULL != apis_[i] ) delete apis_[i];
    }

    free( apis_ );
}

int BMUThread :: Stop()
{
    if( STOPPED != status_ ) status_ = STOPPING;

    return 0;
}

int BMUThread :: StartAuth()
{
    phxrpc::__uthread( *scheduler_ ) - [=]( void * ) {
        for( ; RUNNING == status_ && auth_ok_count_ < args_.user_per_uthread; ) {
            for( int i = 0; RUNNING == status_ && i < args_.user_per_uthread; i++ ) {
                if( ! apis_[i]->IsAuthOK() ) {
                    char username[ 128 ] = { 0 };
                    snprintf( username, sizeof( username ), "user%d", begin_index_ + i );

                    ++global_bm_stat.auth_count;
                    LogicMonitor::GetDefault()->ReportAuthCount();
                    

                    logic::AuthResponse auth_resp;
                    if( 0 == apis_[i]->Auth( username, username, &auth_resp, args_.auth_use_rsa ) ) {
                        ++auth_ok_count_;
                        ++global_bm_stat.auth_ok_count;
                        LogicMonitor::GetDefault()->ReportAuthSuccCount();
                    } else {
                        ++global_bm_stat.auth_fail_count;
                        //printf("auth %s failed\n", username);
                        LogicMonitor::GetDefault()->ReportAuthFailCount();
                    }
                }
            }
        }
    };

    return 0;
}

int BMUThread::LoadFunc(vector<MsgFromTo> & msg_from_to,
        phxrpc::UThreadSocket_t * socket,  
        double qps,
        time_t begin_time,
        int qps_var_interval_sec,
        int duration_sec)
{
    if(0 == msg_from_to.size()) {
        return 0;
    }

    int seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);
    double time_interval_ms = 1000.0 / (double)qps;
    std::exponential_distribution<double> distribution((double)1.0/(double)time_interval_ms);

    struct timeval begin, end;
    struct timeval round_begin_time;
    int ret = 0;
    gettimeofday(&round_begin_time, NULL);

    while(true) {

        gettimeofday(&begin, NULL);

        const MsgFromTo & item = msg_from_to[ generator() % msg_from_to.size() ];

        logic::SendMsgResponse resp;
        ret = apis_[item.from]->SendMsg( item.to.c_str(), "msg from ", &resp );

        if(0 == ret) {
            int msg_size = resp.msg_size();
            for(int i = 0; i < msg_size; i++) {
                ++global_bm_stat.msg_count;
                LogicMonitor::GetDefault()->ReportMsgCount();


               const ::logic::MsgResponse& msg_response =  resp.msg(i);
               if(0 == msg_response.ret()) {
                   ++global_bm_stat.msg_ok_cout;
                   LogicMonitor::GetDefault()->ReportMsgSuccCount();
               } else {
                   ++global_bm_stat.msg_fail_count;
                   LogicMonitor::GetDefault()->ReportMsgFailCount();
               }
            }
        } else {
            ++global_bm_stat.msg_fail_count;
        }

        gettimeofday(&end, NULL);
        unsigned long long run_time = (end.tv_sec - begin.tv_sec) * 1000000 + (end.tv_usec - begin.tv_usec);

        if( 0 == ( global_bm_stat.msg_count % 1000 ) ) {
            //printf( "send msg from %s to %s cost %lld run_time\n",
                //apis_[item.from]->GetUsername(), item.to.c_str(),
                //run_time );
            printf( "tag %d, msg_count %d msg_ok_cout %d msg_fail_count %d\n", tag_, global_bm_stat.msg_count,
                 global_bm_stat.msg_ok_cout, global_bm_stat.msg_fail_count );
        }

        int interval = (int)distribution(generator);;
        interval -= run_time/1000;
        if(0 > interval) {
            interval = 0;
        }

        unsigned long long use_time = (end.tv_sec - round_begin_time.tv_sec) * 1000000 + (end.tv_usec - round_begin_time.tv_usec);
        if(use_time >= (unsigned long long)qps_var_interval_sec * 1000000) {
            return 1;
        }

        if(interval > 0) {
            UThreadWait(*socket, interval);
        }

        time_t curr_time = time(nullptr);
        if((curr_time - begin_time) > (time_t)duration_sec) {
            break;
        }
    }
    return 0;
}


int BMUThread :: StartLoad()
{
#if 0 // use sync_tools to sync
    // waiting for push
    phxrpc::__uthread( *scheduler_ ) - [=]( void * ) {
        PushClient client( scheduler_ );

        char channel[ 128 ] = { 0 };
        snprintf( channel, sizeof( channel ), "channel_%d",
                begin_index_ / PushClient::USER_PER_CHANNEL );

        client.Sub( channel );

        for( ; RUNNING == status_; ) {
            std::string msg;
            client.Wait( channel, &msg );

            if( 0 == strcasecmp( "exit", msg.c_str() ) ) break;
            if( msg.size() < 4 ) continue;

            int index = atoi( msg.c_str() + 4 );
            int offset = index - begin_index_;

            if( offset < args_.user_per_uthread && offset >= 0 ) {
                logic::SyncResponse resp;
                apis_[ offset ]->Sync( &resp );
                printf( "sync %s\n", apis_[offset]->GetUsername() );
            }
        }

        status_ = STOPPED;
        printf("stop sync uthread\n");
    };
#endif

    printf( "do sendmsg\n" );

    // do sendmsg
    phxrpc::__uthread( *scheduler_ ) - [=]( void * ) {
        int fd = ::socket(AF_INET, SOCK_STREAM, IPPROTO_IP); 
        phxrpc::UThreadSocket_t * socket = scheduler_->CreateSocket(fd);

        vector<MsgFromTo> msg_from_to;
        // get all contact
        for( int i = 0; i < args_.user_per_uthread; i++ ) {
            logic::SyncResponse resp;
            apis_[i]->Sync( &resp );

            const std::vector< std::string > & contacts = apis_[i]->GetContacts();
            if( contacts.size() == 0 ) continue;
            for(auto & it : contacts) msg_from_to.push_back( { i, it } );
        }

        printf("msg_from_to size %zu\n", msg_from_to.size());

        double curr_qps = 0.0;

        int ret = 0;
        vector<Op> op_vec;

        for(auto & op : args_.op_vec) {

            printf("op: begin_qps %f end_qps %f duration_sec %d qps_variation %f qps_var_interval_sec %d\n",
                    op.begin_qps, op.end_qps, op.duration_sec, op.qps_variation,
                    op.qps_var_interval_sec);

            curr_qps = op.begin_qps;
            time_t begin_time = time(nullptr);

            while(true) {
                ret =  LoadFunc(msg_from_to, socket,
                        curr_qps,
                        begin_time,
                        op.qps_var_interval_sec,
                        op.duration_sec);
                if(1 == ret) {
                    if(0 == op.qps_var_direction) {
                        curr_qps += op.qps_variation;
                        if(curr_qps > op.end_qps) {
                            curr_qps = op.end_qps;
                        }
                    } else {
                        curr_qps -= op.qps_variation;
                        if(curr_qps < op.end_qps) {
                            curr_qps = op.end_qps;
                        }
                    }
                    
                } else {
                    break;
                }

            }
        }

        free(socket);
        PushClient client( scheduler_ );

        char channel[ 128 ] = { 0 };
        snprintf( channel, sizeof( channel ), "channel_%d",
                begin_index_ / PushClient::USER_PER_CHANNEL );

        client.Pub( channel, "exit" );

        status_ = STOPPED;
    };

    return 0;
}

//===================================================================

int BMThread( int tag, int begin_index, const BMArgs_t & args )
{

    global_bm_stat.Reset();

    BMUThread ** uthreads = ( BMUThread ** ) calloc( args.uthread_per_thread, sizeof( BMUThread * ) );

    phxrpc::UThreadEpollScheduler scheduler( 64 * 1024, args.uthread_per_thread * 2 );

    for( int i = 0; i < args.uthread_per_thread; i++ ) {
        uthreads[i] = new BMUThread( tag * 1000 + i, &scheduler,
                begin_index + i * PushClient::USER_PER_CHANNEL, args );
        uthreads[i]->StartAuth();
    }

    printf( "begin auth\n" );

    scheduler.Run();

    printf( "total auth %d, ok %d, fail %d\n", global_bm_stat.auth_count,
            global_bm_stat.auth_ok_count, global_bm_stat.auth_fail_count );

    for( int i = 0; i < args.uthread_per_thread; i++ ) {
        uthreads[i]->StartLoad();
    }

    printf( "begin load\n" );

    scheduler.Run();

    for( int i = 0; i < args.uthread_per_thread; i++ ) {
        delete uthreads[i];
    }

    printf( "total msg %d, ok %d, fail %d\n", global_bm_stat.msg_count,
            global_bm_stat.msg_ok_cout, global_bm_stat.msg_fail_count );

    free( uthreads );
}

int benchmark( const BMArgs_t & args )
{
    std::thread * threads[ args.thread_count ];

    int unit = PushClient::USER_PER_CHANNEL * args.uthread_per_thread;

    for( int i = 0; i < args.thread_count; i++ ) {
        threads[i] = new std::thread( BMThread, i, args.begin_index + i * unit, args );
    }

    for( int i = 0; i < args.thread_count; i++ ) {
        threads[i]->join();
    }

    for( int i = 0; i < args.thread_count; i++ ) {
        delete threads[i];
    }

    return 0;
}

