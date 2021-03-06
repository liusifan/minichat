#include <string>
#include <iostream>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>


#include "r3c/r3c.h"
#include "phxrpc/file.h"
#include "phxrpc/rpc.h"

#include "common/redis_client_config.h"

using namespace phxrpc;
using namespace std;


int main(int argc, const char *argv[])
{

    if(argc < 4) {
        cout << "conf_path redis_cli_conf package_name" << endl;
        return -1;

    }
    const char * conf_path = argv[1];
    const char * redis_cli_conf = argv[2];
    const char * package_name = argv[3];

    string content;
    if(!FileUtils::ReadFile(conf_path, &content)) {
        cout << "read file " << conf_path << " failed" << endl;
        return -1;
    }

    {
        int tmp = 0;
        phxrpc::Config config;
        config.InitConfig( redis_cli_conf );
        bool ret = config.ReadItem( "Server", "SererCount", &tmp );
    }

    r3c::CRedisClient * client = NULL;
    RedisClientConfig config;

    if(config.Read(redis_cli_conf)) {
        cout << "nodes " << config.GetNodes() << endl;
        client = new r3c::CRedisClient( config.GetNodes() );
    } else {
        cout << "read redis client conf failed " << redis_cli_conf << endl;
        return -1;
    }

    string key("cliconf:");
    key.append(package_name);

    client->set(key, content);

    cout << "set " << package_name << endl;

    string tmp_content;
    if(!client->get(key, &tmp_content)) {
        cout << "get failed" << endl;
    } else {
        cout << "-----------------------" << endl;
        cout << tmp_content << endl;

    }

    delete client;
    return 0;
}


