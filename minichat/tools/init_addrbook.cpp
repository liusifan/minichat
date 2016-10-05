
#include "init_tools.h"

#include <map>
#include <set>
#include <random>
#include <iostream>

#include "addrbook/addrbook_client.h"

using namespace std;

typedef std::set< int > PostingList;

// count1 -> ( user_1, user_2, user_3, ... )
// count2 -> ( user_x, user_y, user_z, ... )
typedef std::map< int, PostingList > CountInfoMap;

typedef struct tagContactList {
    int max;
    PostingList contacts;
} ContactList_t;

// ContactList_t list[] = {}
// user1 -> { max, ( contact_1, contact_2, contact_3, ... ) }
// user2 -> { max, ( contact_x, contact_y, contact_z, ... ) }

static int const MAX_CONTACT_COUNT = 5000;

int GenContactCountInfo( int count, CountInfoMap * info_map )
{
	std::random_device rd;

	std::exponential_distribution<double> less1k( 46 );
	std::exponential_distribution<double> more1k( 10 );

    std::uniform_int_distribution<int> int_dist( 0, 99 );

    // [ 0 - 1000 ) : 99%, [ 1000 - 5000 ] : 1%
	for( int i = 0; i < count; ++i ) {
        int contact_count = 0;

        double number = 0;
        int idx = int_dist( rd );
        if( 0 == idx ) {
            number = more1k( rd );
            if( number > 1.0 ) number = 1;
            contact_count = 1000 + int( 4000 * number );
        } else {
            number = less1k( rd );
            if( number > 1.0 ) number = 1;
            contact_count = int( MAX_CONTACT_COUNT * number );
        }

        CountInfoMap::iterator iter = info_map->find( contact_count );
        if( info_map->end() == iter ) {
            info_map->insert( std::make_pair( contact_count, PostingList() ) );
            iter = info_map->find( contact_count );
        }

        iter->second.insert( i );
	}

    return 0;
}

int GenOneContactListByOrder( const int index, ContactList_t * list,
        const CountInfoMap & info_map, CountInfoMap::const_reverse_iterator info_iter )
{
    ContactList_t & from = list[ index ];

    for( ; info_map.rend() != info_iter; ++info_iter ) {

        if( from.contacts.size() >= from.max ) break;

        PostingList::iterator user_iter = info_iter->second.begin();
        for( ; info_iter->second.end() != user_iter; ++user_iter ) {

            if( from.contacts.size() >= from.max ) break;

            int to_index = *user_iter;

            if( to_index == index ) continue;

            ContactList_t & to = list[ to_index ];

            if( to.contacts.size() >= to.max ) continue;

            if( from.contacts.end() == from.contacts.find( to_index ) ) {
                from.contacts.insert( to_index );
                to.contacts.insert( index );
            }
        }
    }

    return 0;
}

int GenOneContactListByRandom( int count, ContactList_t * list, int index, const CountInfoMap & info_map )
{
    ContactList_t & from = list[ index ];

    static int seed = std::chrono::system_clock::now().time_since_epoch().count();
    static std::default_random_engine rd( seed );
    std::uniform_int_distribution<int> dist( 0, count - 1 );

    std::pair<std::set<int>::iterator,bool> insert_ret;

    int noop = 0;

    for( ; from.contacts.size() < from.max && noop < 100; ) {
        int to_index = rd() % count;
        if( to_index == index ) continue;

        ContactList_t & to = list[ to_index ];
        if( to.contacts.size() >= to.max ) continue;

        if( from.contacts.end() == from.contacts.find( to_index ) ) {
            insert_ret = from.contacts.insert( to_index );
            to.contacts.insert( index );
        } else {
            noop++;
        }
    }

    if( from.contacts.size() < from.max ) {
        //printf( "index %d, size %zu, max %d\n", index, from.contacts.size(), from.max );
        GenOneContactListByOrder( index, list, info_map, info_map.rbegin() );
    }

    return 0;
}

