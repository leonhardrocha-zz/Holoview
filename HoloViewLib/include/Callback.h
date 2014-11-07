#ifdef _MEMORY_DEBUG
#include "vld.h"
#endif

#ifdef _MSC_VER
#pragma once
#endif

#ifndef _CALLBACK_H
#define _CALLBACK_H

#include "ICallable.h"
#include "IArgs.h"
#include <exception>


class Callback : ICallable
{
public:
    virtual void SetCallback(ICallback callback, void* instance, IArgs* args=NULL)
    {
        m_callback = callback;
        m_instance = (void*)instance;
        m_pArgs = (IArgs*)args;
    };

    virtual inline ICallback GetCallback() { return m_callback;};
    virtual inline IArgs* GetArgs() { return m_pArgs;};
    virtual inline void* GetInstance() { return m_instance;};
    virtual inline void Call() { if (m_callback) { (*m_callback)(m_instance, m_pArgs); } else throw new std::exception("Callback is not set or NULL");};

protected:

    ICallback               m_callback;
    void*             m_instance;
    IArgs*            m_pArgs;

};
#endif
