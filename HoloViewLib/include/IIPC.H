#ifndef _IIPC_H
#define _IIPC_H
#include "IArgs.h"

class IIPC
{
public:
    virtual bool    Init()=0;
    virtual bool    Start()=0;
    virtual void    SendMessage(void *message, IArgs* args=NULL)=0;
    virtual void*   ReceiveMessage(IArgs* args=NULL)=0;
};

#endif