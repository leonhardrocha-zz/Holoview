#ifdef _MEMORY_DEBUG
#include "vld.h"
#endif

#ifdef _MSC_VER
#pragma once
#pragma warning (disable : 4006 )
#pragma warning (disable : 4221 )
#endif

#ifndef _ZmqIPC_H_
#define _ZmqIPC_H_
#include "IArgs.h"
#include "IIPC.h"
#include "zmq.hpp"
#include <string>

class ZmqIPC : public IIPC
{
public:
    ZmqIPC(std::string msg = "msg", std::string msg_target = "msg_target", std::string msg_size = "msg_size") : m_msgArg(msg),  m_msgArgTargetId(msg_target), m_msgArgSize(msg_size) {};
    inline ~ZmqIPC() { if (m_pContext) delete m_pContext; if (m_pSocket) delete m_pSocket;};
    virtual bool    Init(IArgs* args=NULL);
    virtual bool    Start(IArgs* args=NULL);
    virtual void    Send(IArgs* args=NULL);
    virtual void*   Receive(IArgs* args=NULL);

protected:
    zmq::context_t* m_pContext;
    zmq::socket_t* m_pSocket;
    std::string m_msgArg;
    std::string m_msgArgSize;
    std::string m_msgArgTargetId;
};
#endif