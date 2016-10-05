
#pragma once

#include <string>

extern int InitAccount( int begin_idx, int end_idx );
extern int InitProfile( int begin_idx, int end_idx );
extern int GenAddrbook( int count, int mode, const char * path );
extern int LoadAddrbook( const char * path, int thread_count );

