#include "ConfigManager.h"

#include "common/comm/BaseHeader.h"
#include "common/xml/XMLConfig.h"
#include "common/util/util.h"

using util::conv::conv;

namespace rc
{

ConfigManager::ConfigManager():
    m_epollMaxFd(0),
    m_NCHeartBeatTimeOut(0),
    m_NCHeartBeatRetryNum(0),
    m_FWMRootHeartBeatTimeOut(0),
    m_FWMRootHeartBeatRetryNum(0),
    m_minNCNum(0)
{

}

ConfigManager::~ConfigManager()
{

}

int ConfigManager::configWithXML(const char *configFileName)
{
    XMLConfig *pXML = new XMLConfig(configFileName);
    string listenNCPort, listenSASPort, listenFWMPort, listenALPort, 
           epollMaxFd, NCHeartBeatTimeOut, NCHeartBeatRetryNum, 
           FWMRootHeartBeatTimeOut, FWMRootHeartBeatRetryNum, minNCNum;
    int ret;

    //listenNCPort
    ret = pXML->getFirstNodeValue(
            "/CONFIG/MODULE_CONFIG/MUTABLE/LISTEN_PORT/RASNC_RASRC_PORT", 
            listenNCPort);
    if(ret < 0)
    {
        delete pXML;
        return ret;
    }
    m_NCListenAddr.setAddress(
            IPV4ANYADDR, conv<unsigned short, string>(listenNCPort));

    //listenSASPort
    ret = pXML->getFirstNodeValue(
            "/CONFIG/MODULE_CONFIG/MUTABLE/LISTEN_PORT/RASSAS_RASRC_PORT", 
            listenSASPort);
    if(ret < 0)
    {
        delete pXML;
        return ret;
    }
    m_SASListenAddr.setAddress(
            IPV4ANYADDR, conv<unsigned short, string>(listenSASPort));

    //listenFWMPort
    ret = pXML->getFirstNodeValue(
            "/CONFIG/MODULE_CONFIG/MUTABLE/LISTEN_PORT/RASFWM_RASRC_PORT", 
            listenFWMPort);
    if(ret < 0)
    {
        delete pXML;
        return ret;
    }
    m_FWMListenAddr.setAddress(
            IPV4ANYADDR, conv<unsigned short, string>(listenFWMPort));

    //listenALPort
    ret = pXML->getFirstNodeValue(
            "/CONFIG/MODULE_CONFIG/MUTABLE/LISTEN_PORT/RASAL_RASRC_PORT", 
            listenALPort);
    if(ret < 0)
    {
        delete pXML;
        return ret;
    }
    m_ALListenAddr.setAddress(
            IPV4ANYADDR, conv<unsigned short, string>(listenALPort));

    //epollMaxFd
    ret = pXML->getFirstNodeValue(
            "/CONFIG/MODULE_CONFIG/UNMUTABLE/Sys/EpollFD", epollMaxFd);
    if(ret < 0)
    {
        delete pXML;
        return ret;
    }
    m_epollMaxFd = conv<unsigned int, string>(epollMaxFd);

    //NCHeartBeatTimeOut
    ret = pXML->getFirstNodeValue(
            "/CONFIG/MODULE_CONFIG/UNMUTABLE/TimeOut/NCHeartBeatTimeOut", 
            NCHeartBeatTimeOut);
    if(ret < 0)
    {
        delete pXML;
        return ret;
    }
    m_NCHeartBeatTimeOut = conv<unsigned int, string>(NCHeartBeatTimeOut);

    //NCHeartBeatRetryNum
    ret = pXML->getFirstNodeValue(
            "/CONFIG/MODULE_CONFIG/UNMUTABLE/TimeOut/NCHeartBeatRetryNum", 
            NCHeartBeatRetryNum);
    if(ret < 0)
    {
        delete pXML;
        return ret;
    }
    m_NCHeartBeatRetryNum = conv<unsigned int, string>(NCHeartBeatRetryNum);

    //FWMRootHeartBeatTimeOut
    ret = pXML->getFirstNodeValue(
            "/CONFIG/MODULE_CONFIG/UNMUTABLE/TimeOut/FWMRootHeartBeatTimeOut", 
            FWMRootHeartBeatTimeOut);
    if(ret < 0)
    {
        delete pXML;
        return ret;
    }
    m_FWMRootHeartBeatTimeOut = conv<unsigned int, string>(
            FWMRootHeartBeatTimeOut);

    //FWMRootHeartBeatRetryNum
    ret = pXML->getFirstNodeValue(
            "/CONFIG/MODULE_CONFIG/UNMUTABLE/TimeOut/FWMRootHeartBeatRetryNum", 
            FWMRootHeartBeatRetryNum);
    if(ret < 0)
    {
        delete pXML;
        return ret;
    }
    m_FWMRootHeartBeatRetryNum = conv<unsigned int, string>(
            FWMRootHeartBeatRetryNum);

    //minNCNum
    ret = pXML->getFirstNodeValue(
            "/CONFIG/MODULE_CONFIG/MUTABLE/MIN_NC_NUM", minNCNum);
    if(ret < 0)
    {
        delete pXML;
        return ret;
    }
    m_minNCNum = conv<unsigned int, string>(minNCNum);

    delete pXML;
    return ret;
}

unsigned int ConfigManager::getEpollMaxFd() const
{
    return m_epollMaxFd;
}

SocketAddress ConfigManager::getNCListenAddr() const
{
    return m_NCListenAddr;
}

SocketAddress ConfigManager::getSASListenAddr() const
{
    return m_SASListenAddr;
}

SocketAddress ConfigManager::getFWMListenAddr() const
{
    return m_FWMListenAddr;
}

SocketAddress ConfigManager::getALListenAddr() const
{
    return m_ALListenAddr;
}

unsigned int ConfigManager::getNCHeartBeatTimeOut() const
{
    return m_NCHeartBeatTimeOut;
}

unsigned int ConfigManager::getNCHeartBeatRetryNum() const
{
    return m_NCHeartBeatRetryNum;
}

unsigned int ConfigManager::getFWMRootHeartBeatTimeOut() const
{
    return m_FWMRootHeartBeatTimeOut;
}

unsigned int ConfigManager::getFWMRootHeartBeatRetryNum() const
{
    return m_FWMRootHeartBeatRetryNum;
}

unsigned int ConfigManager::getMinNCNum() const
{
    return m_minNCNum;
}

}
