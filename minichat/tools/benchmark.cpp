
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


using namespace std;

typedef struct _MsgFromTo {
    int from;
    string to;
} MsgFromTo;

typedef struct tagBMStat {
    int auth_count, auth_ok_count, auth_fail_count;
    int msg_count;
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
            int qps,
            int & curr_cnt,
            int total_req_cnt); 

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

                    logic::AuthResponse auth_resp;
                    if( 0 == apis_[i]->Auth( username, username, &auth_resp, args_.auth_use_rsa ) ) {
                        ++auth_ok_count_;
                        ++global_bm_stat.auth_ok_count;
                    } else {
                        ++global_bm_stat.auth_fail_count;
                        printf("auth %s failed\n", username);
                    }
                }
            }
        }
    };

    return 0;
}

int BMUThread::LoadFunc(vector<MsgFromTo> & msg_from_to,
        phxrpc::UThreadSocket_t * socket,  
        int qps, int & curr_cnt, int total_req_cnt)
{
    if(0 == msg_from_to.size()) {
        return 0;
    }

    double time_interval_ms = 1000.0 / (double)qps;
    int seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);
    std::exponential_distribution<double> distribution((double)1.0/(double)time_interval_ms);

    struct timeval begin, end;
    struct timeval begin_time;
    gettimeofday(&begin_time, NULL);

    for(; curr_cnt < total_req_cnt; ) {

        curr_cnt += 1;
        gettimeofday(&begin, NULL);

        const MsgFromTo & item = msg_from_to[ generator() % msg_from_to.size() ];

        logic::SendMsgResponse resp;
        apis_[item.from]->SendMsg( item.to.c_str(), "msg from ", &resp );

        ++global_bm_stat.msg_count;

        gettimeofday(&end, NULL);
        unsigned long long usec = (end.tv_sec - begin.tv_sec) * 1000000 + (end.tv_usec - begin.tv_usec);

        if( 0 == ( global_bm_stat.msg_count % 100 ) ) {
            printf( "send msg from %s to %s cost %lld usec\n",
                apis_[item.from]->GetUsername(), item.to.c_str(),
                usec );
            printf( "tag %d, msg_count %d\n", tag_, global_bm_stat.msg_count );
        }

        usec = (end.tv_sec - begin_time.tv_sec) * 1000000 + (end.tv_usec - begin_time.tv_usec);
        if(usec >= (unsigned long long)args_.qps_time_interval_per_uthread * 1000000) {
            return 1;
        }

        double interval = distribution(generator);;

        UThreadWait(*socket, interval);
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

        int curr_qps = args_.begin_qps_per_uthread;
        int curr_cnt = 0;
        int total_req_cnt = args_.user_per_uthread * args_.msg_per_user;

        int ret = 0;
        while(RUNNING == status_) {
            ret =  LoadFunc(msg_from_to, socket,
                    curr_qps, curr_cnt, total_req_cnt);
            if(1 == ret) {
                curr_qps += args_.qps_increment_per_uthread;
                if(curr_qps > args_.max_qps_per_uthread) {
                    curr_qps = args_.max_qps_per_uthread;
                }
            } else {
                break;
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
    BMUThread ** uthreads = ( BMUThread ** ) calloc( args.uthread_per_thread, sizeof( BMUThread * ) );

    phxrpc::UThreadEpollScheduler scheduler( 64 * 1024, args.uthread_per_thread * 2 );

    for( int i = 0; i < args.uthread_per_thread; i++ ) {
        uthreads[i] = new BMUThread( tag * 1000 + i, &scheduler,
                begin_index + i * PushClient::USER_PER_CHANNEL, args );
        uthreads[i]->StartAuth();
    }

    printf( "begin auth\n" );

    scheduler.Run();

    printf( "auth %d, ok %d, fail %d\n", global_bm_stat.auth_count,
            global_bm_stat.auth_ok_count, global_bm_stat.auth_fail_count );

    for( int i = 0; i < args.uthread_per_thread; i++ ) {
        uthreads[i]->StartLoad();
    }

    printf( "begin load\n" );

    scheduler.Run();

    for( int i = 0; i < args.uthread_per_thread; i++ ) {
        delete uthreads[i];
    }

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

