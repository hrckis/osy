#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <ctime>
#include <sys/wait.h>

#include "svatky.hpp"

const int days[ 12 ] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

int main( int argc, char** argv ) 
{
    if ( argc != 3 )
    {
        fprintf( stderr, "Usage: %s M N\n", argv[ 0 ] );
        exit( EXIT_FAILURE );
    }

    int M = atoi( argv[ 1 ] );
    int N = atoi( argv[ 2 ] );

    if ( M <= 0 || N <= 0 )
    {
        fprintf( stderr, "M a N must be positive\n" );
        exit( EXIT_FAILURE );
    }

    int pipefd[ 2 ];

    if ( pipe( pipefd ) < 0 ) 
    {
        perror( "pipe" );
        exit( EXIT_FAILURE );
    }

    int child = fork();

    if ( child < 0 ) 
    {
        perror( "fork" );
        exit( EXIT_FAILURE );
    }

    if ( child == 0 ) 
    {
        close( pipefd[ 0 ] );
        
        srand( time( NULL ) );
        unsigned int l_sleep = 1000000 / N;

        for ( int i = 0; i < M; i++ )
        {
            int month = rand() % 12;  
            int day = rand() % days[ month ] + 1;

            char l_buffer[ 128 ];
            int ret = snprintf( l_buffer, sizeof l_buffer, "%d.%d.\n", day, month + 1 );
            write( pipefd[ 1 ], l_buffer, ret );
            
            usleep( l_sleep );
        }

        close( pipefd[ 1 ] );
        exit( EXIT_SUCCESS );
    }


    close( pipefd[ 1 ] );

    FILE* l_in = fdopen( pipefd[ 0 ], "r" );
    if ( l_in == NULL ) 
    {
        perror( "fdopen" );
        exit( EXIT_FAILURE );
    }

    char l_buf[ 256 ];
    while ( fgets( l_buf, sizeof l_buf, l_in ) != NULL )
    {

        for ( int i = 0; l_buf[ i ] != '\0'; i++ )
        {
            if ( l_buf[ i ] == '\n' ) 
            { 
                l_buf[ i ] = '\0'; 
                break; 
            }
        }

        const char* l_name = "-";

        for ( int i = 0; g_svatky[ i ][ 0 ] != nullptr; i++ )
        {
            if ( strcmp( g_svatky[ i ][ 0 ], l_buf ) == 0 )
            {
                l_name = g_svatky[ i ][ 1 ];
                break;
            }
        }

        printf( "%s %s\n", l_buf, l_name );
    }

    // if ( sscanf( l_buf, "%d.%d.", &l_day, &l_month ) == 2 ) { printf( "%d.%d. : %s", l_day, l_month,  ); }

    fclose( l_in );

    int l_status;
    while ( wait( &l_status ) > 0 )
    {

    }

    return EXIT_SUCCESS;
}