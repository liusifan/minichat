
#include <vector>
#include <stdlib.h>
#include <stdio.h>

#include "phxrpc/file.h"
#include "benchmark.h"
#include "common/push_client.h"

using namespace std;

void run( phxrpc::OptMap & opt_map )
{
    const char * cmd = opt_map.Get('f');
    if(0 == strcasecmp(cmd, "benchmark")) {

        if( ( ! opt_map.Has( 't' ) ) || ( ! opt_map.Has( 'u' ) )
                || ( ! opt_map.Has( 'b' ) ) ) {
            return;
        }


        BMArgs_t args;

        args.begin_index = atoi( opt_map.Get( 'b' ) );

        args.thread_count = atoi( opt_map.Get( 't' ) );
        args.uthread_per_thread = atoi( opt_map.Get( 'u' ) );
        args.user_per_uthread = PushClient::USER_PER_CHANNEL;

        args.auth_use_rsa = false;
        if( opt_map.Has( 'r' ) ) args.auth_use_rsa = ( 0 != atoi( opt_map.Get( 'r' ) ) );

        benchmark( args );
    } else if(0 == strcasecmp(cmd, "sendcmd")) {
        if( ( ! opt_map.Has( 'm' ) ) ) {
            return;
        }
        sendcmd(opt_map.Get('m'));
    } else {
        printf("unknown cmd %s\n", cmd);
    }
}

void ShowUsage( const char * program )
{
    printf( "\nUsage: %s [-b <begin user index ] [-r <auth use rsa>]\n", program );
    printf( "\t[-t <thread>] [-u <uthread per thread>]\n" );
    printf( "\t[-f func] [-m cmd]\n" );
    printf( "\tfunc:\n" );
    printf( "\t\tbenchmark\n" );
    printf( "\t\tsendcmd\n" );

    printf( "\n" );
    printf( "\t%d users per uthread/channel as default\n", PushClient::USER_PER_CHANNEL );
    printf( "\n" );

    exit( -1 );
}

int main( int argc, char * argv[] )
{
    phxrpc::openlog( argv[0], "~/log/error", LOG_ERR);

    phxrpc::ClientConfigRegistry::GetDefault()->Stop();

    phxrpc::OptMap opt_map( "m:f:t:u:b:r:v" );

    if( ! opt_map.Parse( argc, argv ) ) ShowUsage( argv[0] );

    if( ( ! opt_map.Has( 'f' ) )
            || opt_map.Has( 'v' ) ) {
        ShowUsage( argv[0] );
    }

    run( opt_map );

    return 0;
}

