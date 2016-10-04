
#include "push_client_factory.h"

#include "push_client.h"

PushClientFactory * PushClientFactory :: GetDefault()
{
    static PushClientFactory factory;

    return &factory;
}

PushClientFactory :: PushClientFactory()
{
}

PushClientFactory :: ~PushClientFactory()
{
}

PushClient & PushClientFactory :: Get()
{
    static __thread PushClient * client = NULL;
    
    if( NULL == client ) client = new PushClient();

    return * client;
}

