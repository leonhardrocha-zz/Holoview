#include "ZmqIPC.h"
#include <string>

bool    ZmqIPC::Init(IArgs* args)
{
    m_pContext = new zmq::context_t(1);
    m_pSocket = new zmq::socket_t(*m_pContext, ZMQ_REP);
    return true;
}
bool    ZmqIPC::Start(IArgs* args)
{
    m_pSocket->bind ("tcp://*:5555");
    return true;
}
void    ZmqIPC::Send(IArgs* args)
{
    std::string* targetId = static_cast<std::string*>(args->Get(m_msgArgTargetId));
    int* size = static_cast<int*>(args->Get(m_msgArgSize));
    void* message = static_cast<void*>(args->Get(m_msgArg));
    zmq::message_t msg((void*)message, *size, NULL);

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

void*   ZmqIPC::Receive(IArgs* args)
{
    zmq::message_t request;
    m_pSocket->recv (&request);
    return request.data();
}