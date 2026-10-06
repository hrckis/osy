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

    int pipeA[ 2 ];
    int pipeB[ 2 ];

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

    // child 1 generator
    if ( fork() == 0 ) 
    {
        close( pipeA[ 0 ] );
        close( pipeB[ 0 ] );
        close( pipeB[ 1 ] );
        
        srand( time( NULL ) );
        unsigned int l_sleep = 1000000 / N;

        for ( int i = 0; i < M; i++ )
        {
            int month = rand() % 12;  
            int day = rand() % days[ month ] + 1;

            char l_buffer[ 128 ];
            int ret = snprintf( l_buffer, sizeof l_buffer, "%d.%d.\n", day, month + 1 );
            write( pipeA[ 1 ], l_buffer, ret );
            
            usleep( l_sleep );
        }

        close( pipeA[ 1 ] );
        exit( EXIT_SUCCESS );
    }

    // child 2 open and send to child 3
    if ( fork() == 0 ) 
    {
        close( pipeA[ 1 ] );
        close( pipeB[ 0 ] );

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

        FILE* pipe_in = fdopen( pipeB[ 0 ], "r" );
        if ( pipe_in == NULL )
        {
            perror( "fdopen" );
            exit( EXIT_FAILURE );
        }

        int i = 1;
        char buff[ 256 ];
        while( fgets( buff, sizeof buff, pipe_in ) != NULL )
        {
            printf( "(%d) %s", i, buff );
            i++;
        }

        fclose( pipe_in );
        exit( EXIT_SUCCESS );
    }

    // parent
    close( pipeA[ 0 ] );
    close( pipeA[ 1 ] );
    close( pipeB[ 0 ] );
    close( pipeB[ 1 ] );

    int l_status;
    pid_t l_pid;

    while ( ( l_pid = wait( &l_status ) ) > 0 )
    {
        if ( WIFEXITED( l_status ) )
        {
            fprintf( stderr, "child %d, exit status %d\n", l_pid, WEXITSTATUS( l_status ) );
        }
        else if ( WIFSIGNALED( l_status ) )
        {
            fprintf( stderr, "child %d, killed %d\n", l_pid, WTERMSIG( l_status ) );
        }
    }

    return EXIT_SUCCESS;
}