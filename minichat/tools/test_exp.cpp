// exponential_distribution
#include <iostream>
#include <random>
#include <stdlib.h>

int main( int argc, char * argv[] )
{
    if( argc < 3 ) {
        printf( "Usage: %s <lambda in 1k> <lambda out 1k>\n", argv[0] );
        return -1;
    }

    double lambda_in_1k = strtof( argv[1], NULL );
    double lambda_out_1k = strtof( argv[2], NULL );

	const int nrolls=1000000;  // number of experiments
	const int nintervals=5000; // number of intervals

	std::default_random_engine generator;
	std::exponential_distribution<double> less1k( lambda_in_1k );
	std::exponential_distribution<double> more1k( lambda_out_1k );
    std::uniform_int_distribution<int> int_dist(0,99);

	int p[nintervals]={};

	for (int i=0; i<nrolls; ++i) {
        int idx = int_dist( generator );
        if( 0 == idx ) {
		    double number = more1k( generator );

            if( number > 1.0 ) number = 0.99;
            ++p[ 1000 + int( 4000 * number ) ];
            continue;
        }

		double number = less1k( generator );
		if (number<1.0) ++p[int(nintervals*number)];
        else printf( "%.2f\n", number );
	}

	std::cout << "exponential_distribution (" << lambda_in_1k << "):" << std::endl;
	std::cout << std::fixed; std::cout.precision(5);

	for (int i=0; i<10; ++i) {
		//std::cout << float(i)/nintervals << "-" << float(i+1)/nintervals << ": ";
		//std::cout << std::string(p[i]*nstars/nrolls,'*') << std::endl;
        std::cout << i << ": " << float(p[i]) / nrolls << std::endl;
	}

    for (int i=1; i<10; ++i) {
        int total = 0;
        for (int j=0; j<10; ++j) {
            total += p[ 10 * i + j ];
        }

        std::cout << i * 10 << ": " << float(total) / nrolls << std::endl;
    }

    for (int i=1; i < 10; ++i ) {
        int total = 0;
        for( int j = 0; j < 100; ++j ) {
            total += p[ i * 100 + j ];
        }

        std::cout << i * 100 << ": " << float(total) / nrolls << std::endl;
    }

    for (int i=1; i < 5; ++i ) {
        int total = 0;
        for( int j = 0; j < 1000; ++j ) {
            total += p[ i * 1000 + j ];
        }

        std::cout << i * 1000 << ": " << float(total) / nrolls << std::endl;
    }

	return 0;
}
