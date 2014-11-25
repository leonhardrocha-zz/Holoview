#ifdef _MEMORY_DEBUG
#include "vld.h"
#endif

#ifdef _MSC_VER
#pragma once
#endif

#ifndef _ARGS_H
#define _ARGS_H

#include "IArgs.h"
#include <string>
#include <map>
#include <exception>

class Args : public IArgs
{
public:
    virtual inline bool    Exists(std::string name) { return argMap.find(name) != argMap.end(); }
    virtual inline void    Set(std::string name, void* value) { argMap[name] = value; };
    virtual inline void*   Get(std::string name) { if (Exists(name)) { return argMap.at(name); } else throw new std::exception("Invalid arg name.");};
    virtual inline void    Add(std::string name, void* value) { if (Exists(name)) { argMap[name] = value; } else throw new std::exception("Arg name already exists."); };
    virtual inline void    Remove(std::string name) { if (Exists(name)) { argMap.erase(name); } else throw new std::exception("Arg name does not exists."); };
    virtual inline int     Size() { return argMap.size();};
protected:
    std::map<std::string, void*> argMap;
};

#endif