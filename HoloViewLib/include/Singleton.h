#ifdef _MEMORY_DEBUG
#include "vld.h"
#endif

#ifdef _MSC_VER
#pragma once
#endif

#ifndef SINGLETON_H
#define SINGLETON_H


template <class T>
class Singleton
{
public:
    static T& Instance()
    {
        static T _instance; // create static instance of our class
        return _instance;   // return it
    }
 
private:
    Singleton();	// hide constructor
    ~Singleton();	// hide destructor
    Singleton(const Singleton &); // hide copy constructor
    Singleton& operator=(const Singleton &); // hide assign op
};
#endif