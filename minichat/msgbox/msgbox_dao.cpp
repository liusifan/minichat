
#include "r3c/r3c.h"

#include "msgbox_dao.h"

#include <string>
#include <sys/time.h>

#include "phxrpc/file.h"

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
    struct timeval tv;
    gettimeofday( &tv, NULL );

    msgbox::MsgIndex tmp = req;
    tmp.set_id( ((uint64_t)tv.tv_sec) << 32 | tv.tv_usec );

    char key[ 128 ] = { 0 };
    snprintf( key, sizeof( key ), "msg_%s", req.to().c_str() );

    std::string value;
    tmp.SerializeToString( &value );

    std::vector< std::string > list;
    list.push_back( value );

    int ret = client_.rpush( key, list, NULL );

    if( ret >= 0 ) {
        resp->set_id( tmp.id() );
        resp->set_newcount( ret );
    }

    return ret >= 0 ? 0 : -1;
}

int MsgBoxDAO :: GetBySeq( const msgbox::GetBySeqReq & req,
        msgbox::MsgIndexList * resp )
{
    char key[ 128 ] = { 0 };
    snprintf( key, sizeof( key ), "msg_%s", req.userid().c_str() );

    std::vector< std::string > list;

    int ret = client_.lrange( key, 0, -1, &list );

    ::msgbox::MsgIndex * msg = resp->add_msg();

    for( size_t i = 0; i < list.size(); i++ ) {
        msg->ParseFromString( list[i] );
        if( msg->seq() > req.seq() ) {
            msg = resp->add_msg();
        }
    }

    resp->mutable_msg()->RemoveLast();

    // TODO: delete msg by seq
    client_.ltrim( key, 1, 0 );

    return ret >= 0 ? 0 : -1;
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

