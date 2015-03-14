/*
 * =====================================================================================
 *
 *       Filename:  Daemon.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  04/08/2013 11:50:55 AM
 *       Revision:  none
 *       Compiler:  gcc/g++
 *
 *         Author:  liyang (ly), liyangndsl@gmail.com
 *        Company:  NDSL UESTC
 *
 * =====================================================================================
 */
#include "common/daemon/Daemon.h"
#include "common/log/log.h"

Daemon::Daemon( char* cmd ): m_lockFilePath( "" )
{
    char* tmp;

    m_lockFilePath = "/var/run/";
    if ( ( tmp = strrchr( cmd, '/' ) ) == NULL )
    {
        tmp = cmd;
        m_lockFilePath += tmp;
        m_lockFilePath += ".pid";
    }
    else
    {
        ++tmp;
        m_lockFilePath += tmp;
        m_lockFilePath += ".pid";
    }
}

Daemon::~Daemon()
{
    try
    {
        m_lockFilePath.clear();
    }
    catch ( ... )
    {
    }
}

int Daemon::init()
{
    /*
     * Daemonize a process
     */
    this->daemonize();

    /*
     * Make sure only one copy of the daemon is running
     */
    if ( this->alreadyRunning() != SUCCESSFUL )
    {
        ERROR_LOG( "The daemon is already running or something error!" );
        exit( 1 );
    }

    INFO_LOG( "The daemon is running up!" );

    return SUCCESSFUL;
}

void Daemon::daemonize()
{
    int fd0, fd1, fd2;
    pid_t pid;
    struct rlimit rl;
    struct sigaction sa;

    umask( 0 );

    if ( getrlimit( RLIMIT_NOFILE, &rl ) < 0 )
    {
        ERROR_LOG( "can't get file limit!" );
    }

    if ( ( pid = fork() ) < 0 )
    {
        ERROR_LOG( "fork error!" );
    }
    else if ( pid > 0 )
    {
        exit( 0 );
    }

    /*
     * The child process will continue
     */
    pid_t rt = setsid();
    if ( rt < 0)
    {
    }

    /*
     * Ensure futrue opens won't allocate controlling TTYs
     */
    sa.sa_handler = SIG_IGN;
    int ret = sigemptyset( &sa.sa_mask );
    if ( ret < 0)
    {
    }
    sa.sa_flags = 0;
    if ( sigaction( SIGHUP, &sa, NULL ) < 0 )
    {
        ERROR_LOG( "can't ignore SIGHUP" );
    }

    if ( ( pid = fork() ) < 0 )
    {
        ERROR_LOG( "fork error!" );
    }
    else if ( pid > 0 )
    {
        exit( 0 );
    }

    /*
     * Change the current working directory to the root so
     * we won't prevent file systems from being unmounted.
     */
    if ( chdir( "/" ) < 0 )
    {
        ERROR_LOG( "can't change directory to /" );
    }

    /*
     * close all open file descriptors
     */
    if ( rl.rlim_max == RLIM_INFINITY )
    {
        rl.rlim_max = 1024;
    }
    for ( unsigned int i = 0; i < rl.rlim_max; ++i )
    {
        close( (int)i );
    }

    /*
     * Attach file descriptors 0, 1, and 2 to /dev/null
     */
    fd0 = open( "/dev/null", O_RDWR );
    fd1 = dup( 0 );
    fd2 = dup( 0 );

    if ( fd0 != 0 || fd1 != 1 || fd2 != 2 )
    {
        ERROR_LOG( "unexpected file descriptors!" );
        exit( 1 );
    }
}

int Daemon::alreadyRunning()
{
    int fd;
    char buf[16];

    fd = open( m_lockFilePath.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH );
    if ( fd < 0 )
    {
        ERROR_LOG(
            "%s opened error,%s",
            m_lockFilePath.c_str(),
            strerror( errno ) );
        return FAILED;
    }

    if ( this->lockFile( fd ) < 0 )
    {
        if ( errno == EACCES || errno == EAGAIN )
        {
            close( fd );
        }
        ERROR_LOG(
            "%s loacked error,%s",
            m_lockFilePath.c_str(),
            strerror( errno ) );
        return FAILED;
    }

    if ( ftruncate( fd, 0 ) < 0 )
    {
        ERROR_LOG( "truncate lockfile error!" );
        return FAILED;
    }

    sprintf( buf, "%ld", ( long )getpid() );

    int bufLen = ( int )strlen( buf );
    if ( ::write( fd, buf, bufLen ) != bufLen )
    {
        ERROR_LOG( "write pid to lockfile error!" );
        return FAILED;
    }

    return SUCCESSFUL;
}

int Daemon::lockFile( int fd ) const
{
    struct flock fl;
    fl.l_type = F_WRLCK;
    fl.l_start = 0;
    fl.l_whence = SEEK_SET;
    fl.l_len = 0;

    return ( fcntl( fd, F_SETLK, &fl ) );
}
