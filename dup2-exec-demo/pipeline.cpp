#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

// STDIN_FILENO  0
// STDOUT_FILENO 1
// STDERR_FILENO 2

// char *l_args[] = { (char*)"nl", (char*)"-s", (char*)". ", nullptr };
// execvp( "nl", l_args );

// which sort

int main( int argc, char** argv ) 
{
    if ( argc < 2 )
    {
        fprintf( stderr, "Usage: %s names.txt\n", argv[ 0 ] );
        exit( EXIT_FAILURE );
    }

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

    if ( fork() == 0 )
    {
        int x = open( argv[ 1 ], O_RDONLY );

        if ( x < 0 )
        {
            perror( argv[ 1 ] );
            exit( EXIT_FAILURE );
        }
// fd 0 : names.txt
// fd 7 : names.txt
        dup2( x, STDIN_FILENO );
// fd 0 : names.txt
// fd 7 : ( closed )
        close( x );

        dup2( pipeA[ 1 ], STDOUT_FILENO );

        close( pipeA[ 0 ] );
        close( pipeA[ 1 ] );
        close( pipeB[ 0 ] );
        close( pipeB[ 1 ] );
// fd 0 : names.txt
// fd 1 : pipeA[ 1 ]
// fd 2 : terminal
// fd 3 : ( closed )
// ...

        execlp( "sort", "sort", nullptr );
// execl( "/usr/bin/sort", "sort", nullptr );
        perror( "execlp sort" );
        exit( EXIT_FAILURE );
    }

    if ( fork() == 0 )
    {
        dup2( pipeA[ 0 ], STDIN_FILENO );
        dup2( pipeB[ 1 ], STDOUT_FILENO );

        close( pipeA[ 0 ] );
        close( pipeA[ 1 ] );
        close( pipeB[ 0 ] );
        close( pipeB[ 1 ] );

        execlp( "nl", "nl", "-s", ". ", nullptr );
        perror( "execlp nl" );
        exit( EXIT_FAILURE );
    }

    if ( fork() == 0 )
    {
        dup2( pipeB[ 0 ], STDIN_FILENO );

        close( pipeA[ 0 ] );
        close( pipeA[ 1 ] );
        close( pipeB[ 0 ] );
        close( pipeB[ 1 ] );

        execlp( "tr", "tr", "a-z", "A-Z", nullptr );
        perror( "execlp tr" );
        exit( EXIT_FAILURE );
    }

    close( pipeA[ 0 ] );
    close( pipeA[ 1 ] );
    close( pipeB[ 0 ] );
    close( pipeB[ 1 ] );

    while ( wait( NULL ) > 0 )
    {

    }

    exit( EXIT_SUCCESS );

}