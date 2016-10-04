
#include "push_client.h"

#include <string>
#include <iostream>

#include "phxrpc/file.h"

void sub( const char * channel )
{
    bool ret = false;

#if 1

    phxrpc::UThreadEpollScheduler scheduler( 64 * 1024, 10 );

    phxrpc::__uthread( scheduler ) - [=,&scheduler,&ret]( void * ) {
        PushClient client( &scheduler );;
        ret = client.Sub( channel );
        std::cout << "sub " << ret << std::endl;
        for( ; ; ) {
            std::string msg;
            ret = client.Wait( &msg );
            std::cout << "wait " << ret << ", msg " << msg << std::endl;
        }
    };

    scheduler.Run();

#else

    PushClient client;
    ret = client.Sub( channel );
    std::cout << "sub " << ret << std::endl;
    for( ; ; ) {
        std::string msg;
        ret = client.Wait( &msg );
        std::cout << "wait " << ret << ", msg " << msg << std::endl;
    }

#endif
}

void pub( const char * channel, const char * msg )
{
    msg = msg ? msg : "NULL";

    PushClient client;

    bool ret = client.Pub( channel, msg );

    std::cout << "pub " << ret << std::endl;
}

void usage( const char * program )
{
    printf( "%s -f <func> -c <channel> -m <msg>\n", program );
    exit( -1 );
}

int main( int argc, char * argv[] )
{
    phxrpc::OptMap opt_map( "f:c:m:v" );

    if( ! opt_map.Parse( argc, argv ) ) usage( argv[0] );

    if( NULL == opt_map.Get( 'f' ) || NULL == opt_map.Get( 'c' ) ) {
        usage( argv[0] );
    }

    if( 0 == strcasecmp( opt_map.Get( 'f' ), "sub" ) ) {
        sub( opt_map.Get( 'c' ) );
    } else if( 0 == strcasecmp( opt_map.Get( 'f' ), "pub" ) ) {
        pub( opt_map.Get( 'c' ), opt_map.Get( 'm' ) );
    } else {
        usage( argv[0] );
    }

    return 0;
}

