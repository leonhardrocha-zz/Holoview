#include "vld.h"
#ifndef _ARGSMAP_H
#define _ARGSMAP_H
#include "IArgs.h"

class ArgsMap : public IArgs
{
public:
    virtual void* GetArgValue(std::string name) { return argMap[name]; };
    virtual void AddArg(std::string name, void* value) { argMap.insert(std::pair<std::string, void*>(name, value)); };
    virtual int GetNumArgs() { return argMap.size();};
protected:
    std::map<std::string, void*> argMap;
};
#endif