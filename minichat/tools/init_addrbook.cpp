
#include "init_tools.h"

#include <map>
#include <set>
#include <random>
#include <iostream>


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

int GenOneContactResult( const int index, ContactList_t * list,
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

int GenContactResult( int count, const CountInfoMap & info_map, ContactList_t * list )
{
    // init result
    {
        CountInfoMap::const_iterator info_iter = info_map.begin();
        for( ; info_map.end() != info_iter; ++info_iter ) {

            PostingList::iterator user_iter = info_iter->second.begin();
            for( ; info_iter->second.end() != user_iter; ++user_iter ) {
                list[ *user_iter ].max = info_iter->first;
            }
        }
    }

    printf( "Init result DONE\n" );

    // gen contact list
    {
        CountInfoMap::const_reverse_iterator info_iter = info_map.rbegin();
        for( ; info_map.rend() != info_iter; ++info_iter ) {

            PostingList::iterator user_iter = info_iter->second.begin();
            for( ; info_iter->second.end() != user_iter; ++user_iter ) {
                GenOneContactResult( *user_iter, list, info_map, info_iter );
            }
        }
    }

    return 0;
}

void PrintCountInfo( const int count, const CountInfoMap & info_map )
{
    size_t p[ MAX_CONTACT_COUNT ] = { 0 };

    CountInfoMap::const_iterator iter = info_map.begin();
    for( ; info_map.end() != iter; ++iter ) {
        p[ iter->first ] = iter->second.size();
    }

	std::cout << "exponential_distribution" << std::endl;
	std::cout << std::fixed; std::cout.precision(5);

	for (int i=0; i<10; ++i) {
        std::cout << i << ": " << float(p[i]) / count << std::endl;
	}

    for (int i=1; i<10; ++i) {
        int total = 0;
        for (int j=0; j<10; ++j) {
            total += p[ 10 * i + j ];
        }

        std::cout << i * 10 << ": " << float(total) / count << std::endl;
    }

    for (int i=1; i < 10; ++i ) {
        int total = 0;
        for( int j = 0; j < 100; ++j ) {
            total += p[ i * 100 + j ];
        }

        std::cout << i * 100 << ": " << float(total) / count << std::endl;
    }

    for (int i=1; i < 5; ++i ) {
        int total = 0;
        for( int j = 0; j < 1000; ++j ) {
            total += p[ i * 1000 + j ];
        }

        std::cout << i * 1000 << ": " << float(total) / count << std::endl;
    }
}

void PrintContactMap( const int count, const ContactList_t * list )
{
    int total = 0;

    for( int i = 0; i < count; i++ ) {
        total += list[i].contacts.size();
    }

    printf( "%d, %d\n", total, total / count );
}

int InitAddrbook( const int count, int thread_count )
{
    CountInfoMap info_map;
    GenContactCountInfo( count, &info_map );

    PrintCountInfo( count, info_map );

    ContactList_t * list = new ContactList_t[ count ];

    GenContactResult( count, info_map, list );

    PrintContactMap( count, list );

    delete [] list;

    return 0;
}

