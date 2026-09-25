#include <cstdio>
#include <cstdlib>
#include <sys/stat.h>
#include <unistd.h>
#include <ctime>
#include <fcntl.h>

int main( int argc, char **argv )
{
    if ( argc != 2 )
    {
        fprintf( stderr, "Usage: %s <FILE>\n", argv[ 0 ] );
        exit( EXIT_FAILURE );
    }

    int fd = open( argv[1], O_RDONLY );
    if ( fd < 0 ) { perror( argv[1] ); exit( EXIT_FAILURE ); }

    off_t l_pos = 0;

    char l_time[ 32 ];

    while ( true )
    {
        struct stat st;
        if ( fstat( fd, &st ) < 0 )
        {
            perror( "fstat" );
            exit( EXIT_FAILURE );
        }

        if ( st.st_size != l_pos )
        {
            time_t l_now = time( NULL );
            strftime( l_time, sizeof l_time, "%Y-%m-%d %H:%M:%S", localtime( &l_now ) );
        }

        if ( st.st_size < l_pos )
        {
            fprintf( stderr, "[%s] size: %ld -> %ld (%+ld)\n", l_time, ( long ) l_pos, ( long ) st.st_size, ( long ) ( st.st_size - l_pos ) );
            lseek( fd, 0, SEEK_SET );
            l_pos = 0;
        }

        if ( st.st_size > l_pos )
        {
            char l_buf[ 4096 ];
            ssize_t l_read;
            fprintf( stderr, "[%s] size: %ld -> %ld (%+ld)\n", l_time, ( long ) l_pos, ( long ) st.st_size, ( long ) ( st.st_size - l_pos ) );
            while ( ( l_read = read( fd, l_buf, sizeof l_buf ) ) > 0 )
            {
                write( 1, l_buf, l_read );
                l_pos += l_read;
            }
        }

        sleep( 1 );
    }

    return 0;
}