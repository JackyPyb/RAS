#ifndef _RC_RESOURCE_H_
#define _RC_RESOURCE_H_

#include <map>
#include <string>
#include <stdint.h>

using std::map;
using std::string;

namespace rc
{

struct Resource
{
    double logicCPUNum;
    uint32_t cpuMemSize;
    map<string, uint32_t> GPUInfo;

    typedef map<string, uint32_t>::iterator GPUInfoIter;
    typedef map<string, uint32_t>::const_iterator GPUInfoConstIter;

    Resource()
    {
        logicCPUNum = 0.0;
        cpuMemSize = 0;
        GPUInfo.clear();
    }

    ~Resource()
    {
        GPUInfo.clear();
    }

    bool isDoubleEqual(double lhs, double rhs) const
    {
        if((lhs - rhs > -0.0000001) &&
                (lhs - rhs) < 0.0000001)
            return true;
        else
            return false;
    }

    Resource operator += (const Resource &rhs)
    {
        this->logicCPUNum += rhs.logicCPUNum;
        this->cpuMemSize += rhs.cpuMemSize;

        GPUInfoConstIter rhsGPUIter = rhs.GPUInfo.begin();
        for(; rhsGPUIter != rhs.GPUInfo.end(); rhsGPUIter++)
        {
            string GPUName = rhsGPUIter->first;
            GPUInfoIter resGPUIter = this->GPUInfo.find(GPUName);
            if(resGPUIter == this->GPUInfo.end())
            {
                this->GPUInfo[GPUName] = rhsGPUIter->second;
            }
            else
            {
                this->GPUInfo[GPUName] += rhsGPUIter->second;
            }
        }
        return *this;
    }

    Resource operator -= (const Resource &rhs)
    {
        this->logicCPUNum -= rhs.logicCPUNum;
        this->cpuMemSize -= rhs.cpuMemSize;

        GPUInfoConstIter rhsGPUIter = rhs.GPUInfo.begin();
        for(; rhsGPUIter != rhs.GPUInfo.end(); rhsGPUIter++)
        {
            string GPUName = rhsGPUIter->first;
            this->GPUInfo[GPUName] -= rhsGPUIter->second;
        }

        return *this;
    }

    Resource operator = (const Resource &rhs)
    {
        this->logicCPUNum = rhs.logicCPUNum;
        this->cpuMemSize = rhs.cpuMemSize;
        this->GPUInfo = rhs.GPUInfo;
        return *this;
    }

    Resource operator + (const Resource &rhs)
    {
        Resource res;
        res = *this;
        res += rhs;
        return res;
    }

    Resource operator - (const Resource &rhs)
    {
        Resource res;
        res = *this;
        res -= rhs;
        return res;
    }

    bool operator >= (const Resource &rhs) const
    {
        bool ret = false;
        if(this->logicCPUNum < rhs.logicCPUNum)
            return ret;

        if(this->cpuMemSize < rhs.cpuMemSize)
            return ret;

        GPUInfoConstIter rhsGPUIter = rhs.GPUInfo.begin();
        for(; rhsGPUIter != rhs.GPUInfo.end(); rhsGPUIter++)
        {
            string GPUName = rhsGPUIter->first;
            GPUInfoConstIter resGPUIter = this->GPUInfo.find(GPUName);
            if(resGPUIter == this->GPUInfo.end())
            {
                return ret;
            }
            else
            {
                if(resGPUIter->second < rhsGPUIter->second)
                    return ret;
            }
        }

        ret = true;
        return ret;
    }

    bool operator < (const Resource &rhs) const
    {
        bool ret = false;
        if(this->logicCPUNum >= rhs.logicCPUNum)
            return ret;
        
        if(this->cpuMemSize >= rhs.cpuMemSize)
            return ret;

        GPUInfoConstIter resGPUIter = this->GPUInfo.begin();
        for(; resGPUIter != this->GPUInfo.end(); resGPUIter++)
        {
            string GPUName = resGPUIter->first;
            GPUInfoConstIter rhsGPUIter = rhs.GPUInfo.find(GPUName);
            if(rhsGPUIter == rhs.GPUInfo.end())
            {
                return ret;
            }
            else
            {
                if(resGPUIter->second >= rhsGPUIter->second)
                    return ret;
            }
        }

        ret = true;
        return ret;
    }

    bool operator == (const Resource &rhs) const
    {
        bool ret = false;
        if(!this->isDoubleEqual(this->logicCPUNum, rhs.logicCPUNum))
            return ret;

        if(this->cpuMemSize != rhs.cpuMemSize)
            return ret;

        if(this->GPUInfo.size() != rhs.GPUInfo.size())
            return ret;

        GPUInfoConstIter rhsGPUIter = rhs.GPUInfo.begin();
        for(; rhsGPUIter != rhs.GPUInfo.end(); rhsGPUIter++)
        {
            string GPUName = rhsGPUIter->first;
            GPUInfoConstIter resGPUIter = this->GPUInfo.find(GPUName);
            if(resGPUIter == this->GPUInfo.end())
            {
                return ret;
            }
            else
            {
                if(resGPUIter->second != rhsGPUIter->second)
                    return ret;
            }
        }

        ret = true;
        return ret;
    }

    bool isResZero() const
    {
        if((!this->isDoubleEqual(this->logicCPUNum, 0.0)) || (this->cpuMemSize != 0))
            return false;

        if(this->GPUInfo.size() == 0)
        {
            return true;
        }
        else
        {
            GPUInfoConstIter gpuInfoIter = (this->GPUInfo).begin();
            for(; gpuInfoIter != GPUInfo.end(); gpuInfoIter++)
            {
                if(gpuInfoIter->second != 0)
                {
                    return false;
                }
            }

            return true;
        }
    }
};

}

#endif
