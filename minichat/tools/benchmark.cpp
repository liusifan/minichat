
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

class BMUThread {
public:
    BMUThread( phxrpc::UThreadEpollScheduler * scheduler,
            int begin_index, int user_per_uthread, int msg_per_user,
            int begin_qps_per_uthread, int max_qps_per_uthread,
            int qps_time_interval_per_uthread,
            int qps_increment_per_uthread  );
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

    phxrpc::UThreadEpollScheduler * scheduler_;
    int begin_index_;
    int user_per_uthread_;
    int msg_per_user_;
    MiniChatAPI ** apis_;

    int auth_ok_count_;

    enum { RUNNING = 0, STOPPING = 1, STOPPED = 2 };
    int status_;

    int begin_qps_per_uthread_; 
    int max_qps_per_uthread_;
    int qps_time_interval_per_uthread_;
    int qps_increment_per_uthread_;
};

BMUThread :: BMUThread( phxrpc::UThreadEpollScheduler * scheduler,
        int begin_index, int user_per_uthread, int msg_per_user,
        int begin_qps_per_uthread, int max_qps_per_uthread,
        int qps_time_interval_per_uthread,
        int qps_increment_per_uthread  )
{
    scheduler_ = scheduler;
    user_per_uthread_ = user_per_uthread;
    begin_index_ = begin_index;
    msg_per_user_ = msg_per_user;

    begin_qps_per_uthread_ = begin_qps_per_uthread; 
    max_qps_per_uthread_ = max_qps_per_uthread;
    qps_time_interval_per_uthread_ = qps_time_interval_per_uthread;
    qps_increment_per_uthread_ = qps_increment_per_uthread;

    apis_ = ( MiniChatAPI ** ) calloc( user_per_uthread_, sizeof( MiniChatAPI * ) );

    for( int i = 0; i < user_per_uthread_; i++ ) {
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

    for( int i = 0; i < user_per_uthread_; i++ ) {
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
        for( ; RUNNING == status_ && auth_ok_count_ < user_per_uthread_; ) {
            for( int i = 0; RUNNING == status_ && i < user_per_uthread_; i++ ) {
                if( ! apis_[i]->IsAuthOK() ) {
                    char username[ 128 ] = { 0 };
                    snprintf( username, sizeof( username ), "user%d", begin_index_ + i );

                    logic::AuthResponse auth_resp;
                    if( 0 == apis_[i]->Auth( username, username, &auth_resp ) ) {
                        auth_ok_count_++;
                    } else {
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
        int qps,
        int & curr_cnt,
        int total_req_cnt)
{
    if(0 == msg_from_to.size()) {
        return 0;
    }
    std::random_device rd;

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

        const MsgFromTo & item = msg_from_to[ rd() % msg_from_to.size() ];

        logic::SendMsgResponse resp;
        apis_[item.from]->SendMsg( item.to.c_str(), "msg from ", &resp );

        gettimeofday(&end, NULL);
        unsigned long long usec = (end.tv_sec - begin.tv_sec) * 1000000 + (end.tv_usec - begin.tv_usec);

        printf( "send msg from %s to %s cost %lld usec\n",
                apis_[item.from]->GetUsername(), item.to.c_str(),
                usec );

        usec = (end.tv_sec - begin_time.tv_sec) * 1000000 + (end.tv_usec - begin_time.tv_usec);
        if(usec >= (unsigned long long)qps_time_interval_per_uthread_ * 1000000) {
            return 1;
        }

        double interval = distribution(generator);;

        UThreadWait(*socket, interval);
    }
    return 0;
}


int BMUThread :: StartLoad()
{
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

            int index = -1;
            if( msg.size() > 4 ) index = atoi( msg.c_str() + 4 );

            int offset = index - begin_index_;

            if( offset < user_per_uthread_ && offset >= 0 ) {
                logic::SyncResponse resp;

                apis_[ offset ]->Sync( &resp );

                printf( "sync %s\n", apis_[offset]->GetUsername() );
            }
        }

        status_ = STOPPED;
        printf("stop sync uthread\n");
    };

    printf( "do sendmsg\n" );

    // do sendmsg
    phxrpc::__uthread( *scheduler_ ) - [=]( void * ) {


        int fd = ::socket(AF_INET, SOCK_STREAM, IPPROTO_IP); 
        phxrpc::UThreadSocket_t * socket = scheduler_->CreateSocket(fd);

        vector<MsgFromTo> msg_from_to;

        // get all contact
        for( int i = 0; i < user_per_uthread_; i++ ) {
            logic::SyncResponse resp;
            apis_[i]->Sync( &resp );

            const std::vector< std::string > & contacts = apis_[i]->GetContacts();
            if( contacts.size() == 0 ) continue;
            for(auto & it : contacts) {
                msg_from_to.push_back( { i, it } );
            }
        }

        //int msg_from_to_size = msg_from_to.size();
        int curr_qps = begin_qps_per_uthread_;
        int curr_cnt = 0;
        int total_req_cnt = user_per_uthread_ * msg_per_user_;

        int ret = 0;

        while(RUNNING == status_) {

            ret =  LoadFunc(msg_from_to,
                    socket,
                    curr_qps,
                    curr_cnt,
                    total_req_cnt);

            if(1 == ret) {
                curr_qps += qps_increment_per_uthread_;
                if(curr_qps > max_qps_per_uthread_) {
                    curr_qps = max_qps_per_uthread_;
                }
            } else {
                break;
            }
        }

        free(socket);

        status_ = STOPPING;
    };

    return 0;
}

//===================================================================

int BMThread( int begin_index, int uthread_per_thread, int msg_per_user,
        int begin_qps_per_uthread, int max_qps_per_uthread, int qps_time_interval_per_uthread,
        int qps_increment_per_uthread )
{
    BMUThread ** uthreads = ( BMUThread ** ) calloc( uthread_per_thread, sizeof( BMUThread * ) );

    phxrpc::UThreadEpollScheduler scheduler( 64 * 1024, uthread_per_thread * 2 );

    for( int i = 0; i < uthread_per_thread; i++ ) {
        uthreads[i] = new BMUThread( &scheduler,
                begin_index + i * PushClient::USER_PER_CHANNEL,
                PushClient::USER_PER_CHANNEL, msg_per_user,
                begin_qps_per_uthread, max_qps_per_uthread,
                qps_time_interval_per_uthread,
                qps_increment_per_uthread );
        uthreads[i]->StartAuth();
    }

    printf( "begin auth\n" );

    scheduler.Run();

    for( int i = 0; i < uthread_per_thread; i++ ) {
        uthreads[i]->StartLoad();
    }

    printf( "begin load\n" );

    scheduler.Run();

    for( int i = 0; i < uthread_per_thread; i++ ) {
        delete uthreads[i];
    }

    free( uthreads );
}

int benchmark( int begin_index, int thread_count,
        int uthread_per_thread, int msg_per_user,
        int begin_qps_per_uthread, int max_qps_per_uthread, int qps_time_interval_per_uthread,
        int qps_increment_per_uthread  )
{
    std::thread * threads[ thread_count ];

    int unit = PushClient::USER_PER_CHANNEL * uthread_per_thread;

    for( int i = 0; i < thread_count; i++ ) {
        threads[i] = new std::thread( BMThread, begin_index + i * unit,
                uthread_per_thread, msg_per_user,
                begin_qps_per_uthread, max_qps_per_uthread, qps_time_interval_per_uthread,
                qps_increment_per_uthread );
    }

    for( int i = 0; i < thread_count; i++ ) {
        threads[i]->join();
    }

    for( int i = 0; i < thread_count; i++ ) {
        delete threads[i];
    }

    return 0;
}

