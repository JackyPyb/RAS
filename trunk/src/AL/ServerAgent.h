/*
 * =====================================================================================
 *
 *       Filename:  ServerAgent.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/05/13 04:04:57
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Heaven (), zhanwenhan@163.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#include <string>

#include "common/comm/TCPAgent.h"

using std::string;

class Epoll;
class SocketAddress;
class TCPSocket;

class ServerAgent : public TCPAgent
{
    public:
        ServerAgent();
        ServerAgent( const SocketAddress & );
        ServerAgent( const TCPSocket &, const SocketAddress & );
        ~ServerAgent();

        int init();
        void readBack( InReq & );
        void writeBack( bool );
        int connectAfter( bool );
        
        int sendPackage( MsgHeader &, const char * );

    private:
        string m_data;
        /*
         * for readBack
         */
        //MsgHeader m_MsgHeader;      //for data header
        //uint32_t  m_ReadDataLength; //data load length
        //char     *m_pDataBuffer;    //for data load
        
};
