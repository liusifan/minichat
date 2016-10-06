
#include "r3c/r3c.h"

#include "msgbox_dao.h"

#include <string>
#include <sys/time.h>

#include "phxrpc/file.h"

#include "seq/seq_client.h"

#include "common/redis_client_ex.h"

MsgBoxDAO :: MsgBoxDAO( r3c::CRedisClient & client )
: client_( client )
{
}

MsgBoxDAO ::  ~MsgBoxDAO()
{
}

int MsgBoxDAO :: Add( const msgbox::MsgIndex & req,
        msgbox::AddMsgResp * resp )
{
    int ret = -1;

    RedisClientEx client_ex( client_ );

    char key[ 128 ] = { 0 };
    snprintf( key, sizeof( key ), "msg_%s", req.to().c_str() );

    msgbox::MsgIndex tmp = req;
    struct timeval tv;
    gettimeofday( &tv, NULL );
    tmp.set_id( ((uint64_t)tv.tv_sec) << 32 | tv.tv_usec );

    for( int retry = 0; retry < 3; retry++ ) {
        if( ! client_ex.RedisBinCommand( key, "WATCH", "", "OK" ) ) {
            continue;
        }

        SeqClient seq_client;
        {
            seq::AllocReq alloc_req;
            google::protobuf::UInt32Value seq;

            alloc_req.set_username( req.to() );
            alloc_req.set_type( seq::TYPE_MSG );

            int ret = seq_client.Alloc( alloc_req, &seq );

            if( 0 != ret ) return ret;

            tmp.set_seq( seq.value() );
            tmp.set_createtime( time( NULL ) );
        }

        if( ! client_ex.RedisMulti( key ) ) continue;

        std::string value;
        tmp.SerializeToString( &value );

        if( ! client_ex.RedisBinCommand( key, "RPUSH", value, "QUEUED" ) ) {
            continue;
        }

        if( client_ex.RedisExec( key ) ) {
            resp->set_id( tmp.id() );
            resp->set_newcount( ret );

            ret = 0;
            break;
        } else {
            continue;
        }
    }

    return ret;
}

int MsgBoxDAO :: GetBySeq( const msgbox::GetBySeqReq & req,
        msgbox::MsgIndexList * resp )
{
    int ret = -1;

    RedisClientEx client_ex( client_ );

    char key[ 128 ] = { 0 }, line[ 256 ] = { 0 };
    snprintf( key, sizeof( key ), "msg_%s", req.username().c_str() );

    for( int i = 0; i < 3; i++ ) {

        if( ! client_ex.RedisBinCommand( key, "WATCH", "", "OK" ) ) {
            continue;
        }

        std::vector< std::string > list;

        client_.lrange( key, 0, -1, &list );

        if( ! client_ex.RedisMulti( key ) ) continue;

        resp->clear_msg();

        ::msgbox::MsgIndex * msg = resp->add_msg();

        for( size_t i = 0; i < list.size(); i++ ) {
            msg->ParseFromString( list[i] );
            if( msg->seq() > req.seq() ) {
                msg = resp->add_msg();
            } else {
                snprintf( line, sizeof( line ), "LSET %s %zu Deleted", key, i );
                if( ! client_ex.RedisCommand( key, "LSET", line, "QUEUED" ) ) {
                    continue;
                }
            }
        }

        resp->mutable_msg()->RemoveLast();

        snprintf( line, sizeof( line ), "LREM %s 0 Deleted", key );
        if( ! client_ex.RedisCommand( key, "LREM", line, "QUEUED" ) ) {
            continue;
        }

        if( client_ex.RedisExec( key ) ) {
            ret = 0;
            break;
        }
    }

    return ret;
}

int MsgBoxDAO :: GetAll( const google::protobuf::StringValue & req,
        msgbox::MsgIndexList * resp )
{
    char key[ 128 ] = { 0 };
    snprintf( key, sizeof( key ), "msg_%s", req.value().c_str() );

    std::vector< std::string > list;

    int ret = client_.lrange( key, 0, -1, &list );

    for( size_t i = 0; i < list.size(); i++ ) {
        resp->add_msg()->ParseFromString( list[i] );
    }

    return ret >= 0 ? 0 : -1;
}

