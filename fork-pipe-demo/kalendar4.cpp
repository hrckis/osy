#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <ctime>
#include <sys/wait.h>

#include "svatky.hpp"

const int days[ 12 ] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

void print_res( int t_fd ) {

    FILE *l_in = fdopen( t_fd, "r" );

    if ( l_in == NULL )
    {
        perror( "fdopen" );
        exit( EXIT_FAILURE );
    }

    int l_i = 1;
    char l_buf[ 256 ];
    while ( fgets( l_buf, sizeof l_buf, l_in ) != NULL )
    {
        printf( "(%d) %s", l_i, l_buf );
        l_i++;
    }

    fclose( l_in );
}

int main( int argc, char** argv ) 
{
    if ( argc != 3 )
    {
        fprintf( stderr, "Usage: %s M N\n", argv[ 0 ] );
        exit( EXIT_FAILURE );
    }

    int M = atoi( argv[ 1 ] );
    int N = atoi( argv[ 2 ] );

    int pipeA[ 2 ];
    int pipeB[ 2 ];
    int pipeC[ 2 ];
    int pipeD[ 2 ];

    if ( pipe( pipeA ) < 0 ) 
    {
        perror( "pipeA" );
        exit( EXIT_FAILURE );
    }

    if ( pipe( pipeB ) < 0 ) 
    {
        perror( "pipeB" );
        exit( EXIT_FAILURE );
    }

    if ( pipe( pipeC ) < 0 ) 
    {
        perror( "pipeC" );
        exit( EXIT_FAILURE );
    }

    if ( pipe( pipeD ) < 0 ) 
    {
        perror( "pipeD" );
        exit( EXIT_FAILURE );
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - 
    int length_g_svatky = 0;
    for ( int i = 0; g_svatky[ i ][ 0 ] != nullptr; i++ )
    {
        length_g_svatky++;
    }

    // child 1 generator
    if ( fork() == 0 ) 
    {
        close( pipeA[ 0 ] );
        close( pipeB[ 0 ] );
        close( pipeB[ 1 ] );
        close( pipeC[ 0 ] );
        close( pipeD[ 0 ] );
        close( pipeD[ 1 ] );
        
        srand( time( NULL ) );
        unsigned int l_sleep = 1000000 / N;

        for ( int i = 0; i < M; i++ )
        {
            int month = rand() % 12;  
            int day = rand() % days[ month ] + 1;
            int rand_name = rand() % length_g_svatky;
            const char* l_name = g_svatky[ rand_name ][ 1 ];

            char l_buffer[ 128 ];
            char l_buffer2[ 128 ];
            int l_ret = snprintf( l_buffer, sizeof l_buffer, "%d.%d.\n", day, month + 1 );
            int l_ret_name = snprintf( l_buffer2, sizeof l_buffer2, "%s\n", l_name );
            write( pipeA[ 1 ], l_buffer, l_ret );
            write( pipeC[ 1 ], l_buffer2, l_ret_name );
            
            usleep( l_sleep );
        }

        close( pipeA[ 1 ] );
        close( pipeC[ 1 ] );

        exit( EXIT_SUCCESS );
    }

    // child 2 open and send to child 3
    if ( fork() == 0 ) 
    {
        close( pipeA[ 1 ] );
        close( pipeB[ 0 ] );
        close( pipeC[ 0 ] );
        close( pipeC[ 1 ] );
        close( pipeD[ 0 ] );
        close( pipeD[ 1 ] );

        FILE* l_in = fdopen( pipeA[ 0 ], "r" );
        if ( l_in == NULL ) 
        {
            perror( "fdopen" );
            exit( EXIT_FAILURE );
        }

        char l_buf[ 64 ];
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

            char l_out[ 128 ];
            snprintf( l_out, sizeof l_out, "%s %s\n", l_buf, l_name );
            write( pipeB[ 1 ], l_out, strlen( l_out ) );
        }

        fclose( l_in );
        close( pipeB[ 1 ] );
        exit( EXIT_SUCCESS );
    }

    // child 3 print
    if ( fork() == 0 )
    {
        close( pipeA[ 0 ] );
        close( pipeA[ 1 ] );
        close( pipeB[ 1 ] );
        close( pipeC[ 0 ] );
        close( pipeC[ 1 ] );
        close( pipeD[ 0 ] );
        close( pipeD[ 1 ] );

        print_res( pipeB[ 0 ] );

        exit( EXIT_SUCCESS );
    }

    // child 4
    if ( fork() == 0 )
    {
        close( pipeA[ 0 ] );
        close( pipeA[ 1 ] );
        close( pipeB[ 0 ] );
        close( pipeB[ 1 ] );
        // close( pipeC[ 0 ] );
        close( pipeC[ 1 ] );
        close( pipeD[ 0 ] );
        // close( pipeD[ 1 ] );

        FILE* l_in = fdopen( pipeC[ 0 ], "r" );
        if ( l_in == NULL )
        {
            perror( "fdopen" );
            exit( EXIT_FAILURE );
        }

        char l_buf[ 1024 ];
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

            const char* l_date = "";
            for ( int i = 0; g_svatky[ i ][ 0 ] != nullptr; i++ )
            {
                if ( strcmp( g_svatky[ i ][ 1 ], l_buf ) == 0 )
                {
                    l_date = g_svatky[ i ][ 0 ];
                    break;
                }
            }
            
            char l_out[ 128 ];
            int l_ret = snprintf( l_out, sizeof l_out, "%s %s\n", l_date, l_buf );
            write( pipeD[ 1 ], l_out, l_ret );
        }

        fclose( l_in );
        close( pipeD[ 1 ] );

        exit( EXIT_SUCCESS );
    }

    // child 5
    if ( fork() == 0 )
    {
        close( pipeA[ 0 ] );
        close( pipeA[ 1 ] );
        close( pipeB[ 0 ] );
        close( pipeB[ 1 ] );
        close( pipeC[ 0 ] );
        close( pipeC[ 1 ] );
        close( pipeD[ 1 ] );

        print_res( pipeD[ 0 ] );

        exit( EXIT_SUCCESS );
    }

    // parent
    close( pipeA[ 0 ] );
    close( pipeA[ 1 ] );
    close( pipeB[ 0 ] );
    close( pipeB[ 1 ] );
    close( pipeC[ 0 ] );
    close( pipeC[ 1 ] );
    close( pipeD[ 0 ] );
    close( pipeD[ 1 ] );


    while ( wait( NULL ) > 0 )
    {
    }

    return EXIT_SUCCESS;
}