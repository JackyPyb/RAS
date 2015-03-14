/*
 * =====================================================================================
 *
 *       Filename:  Daemon.h
 *
 *    Description:  To daemonize a regular process
 *
 *        Version:  1.0
 *        Created:  04/08/2013 10:40:45 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  liyang (ly), liyangndsl@gmail.com
 *        Company:  NDSL UESTC
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <syslog.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <string>

#include "common/comm/Error.h"

using std::string;

class Daemon
{
public:
    Daemon( char* cmd );
    ~Daemon();

    /*
     * public interface to make process be daemon
     */
    int init( void );

private:
    void daemonize( void );    //daemonize a regular process
    int  alreadyRunning( void ); //ensure only one copy is running
    int  lockFile( int fd ) const;   //lock the pid file

    string m_lockFilePath;       //store the path of pid file
};
