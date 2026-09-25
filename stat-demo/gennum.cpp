#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <unistd.h>

int main( int argc, char **argv )
{
    if ( argc < 3 )
    {
        fprintf( stderr, "Usage: %s M N\n", argv[ 0 ] );
        exit( EXIT_FAILURE );
    }

    int M = atoi( argv[ 1 ] );
    int N = atoi( argv[ 2 ] );

    if ( M <= 0 || N <= 0 )
    {
        fprintf( stderr, "M & N must be positive\n" );
        exit( EXIT_FAILURE );
    }

    srand( time( NULL ) );

    unsigned int l_sleep = 60000000 / N;

    while ( true )
    {
        for ( int j = 0; j < M; j++ )
        {
            if ( j > 0 ) printf( " " ); 
            printf( "%d", rand() % ( 1000 - 10 + 1 ) + 10 );
        }
        printf( "\n" );
        fflush( stdout );
        usleep( l_sleep );
    }

    return 0;
}