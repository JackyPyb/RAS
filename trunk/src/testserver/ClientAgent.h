/*
 * =====================================================================================
 *
 *       Filename:  ClientAgent.h
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

#include "common/comm/TCPAgent.h"

class ClientAgent : public TCPAgent
{
    public:
        ClientAgent();
        ClientAgent(const SocketAddress &addrl);
        ClientAgent(const TCPSocket &sock, const SocketAddress &addr);
        ~ClientAgent();

        void readBack(InReq & );
        void writeBack( bool );
        int sendPackage(
		  	MsgHeader &header, const char *dataStr = NULL);

    private:
        /*
         * for readBack
         */
        //MsgHeader m_MsgHeader;      //for data header
        //uint32_t  m_ReadDataLength; //data load length
        //char     *m_pDataBuffer;    //for data load
};
