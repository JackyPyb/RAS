/*
 * =====================================================================================
 *
 *       Filename:  ServerAgent.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/05/13 04:19:46
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Heaven (), zhanwenhan@163.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#include "ServerAgent.h"
#include "protocol/AlProtocol.pb.h"
#include "protocol/RASCmdCode.h"
#include "ConfigManager.h"

#include "common/comm/Epoll.h"
#include "common/comm/SocketAddress.h"
#include "common/log/log.h"

#include <iostream>
#include <stdint.h>
#include <stdlib.h>

ServerAgent::ServerAgent()
{

}

ServerAgent::ServerAgent(const SocketAddress &addr):
    TCPAgent(addr)
{

}

ServerAgent::ServerAgent(const TCPSocket &sock, const SocketAddress &addr):
    TCPAgent(sock,addr)
{

}

ServerAgent::~ServerAgent()
{
}

int
ServerAgent::init()
{
    if ( this->connect() != SUCCESSFUL )
    {
        ERROR_LOG("in ServerAgent::init connect server error");
        return FAILED;
    }

    return SUCCESSFUL;
}

int
ServerAgent::connectAfter( bool bConnect)
{
    if (bConnect )
    {
        AlProto::StartRootModule startRootInfo; 
        startRootInfo.set_framework_id(
                (ConfigManager::getInstance())->getFrameworkID());
        startRootInfo.set_image_lable(
                (ConfigManager::getInstance())->getImageLable());
        startRootInfo.set_module_name(
                (ConfigManager::getInstance())->getModuleName());
        startRootInfo.set_location_file_path(
                (ConfigManager::getInstance())->getLocationFilePath());
        startRootInfo.set_nc_ip(
                (ConfigManager::getInstance())->getNCIP());
        
        AlProto::ResourceInfo *resInfo = 
            startRootInfo.mutable_request_resource_size();
        resInfo->set_cpu_num(
                (ConfigManager::getInstance())->getCPUNum());
        resInfo->set_cpu_mem_size(
                (ConfigManager::getInstance())->getCPUMemSize());
        
        startRootInfo.set_listen_num(
                (ConfigManager::getInstance())->getListenNum());

        startRootInfo.SerializeToString(&m_data);
        MsgHeader msg;
        msg.cmd = MSG_AL_RC_START_ROOT_MODULE;
        msg.length = m_data.length();

        sendPackage(msg, m_data.c_str());        
        
        INFO_LOG("ServerAgent::connectAfter: sendPackage");

        return SUCCESSFUL;
    }
    else
    {
        return FAILED;
    }
}
void
ServerAgent::readBack(InReq &req)
{
    switch (req.m_msgHeader.cmd)
    {
        case MSG_AL_RC_START_ROOT_MODULE_ACK:
        {
            string data(req.ioBuf, req.m_msgHeader.length);
            AlProto::FrameworkInstanceInfo ackInfo;
            ackInfo.ParseFromString(data);
            std::cout << "Framework Instance ID is " 
                << ackInfo.framework_instance_id() << std::endl;
            exit(0);
            break;
        }
        default:
            break;
    }
}

void
ServerAgent::writeBack(bool result)
{
    if (!result)
    {
        ERROR_LOG("\nIn ServerAgent: "
                "writeBack(): write out error!\n");
    }
}

int
ServerAgent::sendPackage(MsgHeader &header, const char *dataStr)
{
    uint32_t msgLength = header.length + sizeof(MsgHeader);
    char *sendBuf = new char[msgLength + 1];
    memset(sendBuf, 0, msgLength + 1);
    memcpy(sendBuf, &header, sizeof(MsgHeader));
    if (dataStr != NULL)
    {
        memcpy(sendBuf + sizeof(MsgHeader), dataStr, header.length);
    }

    if (this->writeToBuffer(sendBuf, msgLength) == FAILED)
    {
        DEBUG_LOG("\nIn ServerAgent: "
                "sendPackage(): write data error!\n");
        return FAILED;
    }

    return SUCCESSFUL;
}
