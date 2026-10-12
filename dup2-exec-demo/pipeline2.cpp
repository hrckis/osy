#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

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

        dup2( x, STDIN_FILENO );
        close( x );

        dup2( pipeA[ 1 ], STDOUT_FILENO );

        close( pipeA[ 0 ] );
        close( pipeA[ 1 ] );
        close( pipeB[ 0 ] );
        close( pipeB[ 1 ] );

        execlp( "sort", "sort", nullptr );
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

        char l_buf[ 1024 ];
        int l_line = 1;
        while ( fgets( l_buf, sizeof( l_buf ), stdin ) != NULL )
        {
            l_buf[ strcspn( l_buf, "\n" ) ] = '\0';
            printf( "%6d. %s\n", l_line, l_buf );
            l_line++;
        }

        exit( EXIT_SUCCESS );
    }

    if ( fork() == 0 )
    {
        dup2( pipeB[ 0 ], STDIN_FILENO );

        if ( argc >= 3 )
        {
            int x = open( argv[ 2 ], O_WRONLY | O_CREAT | O_TRUNC, 0644 );
// O_CREAT : create file if it does not exist
// O_TRUNC : truncate file to zero length if it exists
// 0644    : file permissions (rw-r--r--)

            if ( x < 0 )
            {
                perror( argv[ 2 ] );
                exit( EXIT_FAILURE );
            }

            dup2( x, STDOUT_FILENO );
            close( x );
        }

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