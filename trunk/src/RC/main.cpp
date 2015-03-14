#include "ConfigManager.h"
#include "RunControl.h"

#include "common/comm/TaskManager.h"
#include "common/log/log.h"
#include "common/comm/Epoll.h"
#include "common/Timer/Timer.h"
#include "common/comm/Error.h"

#include <stdlib.h>
#include <signal.h>
#include <iostream>
#include <unistd.h>

using namespace rc;

const char * const shortOpt = "hf:";
TimerManager *g_pTimerManager = NULL;
Epoll *g_pEpoll = NULL;

class GeneralIDGenerator
{
    public:
        GeneralIDGenerator():m_ID(0){}
        uint64_t operator()(void)
        {
            uint64_t msec = time(NULL) << 32;
            ++m_ID;
            return (msec | m_ID);
        }
    private:
        uint64_t m_ID;
};

void sig_internal(int sigNo);

int main(int argc, char *argv[])
{
    INFO_LOG("JFALJJJJJJJJJJJJJJJJJJ");
    if(argc != 3)
    {
        std::cout << "Usage: " << argv[0] << " -f config_filename" << std::endl;
        exit(FAILED);
    }

    if(SIG_ERR == (signal(SIGINT, sig_internal)))
    {
        ERROR_LOG("Can not catch SIGINT");
        return FAILED;
    }

    int ch = 0;
    const char *pConfigFileName = NULL;
    while((ch = getopt(argc, argv, shortOpt)) != -1)
    {
        switch(ch)
        {
            case 'h':
                std::cout << "Usage: " << argv[0] << " -f config_filename" << std::endl;
                return SUCCESSFUL;
            case 'f':
                pConfigFileName = optarg;
                break;
            default:
                std::cerr << "Unknown option: " << char(optopt) << std::endl;
                return FAILED;
        }
    }

    if(NULL == pConfigFileName)
        pConfigFileName = "RASRC_config.xml";

    if(FAILED == (ConfigManager::getInstance())->configWithXML(pConfigFileName))
    {
        ERROR_LOG("RC : Config init ERROR");
        return FAILED;
    }

    g_pTimerManager = new TimerManager();
    if(g_pTimerManager == NULL)
    {
        ERROR_LOG("main: can not init TimerManager");
        return FAILED;
    }

    TaskManager::getInstance()->setIDGenerator(GeneralIDGenerator());

    (RunControl::getInstance())->run();

    (RunControl::getInstance())->epollRun();

    return 0;
}

void sig_internal(int sigNo)
{
    INFO_LOG("Catch SIGINT");
    exit(FAILED);
}
