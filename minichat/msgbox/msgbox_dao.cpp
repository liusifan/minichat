
#include "r3c/r3c.h"

#include "msgbox_dao.h"

#include <string>
#include <sys/time.h>

MsgBoxDAO :: MsgBoxDAO( r3c::CRedisClient & client )
: client_( client )
{
}

MsgBoxDAO ::  ~MsgBoxDAO()
{
}

int MsgBoxDAO :: Add( const msgbox::MsgIndex & req,
                     google::protobuf::UInt64Value * resp )
{
    struct timeval tv;
    gettimeofday( &tv, NULL );

    msgbox::MsgIndex tmp = req;
    tmp.set_id( tv.tv_sec << 32 | tv.tv_usec );

    char key[ 128 ] = { 0 };
    snprintf( key, sizeof( key ), "msg_%s", req.to().c_str() );

    std::string value;
    tmp.SerializeToString( &value );

    std::pair< std::string, uint16_t > which;

    int ret = client_.rpush( key, value, &which );

    if( 0 == ret ) resp->set_value( tmp.id() );

    return ret;
}

int MsgBoxDAO :: GetBySeq( const msgbox::GetBySeqReq & req,
                          msgbox::MsgIndexList * resp )
{
    char key[ 128 ] = { 0 };
    snprintf( key, sizeof( key ), "msg_%s", req.userid().c_str() );

    std::vector< std::string > list;

    int ret = client_.lrange( key, 0, -1, &list );

    bool has_msg = false;
    ::msgbox::MsgIndex * msg = resp->add_msg();

    for( size_t i = 0; i < list.size(); i++ ) {
        msg->ParseFromString( list[i] );

        if( msg->seq() > req.seq() ) {
            has_msg = true;
            msg = resp->add_msg();
        }
    }

    resp->mutable_msg()->RemoveLast();

    // TODO: delete msg by seq
    client_.ltrim( key, 0, -1 );

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

    return ret;
}

