
#include "addrbook_dao.h"

#include "r3c/r3c.h"

#include "addrbook.pb.h"

#include "seq/seq_client.h"

AddrbookDAO :: AddrbookDAO( r3c::CRedisClient & client )
    : client_( client )
{
    //SeqClient::Init( "~/minichat/etc/client/seq_client.conf" );
}

AddrbookDAO :: ~AddrbookDAO()
{
}

int AddrbookDAO :: Set( const addrbook::ContactReq & req,
    google::protobuf::Empty * resp )
{
    char key[ 128 ] = { 0 };
    snprintf( key, sizeof( key ), "addrbook_%s", req.username().c_str() );

    addrbook::Contact contact = req.contact();

    // TODO: use transaction commands, watch/multi/exec
    for( int i = 0; i < 1; i++ ) {
        SeqClient seq_client;
        {
            seq::AllocReq alloc_req;
            google::protobuf::UInt32Value seq;

            alloc_req.set_username( req.username() );
            alloc_req.set_type( seq::TYPE_CONTACT );

            int ret = seq_client.Alloc( alloc_req, &seq );

            if( 0 != ret ) return ret;

            contact.set_seq( seq.value() );
            contact.set_updatetime( time( NULL ) );
        }

        std::string value;
        contact.SerializeToString( &value );

        client_.hset( key, contact.username(), value, NULL );
    }

    return 0;
}

int AddrbookDAO :: GetAll( const google::protobuf::StringValue & req,
    addrbook::ContactList * resp )
{
    char key[ 128 ] = { 0 };
    snprintf( key, sizeof( key ), "addrbook_%s", req.value().c_str() );

    std::map< std::string, std::string > list;

    int ret = client_.hgetall( key, &list, NULL );

    std::map< std::string, std::string >::iterator it = list.begin();
    for( ; list.end() != it; ++it ) {
        resp->add_contact()->ParseFromString( it->second );
    }

    return ret >= 0 ? 0 : -1;
}

int AddrbookDAO :: GetOne( const addrbook::GetOneReq & req,
    addrbook::Contact * resp )
{
    char key[ 128 ] = { 0 };
    snprintf( key, sizeof( key ), "addrbook_%s", req.username().c_str() );

    std::string value;

    bool ret = client_.hget( key, req.contact().c_str(), &value );
    if( ret ) resp->ParseFromString( value );

    return ret ? 0 : -2;
}

int AddrbookDAO :: GetBySeq( const addrbook::GetBySeqReq & req,
    addrbook::ContactList * resp )
{
    char key[ 128 ] = { 0 };
    snprintf( key, sizeof( key ), "addrbook_%s", req.username().c_str() );

    std::map< std::string, std::string > list;

    int ret = client_.hgetall( key, &list, NULL );

    addrbook::Contact * contact = resp->add_contact();

    std::map< std::string, std::string >::iterator it = list.begin();
    for( ; list.end() != it; ++it ) {
        contact->ParseFromString( it->second );
        if( contact->seq() > req.seq() ) {
            contact = resp->add_contact();
        }
    }

    resp->mutable_contact()->RemoveLast();

    return ret >= 0 ? 0 : -1;
}

