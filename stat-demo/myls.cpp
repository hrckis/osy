// ***************************************************************************
//
// Program example for subject Operating Systems
//
// Petr Olivka, Dept. of Computer Science, petr.olivka@vsb.cz, 2026
//
// The example of using stat* functions. 
//
// ***************************************************************************

#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <cstdarg>
#include <vector>
#include <sys/param.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctime>
#include <string>

// ***************************************************************************
// log messages

#define LOG_ERROR               0       // errors
#define LOG_INFO                1       // information and notifications
#define LOG_DEBUG               2       // debug messages

// debug flag
int g_debug = LOG_INFO;

void log_msg( int t_log_level, const char *t_form, ... )
{
    const char *out_fmt[] = {
            "ERR: (%d-%s) %s\n",
            "INF: %s\n",
            "DEB: %s\n" };

    if ( t_log_level && t_log_level > g_debug ) 
    {
        return;
    }

    char l_buf[ 4096 ];
    va_list l_arg;
    va_start( l_arg, t_form );

    vsnprintf( l_buf, sizeof( l_buf ), t_form, l_arg );

    va_end( l_arg );

    switch ( t_log_level )
    {
        case LOG_INFO:
        case LOG_DEBUG:
            fprintf( stdout, out_fmt[ t_log_level ], l_buf );
            break;

        case LOG_ERROR:
            fprintf( stderr, out_fmt[ t_log_level ], errno, strerror( errno ), l_buf );
            break;
    }
}

// ***************************************************************************
// help

const char g_help[] = 
    "\n"
    "  My own ls.\n"
    "\n"
    "  Use: %s [-h] [-d] [-s] [-t] [-r] soubor...\n"
    "\n"
    "    -h  this help\n"
    "    -d  debug mode\n"
    "    -s  file size\n"
    "    -t  modification time\n"
    "    -r  access rights\n"
    "\n"
    ;

// ***************************************************************************

struct file_info_t 
{
    char m_file_name[ MAXPATHLEN ];
    struct stat m_file_stat;
};

void format_rights( mode_t t_mode, char *t_out )
{
    if      ( S_ISDIR ( t_mode ) ) t_out[ 0 ] = 'd';
    else if ( S_ISLNK ( t_mode ) ) t_out[ 0 ] = 'l';
    else if ( S_ISCHR ( t_mode ) ) t_out[ 0 ] = 'c';
    else if ( S_ISBLK ( t_mode ) ) t_out[ 0 ] = 'b';
    else if ( S_ISFIFO( t_mode ) ) t_out[ 0 ] = 'p';
    else if ( S_ISSOCK( t_mode ) ) t_out[ 0 ] = 's';
    else                           t_out[ 0 ] = '-';

    t_out[ 1 ] = ( t_mode & S_IRUSR ) ? 'r' : '-';
    t_out[ 2 ] = ( t_mode & S_IWUSR ) ? 'w' : '-';
    t_out[ 3 ] = ( t_mode & S_IXUSR ) ? 'x' : '-';
    t_out[ 4 ] = ( t_mode & S_IRGRP ) ? 'r' : '-';
    t_out[ 5 ] = ( t_mode & S_IWGRP ) ? 'w' : '-';
    t_out[ 6 ] = ( t_mode & S_IXGRP ) ? 'x' : '-';
    t_out[ 7 ] = ( t_mode & S_IROTH ) ? 'r' : '-';
    t_out[ 8 ] = ( t_mode & S_IWOTH ) ? 'w' : '-';
    t_out[ 9 ] = ( t_mode & S_IXOTH ) ? 'x' : '-';
    t_out[ 10 ] = '\0';
}

int main( int t_argc, char **t_argv )
{
    std::vector< file_info_t > l_file_list;
    std::vector< char > l_columns;


    for ( int inx = 1; inx < t_argc; inx++ )
    {
        if ( strcmp( t_argv[ inx ], "-h" ) == 0 )
        {
            printf( g_help, t_argv[ 0 ] );
            exit( EXIT_SUCCESS );
        }
        if ( strcmp( t_argv[ inx ], "-s" ) == 0 )
        {
            l_columns.push_back( 's' );
        }
        if ( strcmp( t_argv[ inx ], "-t" ) == 0 )
        {
            l_columns.push_back( 't' );
        }
        if ( strcmp( t_argv[ inx ], "-r" ) == 0 )
        {
            l_columns.push_back( 'r' );
        }

        if ( *t_argv[ inx ] != '-' )
        {
            // add next file name into l_file_list
            file_info_t l_finfo;
            memset( &l_finfo, 0, sizeof( l_finfo ) );
            strcpy( l_finfo.m_file_name, t_argv[ inx ] );
            l_file_list.push_back( l_finfo );
        }
    }

    // file_list empty?
    if ( l_file_list.size() == 0 )
    {
        log_msg( LOG_INFO, "No file specified!" );
        exit( EXIT_FAILURE );
    }

    std::vector< std::string > l_missing;

    // get stat info for all file names
    for ( auto finfo = l_file_list.begin(); finfo != l_file_list.end(); )
    {
        if ( stat( finfo->m_file_name, &finfo->m_file_stat ) < 0 )
        {
            l_missing.push_back( finfo->m_file_name );
            finfo = l_file_list.erase( finfo );
        }
        else
        {
            finfo++;
        }
    }

     if ( l_missing.size() > 0 )
    {
        printf( "No such files:\n" );
        for ( size_t inx = 0; inx < l_missing.size(); inx++ )
        {
            printf( "  %s\n", l_missing[ inx ].c_str() );
        }
    }

    for ( size_t inx = 0; inx < l_file_list.size(); inx++ )
    {
        auto &finfo = l_file_list[ inx ];

        for ( size_t c = 0; c < l_columns.size(); c++ )
        {
            switch ( l_columns[ c ] )
            {
                case 's':
                    printf( "%12ld", ( long ) finfo.m_file_stat.st_size );
                    break;
                case 't':
                    // printf( "%s", ctime( &finfo.m_file_stat.st_mtime ) );
                {
                    char l_time[ 32 ];
                    snprintf( l_time, sizeof l_time, "%s", ctime( &finfo.m_file_stat.st_mtime ) );
                    l_time[ strcspn( l_time, "\n" ) ] = '\0';
                    printf( "  %s", l_time );
                    break;
                }
                case 'r':
                {
                    char l_rights[ 10 ];
                    format_rights( finfo.m_file_stat.st_mode, l_rights );
                    printf( "  %s", l_rights );
                    break;
                }
            }
        }

        // printf( "%16lu  %s\n", finfo.m_file_stat.st_size, finfo.m_file_name );
        printf( "  %s\n", finfo.m_file_name );
    }
     
    return EXIT_SUCCESS;
} // main

