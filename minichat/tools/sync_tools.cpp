
#include "phxrpc/file.h"
#include "phxrpc/network.h"

#include "common/push_client.h"
#include "logic/minichat_api.h"
#include "logic/logic_client.h"

#include <stdlib.h>
#include <string.h>

#include <thread>

typedef struct tagSyncStat {
    int push_count;
    int sync_count;
    int msg_count;
} SyncStat_t;

__thread SyncStat_t global_sync_stat;

class SyncUThread {
public:
    SyncUThread( int tag, phxrpc::UThreadEpollScheduler * scheduler,
            int begin_index, int user_per_uthread );
    ~SyncUThread();

    int StartSync();

private:
    phxrpc::UThreadEpollScheduler * scheduler_;
    MiniChatAPI ** apis_;
    int begin_index_;
    int user_per_uthread_;
    int tag_;
};

SyncUThread :: SyncUThread( int tag, phxrpc::UThreadEpollScheduler * scheduler,
        int begin_index, int user_per_uthread )
{
    tag_ = tag;

    scheduler_ = scheduler;
    begin_index_ = begin_index;
    user_per_uthread_ = user_per_uthread;

    apis_ = (MiniChatAPI**)calloc( user_per_uthread_, sizeof( MiniChatAPI * ) );
    for( int i = 0; i < user_per_uthread_; i++ ) {
        apis_[i] = new MiniChatAPI( scheduler_ );
    }
}

SyncUThread :: ~SyncUThread()
{
    for( int i = 0; i < user_per_uthread_; i++ ) {
        delete apis_[i];
    }

    free( apis_ );
}

int SyncUThread :: StartSync()
{
    // waiting for push
    phxrpc::__uthread( *scheduler_ ) - [=]( void * ) {
        PushClient client( scheduler_ );

        char channel[ 128 ] = { 0 };
        snprintf( channel, sizeof( channel ), "channel_%d",
                begin_index_ / PushClient::USER_PER_CHANNEL );

        client.Sub( channel );

        printf( "Waiting %s ......\n", channel );

        for( ; ; ) {
            std::string msg;
            client.Wait( channel, &msg );

            ++global_sync_stat.push_count;

            if( 0 == strcasecmp( "exit", msg.c_str() ) ) break;
            if( msg.size() < 4 ) continue;

            int index = atoi( msg.c_str() + 4 );
            int offset = index - begin_index_;

            if( offset < user_per_uthread_ && offset >= 0 ) {
                logic::SyncResponse resp;

                MiniChatAPI * api = apis_[ offset ];

                char username[ 128 ] = { 0 };
                strncpy( username, msg.c_str(), sizeof( username ) );
                strtok( username, " " );

                ++global_sync_stat.sync_count;
                api->SetUsername( username );

                int msg_count = 0;
                apis_[ offset ]->Sync( &resp, &msg_count );

                global_sync_stat.msg_count += msg_count;

                if( 0 == ( global_sync_stat.sync_count % 100 ) ) {
                    printf( "sync %s\n", apis_[offset]->GetUsername() );
                    printf( "tag %d, push_client %d, sync_count %d, msg_count %d\n",
                            tag_, global_sync_stat.push_count, global_sync_stat.sync_count,
                            global_sync_stat.msg_count );
                }
            }
        }
    };

    return 0;
}

//===================================================================

int SyncThread( int tag, int begin_index, int uthread_per_thread )
{
    SyncUThread ** uthreads = ( SyncUThread ** ) calloc( uthread_per_thread, sizeof( SyncUThread * ) );

    phxrpc::UThreadEpollScheduler scheduler( 64 * 1024, uthread_per_thread * 2 );

    for( int i = 0; i < uthread_per_thread; i++ ) {
        uthreads[i] = new SyncUThread( tag * 1000 + i,
                &scheduler, begin_index + i * PushClient::USER_PER_CHANNEL,
                PushClient::USER_PER_CHANNEL );
        uthreads[i]->StartSync();
    }

    printf( "begin sync\n" );

    scheduler.Run();

    for( int i = 0; i < uthread_per_thread; i++ ) {
        delete uthreads[i];
    }

    free( uthreads );
}

int DoSync( int begin_index, int thread_count, int uthread_per_thread )
{
    std::thread * threads[ thread_count ];

    int unit = PushClient::USER_PER_CHANNEL * uthread_per_thread;

    for( int i = 0; i < thread_count; i++ ) {
        threads[i] = new std::thread( SyncThread, i, begin_index + i * unit, uthread_per_thread );
    }

    for( int i = 0; i < thread_count; i++ ) {
        threads[i]->join();
    }

    for( int i = 0; i < thread_count; i++ ) {
        delete threads[i];
    }

    return 0;
}

void run( phxrpc::OptMap & opt_map )
{
    int thread_count = atoi( opt_map.Get( 't' ) );
    int uthread_per_thread = atoi( opt_map.Get( 'u' ) );
    int begin_index = atoi( opt_map.Get( 'b' ) );

    DoSync( begin_index, thread_count, uthread_per_thread );
}

void ShowUsage( const char * program )
{
    printf( "\nUsage: %s [-t <thread>] [-u <uthread per thread>] [-b <begin user index>]\n", program );
    printf( "\n" );
    printf( "\t%d users per uthread/channel as default\n", PushClient::USER_PER_CHANNEL );
    printf( "\n" );

    exit( -1 );
}

int main( int argc, char * argv[] )
{
    phxrpc::openlog( argv[0], "~/log/error", LOG_DEBUG);

    phxrpc::ClientConfigRegistry::GetDefault()->Stop();

    phxrpc::OptMap opt_map( "t:u:b:v" );

    if( ! opt_map.Parse( argc, argv ) ) ShowUsage( argv[0] );

    if( ( ! opt_map.Has( 't' ) ) || ( ! opt_map.Has( 'u' ) )
            || ( ! opt_map.Has( 'b' ) ) || opt_map.Has( 'v' ) ) {
        ShowUsage( argv[0] );
    }

    run( opt_map );

    return 0;
}

