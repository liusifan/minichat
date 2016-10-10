
#include <vector>
#include <stdlib.h>
#include <stdio.h>

#include "phxrpc/file.h"
#include "benchmark.h"
#include "common/push_client.h"

using namespace std;

int ParseOpFile(vector<Op> & op_vec,
        const char * op_file)
{
    FILE * fp = NULL;
    fp = fopen(op_file, "r");
    if(NULL == fp) {
        printf("open op_file %s failed errno %d\n", op_file, errno);
        return -1;
    }

    char line[128];
    memset(line, 0x0, sizeof(line));
    int idx = 0;
    while(NULL != fgets(line, sizeof(line), fp)) {
        line[strlen(line) - 1] = '\0';
        Op op;
        char * str1 = line;
        char * str2 = NULL;

        int i = 0;
        for(i = 0; i < 5;i++) {
            str2 = strtok(str1," ");
            if(NULL == str2) {
                break;
            }

            if(0 == i) {
                op.duration_sec = atoi(str2);
            } else if(1 == i) {
                op.begin_qps = strtod(str2, nullptr);
            } else if(2 == i) {
                op.end_qps = strtod(str2, nullptr);
            } else if(3 == i) {
                op.qps_variation = strtod(str2, nullptr);
            } else {
                op.qps_var_interval_sec = atoi(str2);
            }

            if(op.end_qps >= op.begin_qps) {
                op.qps_var_direction = 0;
            } else {
                op.qps_var_direction = 1;
            }
            str1 = NULL;
        }

        op_vec.push_back(op);

        memset(line, 0x0, sizeof(line));
    }
    fclose(fp);
    return 0;
}

void run( phxrpc::OptMap & opt_map )
{
    BMArgs_t args;

    args.begin_index = atoi( opt_map.Get( 'b' ) );

    args.thread_count = atoi( opt_map.Get( 't' ) );
    args.uthread_per_thread = atoi( opt_map.Get( 'u' ) );
    args.user_per_uthread = PushClient::USER_PER_CHANNEL;

    args.auth_use_rsa = false;
    if( opt_map.Has( 'r' ) ) args.auth_use_rsa = ( 0 != atoi( opt_map.Get( 'r' ) ) );

    ParseOpFile(args.op_vec, opt_map.Get('f'));
    benchmark( args );
}

void ShowUsage( const char * program )
{
    printf( "\nUsage: %s [-b <begin user index ] [-r <auth use rsa>]\n", program );
    printf( "\t[-t <thread>] [-u <uthread per thread>]\n" );
    printf( "\t[-f op file]\n" );

    printf( "\n" );
    printf( "\t%d users per uthread/channel as default\n", PushClient::USER_PER_CHANNEL );
    printf( "\n" );

    exit( -1 );
}

int main( int argc, char * argv[] )
{
    phxrpc::openlog( argv[0], "~/log/error", LOG_ERR);

    phxrpc::ClientConfigRegistry::GetDefault()->Stop();

    phxrpc::OptMap opt_map( "f:t:u:b:r:v" );

    if( ! opt_map.Parse( argc, argv ) ) ShowUsage( argv[0] );

    if( ( ! opt_map.Has( 't' ) ) || ( ! opt_map.Has( 'u' ) )
            || ( ! opt_map.Has( 'b' ) ) || ( ! opt_map.Has( 'f' ) )
            || opt_map.Has( 'v' ) ) {
        ShowUsage( argv[0] );
    }

    run( opt_map );

    return 0;
}

