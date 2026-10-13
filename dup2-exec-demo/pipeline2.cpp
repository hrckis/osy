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
        fprintf( stderr, "Usage: %s bad.jpg\n", argv[ 0 ] );
        exit( EXIT_FAILURE );
    }

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
        close( pipeC[ 0 ] );
        close( pipeC[ 1 ] );
        close( pipeD[ 0 ] );
        close( pipeD[ 1 ] );

        execlp( "./myrev", "./myrev", nullptr );
        perror( "execlp myrev" );
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
        close( pipeC[ 0 ] );
        close( pipeC[ 1 ] );
        close( pipeD[ 0 ] );
        close( pipeD[ 1 ] );

        execlp( "sort", "sort", "-n",  nullptr );
        perror( "execlp sort" );
        exit( EXIT_FAILURE );
    }

    if ( fork() == 0 )
    {
        dup2( pipeB[ 0 ], STDIN_FILENO );
        dup2( pipeC[ 1 ], STDOUT_FILENO );

        close( pipeA[ 0 ] );
        close( pipeA[ 1 ] );
        close( pipeB[ 0 ] );
        close( pipeB[ 1 ] );
        close( pipeC[ 0 ] );
        close( pipeC[ 1 ] );
        close( pipeD[ 0 ] );
        close( pipeD[ 1 ] );

        execlp( "cut", "cut", "-d", ".", "-f", "2", nullptr );
        perror( "execlp cut" );
        exit( EXIT_FAILURE );
    }

    if ( fork() == 0 )
    {
        dup2( pipeC[ 0 ], STDIN_FILENO );
        dup2( pipeD[ 1 ], STDOUT_FILENO );

        close( pipeA[ 0 ] );
        close( pipeA[ 1 ] );
        close( pipeB[ 0 ] );
        close( pipeB[ 1 ] );
        close( pipeC[ 0 ] );
        close( pipeC[ 1 ] );
        close( pipeD[ 0 ] );
        close( pipeD[ 1 ] );

        char l_buf[ 1024 ];

        while ( fgets( l_buf, sizeof( l_buf ), stdin ) != NULL )
        {
            l_buf[ strcspn( l_buf, "\n" ) ] = '\0';
            for ( int i = 0; i < strlen( l_buf ) - 1; i += 2 )
            {
                char l_pair[ 3 ];

                l_pair[ 0 ] = l_buf[ i ];
                l_pair[ 1 ] = l_buf[ i + 1 ];
                l_pair[ 2 ] = '\0';

                long l_byte = strtol( l_pair, NULL, 16 );
                putchar( (int) l_byte );
            }
        }

        exit( EXIT_FAILURE );
    }

    if ( fork() == 0 )
    {
        dup2( pipeD[ 0 ], STDIN_FILENO );

        close( pipeA[ 0 ] );
        close( pipeA[ 1 ] );
        close( pipeB[ 0 ] );
        close( pipeB[ 1 ] );
        close( pipeC[ 0 ] );
        close( pipeC[ 1 ] );
        close( pipeD[ 0 ] );
        close( pipeD[ 1 ] );

        if ( argc >= 3 )
        {
            int x = open( argv[ 2 ], O_WRONLY | O_CREAT | O_TRUNC, 0644 );

            if ( x < 0 )
            {
                perror( argv[ 2 ] );
                exit( EXIT_FAILURE );
            }

            dup2( x, STDOUT_FILENO );
            close( x );
        }
        else 
        {
            execlp( "display", "display", "-", nullptr );
        }

        perror( "execlp display" );
        exit( EXIT_FAILURE );
    }

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

    exit( EXIT_SUCCESS );

}