int GenContactList( int count, const CountInfoMap & info_map, ContactList_t * list, int mode )
{
    // init list
    {
        CountInfoMap::const_iterator info_iter = info_map.begin();
        for( ; info_map.end() != info_iter; ++info_iter ) {

            PostingList::iterator user_iter = info_iter->second.begin();
            for( ; info_iter->second.end() != user_iter; ++user_iter ) {
                list[ *user_iter ].max = info_iter->first;
            }
        }
    }

    int unit = count / 100, progress = 0;

    if( 0 == mode ) {
        CountInfoMap::const_reverse_iterator info_iter = info_map.rbegin();
        for( ; info_map.rend() != info_iter; ++info_iter ) {
            PostingList::iterator user_iter = info_iter->second.begin();
            for( ; info_iter->second.end() != user_iter; ++user_iter ) {
                GenOneContactListByOrder( *user_iter, list, info_map, info_iter );

                if( 0 == ( ++progress % unit ) ) {
                    printf( "#" );
                    fflush( stdout );
                }
            }
        }
    } else {
        for( int i = 0; i < count; i++ ) {
            GenOneContactListByOrder( i, list, info_map, info_map.rbegin() );

            if( 0 == ( ++progress % unit ) ) {
                printf( "#" );
                fflush( stdout );
            }
        }
    }

    printf( "\n" );

    return 0;
}

void PrintDetail( const char * tag, int count, int * p )
{
    std::cout << tag << " distribution" << std::endl;
    std::cout << std::fixed; std::cout.precision(5);

    for (int i=0; i<10; ++i) {
        std::cout << i << ": " << float(p[i]) / count
            << ", cnt: " << p[i]
            << std::endl;
    }

    for (int i=1; i<10; ++i) {
        int total = 0, sum = 0;
        for (int j=0; j<10; ++j) {
            total += p[ 10 * i + j ];
            sum += ( 10 * i + j ) * p[ 10 * i + j ];
        }

        total = ( 0 == total ) ? 1 : total;
        std::cout << i * 10 << ": " << float(total) / count
            << ", cnt: " << total
            << ", avg: " << sum / total << std::endl;
    }

    for (int i=1; i < 10; ++i ) {
        int total = 0, sum = 0;
        for( int j = 0; j < 100; ++j ) {
            total += p[ i * 100 + j ];
            sum += ( i * 100 + j ) * p[ 100 * i + j ];
        }

        total = ( 0 == total ) ? 1 : total;
        std::cout << i * 100 << ": " << float(total) / count
            << ", cnt: " << total
            << ", avg: " << sum / total << std::endl;
    }

    for (int i=1; i < 5; ++i ) {
        int total = 0, sum = 0;
        for( int j = 0; j < 1000; ++j ) {
            total += p[ i * 1000 + j ];
            sum += ( i * 1000 + j ) * p[ i * 1000 + j ];
        }

        total = ( 0 == total ) ? 1 : total;
        std::cout << i * 1000 << ": " << float(total) / count
            << ", cnt: " << total
            << ", avg: " << sum / total << std::endl;
    }
}

void PrintCountInfo( const int count, const CountInfoMap & info_map )
{
    int p[ MAX_CONTACT_COUNT ] = { 0 };

    CountInfoMap::const_iterator iter = info_map.begin();
    for( ; info_map.end() != iter; ++iter ) {
        p[ iter->first ] = iter->second.size();
    }

    PrintDetail( "expectation", count, p );
}

void PrintContactList( const int count, const ContactList_t * list )
{
    printf( "\n" );

    int p[ MAX_CONTACT_COUNT ] = { 0 };

    int total = 0, lack_count = 0, lack_total = 0;

    for( int i = 0; i < count; i++ ) {
        const ContactList_t & item = list[i];
        ++p[ item.contacts.size() ];

        total += item.contacts.size();
        if( item.contacts.size() < item.max ) {
            lack_count++;
            lack_total += item.max - item.contacts.size();

            printf( "%d, %zu, %d\n", i, item.contacts.size(), item.max );
        }
    }

    printf( "total %d, avg %d, lack %d, lack.total %d\n\n",
            total, total / count, lack_count, lack_total );

    PrintDetail( "real", count, p );
}

