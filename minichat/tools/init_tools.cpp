
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

void InitUser( int count, int thread_count )
{
    if( count <= 0 ) return;

    int interval = count / thread_count;
    if(0 == interval) {
        thread_count = 1;
        interval = count;
    }

    cout << "thread_count " << thread_count << " count "
        << count << " interval " << interval << endl;

    std::thread * threads = new std::thread[thread_count];

    for(int i = 0 ; i < thread_count; i++) {

        int begin_idx = interval * i;
        int end_idx = interval * ( i + 1 );

        threads[i] = std::thread( InitUserFunc, begin_idx, end_idx );
    }

    for(int i = 0; i < thread_count; i++ ) {
        try {
            threads[i].join();
        } catch (const std::exception & ex) {
            cout << ex.what() << endl;
        }
    }

    delete [] threads;
}

void ShowUsage( const char * program )
{
    printf ( "\n" ) ;
    printf ( "%s [-f func] [-c count] [-t thread count] [-v]\n", program );
    printf ( "\n" );
    printf ( "\t-f user -c <count> -t <thread count>\n" );
    printf ( "\t-f addrbook -c <count> -t <thread count>\n" );
    printf ( "\n" );

    exit( -1 );
}

int main( int argc, char * *argv )
{
    phxrpc::openlog( argv[0], "~/log/error", LOG_DEBUG);

    phxrpc::ClientConfigRegistry::GetDefault()->Stop();

    extern char *optarg ;
    int c ;

    const char * func = NULL;
    int count = 0, thread_count = 1;

    while ( (c = getopt ( argc, argv, "f:c:t:v" )) != EOF ) {
        switch ( c ) {
            case 'f':
                func = optarg;
                break;
            case 'c' :
                count = atoi(optarg);
                break;
            case 't' :
                thread_count = atoi(optarg);
                break;
            case 'v' : 
            case '?' :
            default :
                ShowUsage( argv[0] );
                break ;
        }
    }

    if( NULL == func || count <= 0 ) ShowUsage( argv[0] );

    if(0 == strcasecmp("user", func)) {
        InitUser( count, thread_count );
    } else if(0 == strcasecmp("addrbook", func)) {
        InitAddrbook( count, thread_count );
    } else {
        cout << "func " << func << " is undefined " << endl;
    } 
    return 0;
}

