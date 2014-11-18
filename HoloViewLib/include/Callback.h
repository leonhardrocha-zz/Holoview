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
#include "Synchronizable.h"
#include <exception>
#include <omp.h>


class Callback : public ICallable/*, Synchronizable*/
{
public:
    Callback() :m_callback(NULL), m_instance(NULL) { }
    Callback(ICallback callback, void* instance) :m_callback(callback), m_instance(instance) {}

    virtual void Set(ICallback callback, void* instance)
    {
        m_callback = callback;
        m_instance = (void*)instance;
    };

    virtual inline ICallback GetCallback() { return m_callback;};
    virtual inline void* GetInstance() { return m_instance;};
    virtual inline void Call() { if (m_callback) { (*m_callback)(m_instance); } else throw new std::exception("Callback is not set or NULL");};
    virtual inline void SyncCall() 
    { 
        #pragma omp critical
        Call();
    }
    
protected:
   
    ICallback         m_callback;
    void*             m_instance;

};
#endif