void CallAddrbook( int count, ContactList_t * list, int begin, int end )
{
    if( end > count ) end = count;

    AddrbookClient client;
    char from[ 128 ] = { 0 }, to[ 128 ] = { 0 };

    for( int i = begin; i < end; i++ ) {
        snprintf( from, sizeof( from ), "user%d", i );
        for( auto & c : list[i].contacts ) {
            snprintf( to, sizeof( to ), "user%d", c );

            addrbook::ContactReq req;
            google::protobuf::Empty resp;

            req.set_username( from );
            req.mutable_contact()->set_username( to );
            req.mutable_contact()->set_remark( to );

            int k = 0;
            int ret = 0;
            for(k = 0; k < 3; k++) {
                ret = client.Set( req, &resp );
                if(0 != ret) {
                    continue;
                } else {
                    break;
                }
            }
            if(k == 3) printf( "add %s - %s fail\n", from, to );
        }

    }
}

int SaveAddrbook( int count, ContactList_t * list, int thread_count )
{
    int unit = ( count + thread_count - 1 ) / thread_count;

    std::thread * threads[ thread_count ];

    for( int i = 0; i < thread_count; i++ ) {
        threads[i] = new std::thread( CallAddrbook, count, list, unit * i, unit * ( i + 1 ) );
    }

    for( int i = 0; i < thread_count; i++ ) {
        try {
            threads[i]->join();
        } catch (const std::exception & ex) {
            cout << ex.what() << endl;
        }
    }

    for( int i = 0; i < thread_count; i++ ) {
        delete threads[i];
    }

    return 0;
}

int SaveToFile( int count, ContactList_t * list, const char * path )
{
    FILE * fp = fopen( path, "w" );
    if( NULL != fp ) {

        fprintf( fp, "#%d\n", count );

        for( int i = 0; i < count; i++ ) {
            fprintf( fp, "#%d\n", i );
            fprintf( fp, "#%zu\n", list[i].contacts.size() );
            for( auto & u : list[i].contacts ) {
                fprintf( fp, "\t%d\n", u );
            }
        }
        fclose( fp );
    }

    return 0;
}

int GenAddrbook( const int count, int mode, const char * path )
{
    CountInfoMap info_map;
    GenContactCountInfo( count, &info_map );

    PrintCountInfo( count, info_map );

    ContactList_t * list = new ContactList_t[ count ];

    GenContactList( count, info_map, list, mode );

    PrintContactList( count, list );

    SaveToFile( count, list, path );

    delete [] list;

    return 0;
}

int LoadAddrbook( const char * path, int thread_count )
{
    int count = 0;
    ContactList_t * list = NULL;

    FILE * fp = fopen( path, "r" );
    if( NULL != fp ) {
        char line[ 128 ] = { 0 };
        if( NULL != fgets( line, sizeof( line ), fp ) ) {
            count = atoi( line + 1 );
        }

        list = new ContactList_t[ count ];

        for( int i = 0; i < count; i++ ) {
            int index = 0;

            if( NULL == fgets( line, sizeof( line ), fp ) ) break;
            index = atoi( line + 1 );

            assert( i == index );

            if( NULL == fgets( line, sizeof( line ), fp ) ) break;
            list[i].max = atoi( line + 1 );

            for( int j = 0; j < list[i].max; j++ ) {
                if( NULL == fgets( line, sizeof( line ), fp ) ) break;
                list[i].contacts.insert( atoi( line ) );
            }
        }

        fclose( fp );
    }

    if( NULL != list ) {

        PrintContactList( count, list );

        SaveAddrbook( count, list, thread_count );

        delete [] list;
    }

    return 0;
}

