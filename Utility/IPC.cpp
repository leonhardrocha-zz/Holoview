//#include "IPC.h"
//
//bool IPC::Init()
//{
//    m_pContext = new zmq::context_t(1);
//    m_pSocket = new zmq::socket_t(*m_pContext, ZMQ_REP);
//    return true;
//}
//
//bool IPC::Start()
//{
//    m_pSocket->bind ("tcp://*:5555");
//    return true;
//}
//
//void IPC_Clear(void* data, void* hint)
//{
//    if (!hint)
//    {
//        delete data;
//    }
//}
//
//void IPC::SendMessage(void *message, IArgs* args=NULL)
//{
//    std::string* targetId = static_cast<std::string*>(args->Get("id"));
//    int* size = static_cast<int*>(args->Get("msg_size"));
//    zmq::message_t msg(message, *size, IPC_Clear, message);
//
//    if (!targetId->empty())
//    {
//        m_pSocket->setsockopt(ZMQ_SUBSCRIBE, targetId, targetId->length());
//    } 
//    else
//    {
//        m_pSocket->setsockopt(ZMQ_SUBSCRIBE, NULL, 0);
//    }
//    m_pSocket->send(msg);
//}
//
//void* IPC::ReceiveMessage(IArgs* args=NULL)
//{
//    zmq::message_t request;
//    m_pSocket->recv (&request);
//    return request.data();
//}