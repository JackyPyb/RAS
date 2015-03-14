#include "common/comm/UDPWriter.h"
#include "common/comm/BufferManager.h"
#include "common/log/log.h"

#include <stdint.h>

int
UDPWriter::doWrite( list<Iov>* pSendIovList )
{
    for ( list<Iov>::iterator it = pSendIovList->begin();
            it != pSendIovList->end(); ++it )
    {
        int rt = doWriteInside(
                 ( char* )( it->m_Iovec.iov_base ),
                 it->m_Iovec.iov_len,
                 it->m_UDPAddr );
        // error ocurred
        if ( rt < 0 )
        {
            ERROR_LOG(
                "In UDPWriter::doWrite, doWriteInternal() error" );
            return FAILED;
        }
        // 标记已发送的数据块
        uint32_t writen = ( uint32_t )rt;
        it->m_Iovec.iov_base =
            static_cast<char*>( it->m_Iovec.iov_base ) + writen;
        it->m_Iovec.iov_len -= static_cast<size_t>( writen );
    }

    return SUCCESSFUL;
}

