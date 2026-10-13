#include <stdio.h>
#include <string.h>

int main() 
{
    char l_buf[ 1024 ];
    while ( fgets( l_buf, sizeof( l_buf ), stdin ) != NULL )
    {
        l_buf[ strcspn( l_buf, "\n" ) ] = '\0';
        for ( int i = strlen( l_buf ) - 1; i >= 0; i-- )
        {
            putchar( l_buf[ i ] );
        }
        putchar( '\n' );
    }

    return 0;
}