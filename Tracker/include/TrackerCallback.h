#include "stdafx.h"
#ifndef _TRACKERCALLBACK_H
#define _TRACKERCALLBACK_H

#include "ICallable.h"
#include "IArgs.h"
#include <exception>


class TrackerCallback : ICallable
{
public:
    virtual void SetCallback(ICallback callback, void* instance=NULL, IArgs* args=NULL)
    {
        m_callback = callback;
        m_instance = instance;
        m_callbackArgs = args;
    };

    virtual ICallback GetCallback() { return m_callback;};
    virtual IArgs* GetArgs() { return m_callbackArgs;};
    virtual void* GetInstance() { return m_instance;};
    virtual void Call() { if (m_callback) { (*m_callback)(m_instance, m_callbackArgs); } else throw new std::exception("Callback is not set or NULL");};

protected:

    ICallback               m_callback;
    void*                   m_instance;
    IArgs*                  m_callbackArgs;

};
#endif
