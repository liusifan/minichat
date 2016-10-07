
#include "init_tools.h"

#include "phxrpc/file.h"
#include "phxrpc/rpc.h"

#include <iostream>
#include <thread>

using namespace std;

void InitUserFunc(int begin_idx, int end_idx)
{
    InitAccount(begin_idx,end_idx);
    InitProfile(begin_idx,end_idx);
}

int InitUser( phxrpc::OptMap & opt_map )
{
    if( NULL == opt_map.Get( 'c' ) ) return -1;

    int count = atoi( opt_map.Get( 'c' ) );

    int thread_count = 1;
    if( NULL != opt_map.Get( 't' ) ) thread_count = atoi( opt_map.Get( 't' ) );

    int interval = count / thread_count;
    if(0 == interval) {
        thread_count = 1;
        interval = count;
    }

    cout << "thread_count " << thread_count << " count "
        << count << " interval " << interval << endl;

    std::thread * threads[ thread_count ];

    for(int i = 0 ; i < thread_count; i++) {

        int begin_idx = interval * i;
        int end_idx = interval * ( i + 1 );

        threads[i] = new std::thread( InitUserFunc, begin_idx, end_idx );
    }

    for(int i = 0; i < thread_count; i++ ) {
        try {
            threads[i]->join();
        } catch (const std::exception & ex) {
            cout << ex.what() << endl;
        }
    }

    for( int i = 0; i < thread_count; i++ ) {
        delete threads[i];
    }

    return 0;
}

int GenAddrbook( phxrpc::OptMap & opt_map )
{
    if( NULL == opt_map.Get( 'c' ) ) return -1;

    int count = atoi( opt_map.Get( 'c' ) );

    int mode = 0;
    if( NULL != opt_map.Get( 'm' ) ) mode = atoi( opt_map.Get( 'm' ) );

    const char * path = "minichat_addrbook.txt";

    GenAddrbook( count, mode, path );

    printf( "generate addrbook %s\n", path );

    return 0;
}

int LoadAddrbook( phxrpc::OptMap & opt_map )
{
    if( NULL == opt_map.Get( 'p' ) ) return -1;
    if( NULL == opt_map.Get( 'r' ) ) return -1;

    const char * path = opt_map.Get( 'p' );
    const char * result_path = opt_map.Get( 'r' );


    int thread_count = 1;
    if( NULL != opt_map.Get( 't' ) ) thread_count = atoi( opt_map.Get( 't' ) );

    LoadAddrbook( path, result_path, thread_count );

    return 0;
}

void ShowUsage( const char * program )
{
    printf ( "\n" ) ;
    printf ( "%s [-f func] [-v]\n", program );
    printf ( "\n" );
    printf ( "\t-f inituser -c <count> -t <thread count>\n" );
    printf ( "\t-f genaddr -c <count> -m <mode>\n" );
    printf ( "\t-f loadaddr -p <data file> -r <result file> -t <thread count>\n" );
    printf ( "\n" );

    exit( -1 );
}

int main( int argc, char * *argv )
{
    phxrpc::openlog( argv[0], "~/log/error", LOG_DEBUG);

    phxrpc::ClientConfigRegistry::GetDefault()->Stop();

    phxrpc::OptMap opt_map( "r:f:c:m:t:p:v" );

    if( ! opt_map.Parse( argc, argv ) ) ShowUsage( argv[0] );

    if( NULL == opt_map.Get( 'f' ) || opt_map.Has( 'v' ) ) ShowUsage( argv[0] );

    const char * func = opt_map.Get( 'f' );

    int ret = -1;

    if(0 == strcasecmp("inituser", func)) {
        ret = InitUser( opt_map );
    } else if(0 == strcasecmp("genaddr", func)) {
        ret = GenAddrbook( opt_map );
    } else if(0 == strcasecmp("loadaddr", func)) {
        ret = LoadAddrbook( opt_map );
    }

    if( 0 != ret ) ShowUsage( argv[0] );

    return 0;
}

