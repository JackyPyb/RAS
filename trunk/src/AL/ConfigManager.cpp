#include "ConfigManager.h"

#include "common/xml/XMLConfig.h"
#include "common/util/util.h"

using util::conv::conv;

ConfigManager::ConfigManager():
    m_connectIP(""),
    m_connectPort(0),
    m_frameworkID(0),
    m_imageLable(""),
    m_moduleName(""),
    m_locationFilePath(""),
    m_ncIP(""),
    m_cpuNum(0.0),
    m_cpuMemSize(0),
    m_listenNum(0)
{

}

ConfigManager::~ConfigManager()
{

}

int ConfigManager::configWithXML(const char *configFileName)
{
    XMLConfig *pXML = new XMLConfig(configFileName);
    string connectIP, connectPort, frameworkID, imageLable,
           moduleName, locationFilePath, ncIP, cpuNum,
           cpuMemSize, listenNum;

    int ret;

    //connectIP
    ret = pXML->getFirstNodeValue("/CONFIG/CONNECT_IP", connectIP);
    if(ret < 0)
    {
        delete pXML;
        return ret;
    }
    m_connectIP = connectIP;

    //connectPort
    ret = pXML->getFirstNodeValue("/CONFIG/CONNECT_PORT", connectPort);
    if(ret < 0)
    {
        delete pXML;
        return ret;
    }
    m_connectPort = conv<unsigned short, string>(connectPort);

    //frameworkID
    ret = pXML->getFirstNodeValue("/CONFIG/FRAMEWORK_ID", frameworkID);
    if(ret < 0)
    {
        delete pXML;
        return ret;
    }
    m_frameworkID = conv<uint32_t, string>(frameworkID);

    //imageLable
    ret = pXML->getFirstNodeValue("/CONFIG/IMAGE_LABLE", imageLable);
    if(ret < 0)
    {
        delete pXML;
        return ret;
    }
    m_imageLable = imageLable;

    //moduleName
    ret = pXML->getFirstNodeValue("/CONFIG/MODULE_NAME", moduleName);
    if(ret < 0)
    {
        delete pXML;
        return ret;
    }
    m_moduleName = moduleName;

    //locationFilePath
    ret = pXML->getFirstNodeValue("/CONFIG/LOCATION_FILE_PATH", 
            locationFilePath);
    if(ret < 0)
    {
        delete pXML;
        return ret;
    }
    m_locationFilePath = locationFilePath;

    //ncIP
    ret = pXML->getFirstNodeValue("/CONFIG/NC_IP", ncIP);
    if(ret < 0)
    {
        delete pXML;
        return ret;
    }
    m_ncIP = ncIP;

    //cpuNum
    ret = pXML->getFirstNodeValue("/CONFIG/CPU_NUM", cpuNum);
    if(ret < 0)
    {
        delete pXML;
        return ret;
    }
    m_cpuNum = conv<double, string>(cpuNum);

    //cpuMemSize
    ret = pXML->getFirstNodeValue("/CONFIG/CPU_MEM_SIZE", cpuMemSize);
    if(ret < 0)
    {
        delete pXML;
        return ret;
    }
    m_cpuMemSize = conv<uint32_t, string>(cpuMemSize);

    //listenNum
    ret = pXML->getFirstNodeValue("/CONFIG/LISTEN_NUM", listenNum);
    if(ret < 0)
    {
        delete pXML;
        return ret;
    }
    m_listenNum = conv<uint32_t, string>(listenNum);

    delete pXML;
    return ret;
}

string ConfigManager::getConnectIP() const
{
    return m_connectIP;
}

unsigned short ConfigManager::getConnectPort() const
{
    return m_connectPort;
}

uint32_t ConfigManager::getFrameworkID() const
{
    return m_frameworkID;
}

string ConfigManager::getImageLable() const
{
    return m_imageLable;
}

string ConfigManager::getModuleName() const
{
    return m_moduleName;
}

string ConfigManager::getLocationFilePath() const
{
    return m_locationFilePath;
}

string ConfigManager::getNCIP() const
{
    return m_ncIP;
}

double ConfigManager::getCPUNum() const
{
    return m_cpuNum;
}

uint32_t ConfigManager::getCPUMemSize() const
{
    return m_cpuMemSize;
}

uint32_t ConfigManager::getListenNum() const
{
    return m_listenNum;
}
