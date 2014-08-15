#include "vld.h"
#ifndef _IARGS_H
#define _IARGS_H

#include <string>

class IArgs
{
public:
    virtual bool    Exists(std::string name)=0;
    virtual void    Set(std::string name, void* value)=0;
    virtual void    Remove(std::string name)=0;
    virtual void*   Get(std::string name)=0;
    virtual void    Add(std::string name, void* value)=0;
    virtual int     Size()=0;
};

#endif