
#include "phxrpc/file.h"

#include "benchmark.h"

#include "common/push_client.h"

#include <stdlib.h>

void run( phxrpc::OptMap & opt_map )
{
    int thread_count = atoi( opt_map.Get( 't' ) );
    int uthread_per_thread = atoi( opt_map.Get( 'u' ) );
    int begin_index = atoi( opt_map.Get( 'b' ) );
    int msg_per_user = atoi( opt_map.Get( 'm' ) );

    benchmark( begin_index, thread_count, uthread_per_thread, msg_per_user );
}

void ShowUsage( const char * program )
{
    printf( "\nUsage: %s [-t <thread>] [-u <uthread per thread>] "
            "[-b <begin user index>] [-m <msg per user>]\n", program );
    printf( "\n" );
    printf( "\t%d users per uthread/channel as default\n", PushClient::USER_PER_CHANNEL );
    printf( "\n" );

    exit( -1 );
}

int main( int argc, char * argv[] )
{
    phxrpc::openlog( argv[0], "~/log/error", LOG_DEBUG);

    phxrpc::OptMap opt_map( "t:u:b:m:v" );

    if( ! opt_map.Parse( argc, argv ) ) ShowUsage( argv[0] );

    if( ( ! opt_map.Has( 't' ) ) || ( ! opt_map.Has( 'u' ) )
            || ( ! opt_map.Has( 'b' ) ) || ( ! opt_map.Has( 'm' ) )
            || opt_map.Has( 'v' ) ) {
        ShowUsage( argv[0] );
    }

    run( opt_map );

    return 0;
}

