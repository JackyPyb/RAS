#ifndef _AL_CONFIGMANAGER_H_
#define _AL_CONFIGMANAGER_H_

#include "common/sys/Singleton.h"

#include <string>
#include <stdint.h>

using std::string;

class ConfigManager : public Singleton<ConfigManager>
{
public:
    friend class Singleton<ConfigManager>;

    ConfigManager();
    ~ConfigManager();

    int configWithXML(const char*);

    string getConnectIP() const;
    unsigned short getConnectPort() const;
    uint32_t getFrameworkID() const;
    string getImageLable() const;
    string getModuleName() const;
    string getLocationFilePath() const;
    string getNCIP() const;
    double getCPUNum() const;
    uint32_t getCPUMemSize() const;
    uint32_t getListenNum() const;

private:
    string m_connectIP;
    unsigned short m_connectPort;
    uint32_t m_frameworkID;
    string m_imageLable;
    string m_moduleName;
    string m_locationFilePath;
    string m_ncIP;
    double m_cpuNum;
    uint32_t m_cpuMemSize;
    uint32_t m_listenNum;
};

#endif
