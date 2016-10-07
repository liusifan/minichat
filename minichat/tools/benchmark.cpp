
#include "benchmark.h"

#include "phxrpc/network.h"

#include "logic/logic_client.h"
#include "logic/minichat_api.h"

#include "common/push_client.h"

#include <thread>
#include <random>

class BMUThread {
public:
    BMUThread( phxrpc::UThreadEpollScheduler * scheduler,
            int begin_index, int user_per_uthread, int msg_per_user );
    ~BMUThread();

    int StartAuth();

    int StartLoad();

    int Stop();

private:
    phxrpc::UThreadEpollScheduler * scheduler_;
    int begin_index_;
    int user_per_uthread_;
    int msg_per_user_;
    MiniChatAPI ** apis_;

    int auth_ok_count_;

    enum { RUNNING = 0, STOPPING = 1, STOPPED = 2 };
    int status_;
};

BMUThread :: BMUThread( phxrpc::UThreadEpollScheduler * scheduler,
        int begin_index, int user_per_uthread, int msg_per_user )
{
    scheduler_ = scheduler;
    user_per_uthread_ = user_per_uthread;
    begin_index_ = begin_index;
    msg_per_user_ = msg_per_user;

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
                    }
                }
            }
        }
    };

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

            if( offset < user_per_uthread_ ) {
                logic::SyncResponse resp;
                apis_[ offset ]->Sync( &resp );

                printf( "sync %s\n", apis_[offset]->GetUsername() );
            }
        }

        status_ = STOPPED;
    };

    printf( "do sendmsg\n" );

    // do sendmsg
    phxrpc::__uthread( *scheduler_ ) - [=]( void * ) {
        // get all contact
        for( int i = 0; i < user_per_uthread_; i++ ) {
            logic::SyncResponse resp;
            apis_[i]->Sync( &resp );
        }

        std::random_device rd;

        for( int i = 0; RUNNING == status_ && i < user_per_uthread_; i++ ) {
            const std::vector< std::string > contacts = apis_[i]->GetContacts();
            if( contacts.size() == 0 ) continue;

            for( int j = 0; j < msg_per_user_; j++ ) {
                const std::string & item = contacts[ rd() % contacts.size() ];
                logic::SendMsgResponse resp;
                apis_[i]->SendMsg( item.c_str(), "msg from ", &resp );

                printf( "send msg from %s to %s\n", apis_[i]->GetUsername(), item.c_str() );
            }
        }

        status_ = STOPPING;
    };

    return 0;
}

//===================================================================

int BMThread( int begin_index, int uthread_per_thread, int msg_per_user )
{
    BMUThread ** uthreads = ( BMUThread ** ) calloc( uthread_per_thread, sizeof( BMUThread * ) );

    phxrpc::UThreadEpollScheduler scheduler( 64 * 1024, uthread_per_thread * 2 );

    for( int i = 0; i < uthread_per_thread; i++ ) {
        uthreads[i] = new BMUThread( &scheduler,
                begin_index + i * PushClient::USER_PER_CHANNEL,
                PushClient::USER_PER_CHANNEL, msg_per_user );
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
        int uthread_per_thread, int msg_per_user )
{
    std::thread * threads[ thread_count ];

    int unit = PushClient::USER_PER_CHANNEL * uthread_per_thread;

    for( int i = 0; i < thread_count; i++ ) {
        threads[i] = new std::thread( BMThread, begin_index + i * unit,
                uthread_per_thread, msg_per_user );
    }

    for( int i = 0; i < thread_count; i++ ) {
        threads[i]->join();
    }

    for( int i = 0; i < thread_count; i++ ) {
        delete threads[i];
    }

    return 0;
}

