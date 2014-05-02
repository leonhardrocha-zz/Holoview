#ifdef FACTORYSDLL
#ifdef FACTORYSDLL_EXPORTS
#define FACTORYDLL_API __declspec(dllexport) 
#else
#define FACTORYDLL_API __declspec(dllimport) 
#endif
#else
#define FACTORYDLL_API 
#endif


#pragma once
#include "ITrackerFactory.h"

class IKinectFactory
{
public:
	virtual FACTORYDLL_API ITrackerFactory *GetTrackerFactory()=0;
};
