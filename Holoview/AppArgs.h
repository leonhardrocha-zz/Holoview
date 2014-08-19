#include "qtLib.h"
#ifndef _APPARGS_H
#define _APPARGS_H
#include "IArgs.h"

class AppArgs : public IArgs
{
public:
    virtual bool    Exists(std::string name) { return argMap.find(name) != argMap.end(); }
    virtual void    Set(std::string name, void* value) { argMap[name] = value; };
    virtual void*   Get(std::string name) { if (Exists(name)) { return argMap.at(name); } else throw new std::exception("Invalid arg name.");};
    virtual void    Add(std::string name, void* value) { if (Exists(name)) { argMap[name] = value; } else throw new std::exception("Arg name already exists."); };
    virtual void    Remove(std::string name) { if (Exists(name)) { argMap.erase(name); } else throw new std::exception("Arg name does not exists."); };
    virtual int     Size() { return argMap.size();};
protected:
    std::map<std::string, void*> argMap;
};
#endif