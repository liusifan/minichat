
#include "seq_dao.h"

#include "r3c/r3c.h"

#include "phxrpc/file.h"

SeqDAO :: SeqDAO( r3c::CRedisClient & client )
    : client_( client )
{
}

SeqDAO :: ~SeqDAO()
{
}

int SeqDAO :: Alloc( const seq::AllocReq & req,
    google::protobuf::UInt32Value * resp )
{
    char key[ 128 ] = { 0 }, field[ 128 ] = { 0 };

    snprintf( key, sizeof( key ), "seq_%s", req.username().c_str() );
    snprintf( field, sizeof( field ), "%d", req.type() );

    uint64_t ret = client_.hincrby( key, field, 1, NULL );

    resp->set_value( ret );

    return 0;
}

int SeqDAO :: Get( const char * username, seq::SyncKey * resp )
{
    char key[ 128 ] = { 0 };

    snprintf( key, sizeof( key ), "seq_%s", username );

    std::map< std::string, std::string > synckey;

    int ret = client_.hgetall( key, &synckey, NULL );

    std::map< std::string, std::string >::iterator it = synckey.begin();
    for( ; synckey.end() != it; ++it ) {
        int type = atoi( it->first.c_str() );
        if( type == seq::TYPE_CONTACT ) {
            resp->set_contact_seq( atoi( it->second.c_str() ) );
        } else if( type == seq::TYPE_MSG ) {
            resp->set_msg_seq( atoi( it->second.c_str() ) );
        } else {
            phxrpc::log( LOG_ERR, "ERROR: unknown seq %s,%s",
                    it->first.c_str(), it->second.c_str() );
        }
    }

    return ret >= 0 ? 0 : -1;
}

