/* presence_main.cpp

 Generated by phxrpc_pb2server from presence.proto

*/

#include <iostream>
#include <memory>
#include <unistd.h>
#include <signal.h>
#include "phxrpc_presence_dispatcher.h"
#include "presence_service_impl.h"
#include "presence_server_config.h"

#include "phxrpc/rpc.h"
#include "phxrpc/http.h"
#include "phxrpc/file.h"

#include "common/redis_client_factory.h"

using namespace std;

void HttpDispatch( const phxrpc::HttpRequest & request, phxrpc::HttpResponse * response, phxrpc::DispatcherArgs_t * args ) {

    ServiceArgs_t * service_args = (ServiceArgs_t *)(args->service_args);

    PresenceServiceImpl service( * service_args );
    PresenceDispatcher dispatcher( service, args );

    phxrpc::HttpDispatcher<PresenceDispatcher> http_dispatcher(
            dispatcher, PresenceDispatcher::GetURIFuncMap() );
    if( ! http_dispatcher.Dispatch( request, response ) ) {
        response->SetStatusCode( 404 );
        response->SetReasonPhrase( "Not Found" );
    }
}

void showUsage( const char * program ) {
    printf( "\n" );
    printf( "Usage: %s [-c <config>] [-d] [-v]\n", program );
    printf( "\n" );

    exit( 0 );
}

void LogImpl(int priority, const char * format, va_list args) {
    //or implement your logmode here
}

int main( int argc, char * argv[] ) {
    const char * config_file = NULL;
    bool daemonize = false;;
    extern char *optarg ;
    int c ;
    while( ( c = getopt( argc, argv, "c:vd" ) ) != EOF ) {
        switch ( c ) {
            case 'c' : config_file = optarg; break;
            case 'd' : daemonize = true; break;

            case 'v' :
            default: showUsage( argv[ 0 ] ); break;
        }
    }

    if( daemonize ) phxrpc::ServerUtils::Daemonize();

    assert(signal(SIGPIPE, SIG_IGN) != SIG_ERR);

    //set your logfunc
    //phxrpc::setvlog(LogImpl);
    //phxrpc::MonitorFactory::SetFactory( new YourSelfsMonitorFactory() );

    if( NULL == config_file ) showUsage( argv[0] );
    PresenceServerConfig config;
    if( ! config.Read( config_file ) ) showUsage( argv[0] );

    ServiceArgs_t service_args;
    service_args.config = &config;
    service_args.factory = new RedisClientFactory( "~/minichat/etc/client/redis_client.conf" );

    phxrpc::HshaServer server( config.GetHshaServerConfig(), HttpDispatch, &service_args );
    server.RunForever();
    return 0;
}
