#ifndef _RC_CONFIGMANAGER_H_
#define _RC_CONFIGMANAGER_H_

#include "common/sys/Singleton.h"
#include "common/comm/SocketAddress.h"

#include <string>

using std::string;

namespace rc
{

class ConfigManager : public Singleton<ConfigManager>
{
public:
    friend class Singleton<ConfigManager>;
    int configWithXML(const char *);

    unsigned int getEpollMaxFd() const;
    SocketAddress getNCListenAddr() const;
    SocketAddress getSASListenAddr() const;
    SocketAddress getFWMListenAddr() const;
    SocketAddress getALListenAddr() const;
    unsigned int getNCHeartBeatTimeOut() const;
    unsigned int getNCHeartBeatRetryNum() const;
    unsigned int getFWMRootHeartBeatTimeOut() const;
    unsigned int getFWMRootHeartBeatRetryNum() const;
    unsigned int getMinNCNum() const;
    string getLocalIP() const;

private:
    ConfigManager();
    ~ConfigManager();

    unsigned int m_epollMaxFd;
    SocketAddress m_NCListenAddr;
    SocketAddress m_SASListenAddr;
    SocketAddress m_FWMListenAddr;
    SocketAddress m_ALListenAddr;
    string m_localIP;
    unsigned int m_NCHeartBeatTimeOut;
    unsigned int m_NCHeartBeatRetryNum;
    unsigned int m_FWMRootHeartBeatTimeOut;
    unsigned int m_FWMRootHeartBeatRetryNum;
    unsigned int m_minNCNum;

};    

}

#endif
