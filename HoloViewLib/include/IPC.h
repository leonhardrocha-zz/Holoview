#ifdef _MSC_VER
#pragma once
#endif

#ifndef _IPC_H_
#define _IPC_H_
#include "IIPC.h"
#include "ZeroMQ/zmq.hpp"
#include <string>

class IPC : public IIPC
{
public:
    IPC() {};
    inline ~IPC() { if (m_pContext) delete m_pContext; if (m_pSocket) delete m_pSocket;};
    virtual bool    Init()
    {
        m_pContext = new zmq::context_t(1);
        m_pSocket = new zmq::socket_t(*m_pContext, ZMQ_REP);
        return true;
    }
    virtual bool    Start()
    {
        m_pSocket->bind ("tcp://*:5555");
        return true;
    }
    virtual void    SendMessage(void *message, IArgs* args=NULL)
    {
        std::string* targetId = static_cast<std::string*>(args->Get("id"));
        int* size = static_cast<int*>(args->Get("msg_size"));
        zmq::message_t msg(message, *size, NULL);

        if (!targetId->empty())
        {
            m_pSocket->setsockopt(ZMQ_SUBSCRIBE, targetId, targetId->length());
        } 
        else
        {
            m_pSocket->setsockopt(ZMQ_SUBSCRIBE, NULL, 0);
        }
        m_pSocket->send(msg);
    }

    virtual void*   ReceiveMessage(IArgs* args=NULL)
    {
        zmq::message_t request;
        m_pSocket->recv (&request);
        return request.data();
    }

protected:
    zmq::context_t* m_pContext;
    zmq::socket_t* m_pSocket;
};
#endif