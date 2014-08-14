#include "vld.h"
#ifndef _IArgs_H
#define _IArgs_H

#include <string>
#include <map>

class IArgs
{
public:
    virtual void* GetArgValue(std::string name)=0;
    virtual void AddArg(std::string name, void* value)=0;
    virtual int GetNumArgs()=0;
};

#endif