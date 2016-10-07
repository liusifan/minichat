
#include "phxrpc/file.h"

#include "benchmark.h"

#include "common/push_client.h"

#include <stdlib.h>

void run( phxrpc::OptMap & opt_map )
{
    BMArgs_t args;

    args.begin_index = atoi( opt_map.Get( 'b' ) );

    args.thread_count = atoi( opt_map.Get( 't' ) );
    args.uthread_per_thread = atoi( opt_map.Get( 'u' ) );
    args.user_per_uthread = PushClient::USER_PER_CHANNEL;
    args.msg_per_user = atoi( opt_map.Get( 'm' ) );

    args.begin_qps_per_uthread = atoi( opt_map.Get( 'q' ) );
    args.max_qps_per_uthread = atoi( opt_map.Get( 'x' ) );
    args.qps_time_interval_per_uthread = atoi( opt_map.Get( 'i' ) );
    args.qps_increment_per_uthread = atoi( opt_map.Get( 'n' ) );

    args.auth_use_rsa = false;
    if( opt_map.Has( 'r' ) ) args.auth_use_rsa = ( 0 != atoi( opt_map.Get( 'r' ) ) );

    benchmark( args );
}

void ShowUsage( const char * program )
{
    printf( "\nUsage: %s [-b <begin user index ] [-r <auth use rsa>]\n", program );
    printf( "\t[-t <thread>] [-u <uthread per thread>] [-m <msg per user>]\n" );
    printf( "\t[-q begin qps] [-x max qps] [-i qps time interval] [-n qps increment]\n" );

    printf( "\n" );
    printf( "\t%d users per uthread/channel as default\n", PushClient::USER_PER_CHANNEL );
    printf( "\n" );

    exit( -1 );
}

int main( int argc, char * argv[] )
{
    phxrpc::openlog( argv[0], "~/log/error", LOG_DEBUG);

    phxrpc::ClientConfigRegistry::GetDefault()->Stop();

    phxrpc::OptMap opt_map( "q:x:i:n:t:u:b:m:r:v" );

    if( ! opt_map.Parse( argc, argv ) ) ShowUsage( argv[0] );

    if( ( ! opt_map.Has( 't' ) ) || ( ! opt_map.Has( 'u' ) )
            || ( ! opt_map.Has( 'b' ) ) || ( ! opt_map.Has( 'm' ) )
            || opt_map.Has( 'v' ) ) {
        ShowUsage( argv[0] );
    }

    run( opt_map );

    return 0;
}

