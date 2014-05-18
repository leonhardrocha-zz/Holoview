#ifndef _CALLABLE_H
#define _CALLABLE_H

typedef void *TrackingArgs;
typedef void (*FTCallBack)(void* lpParam, TrackingArgs args);

#ifndef NULL
#define NULL 0
#endif

class Callable
{
public:
	Callable() : m_CallBack(NULL), m_CallBackParam(NULL) {};
	void SetTrackerCallback(FTCallBack callBack, void* callBackParam, TrackingArgs callBackArgs=NULL)
	{
		m_CallBack = callBack;
		m_CallBackParam = callBackParam;
		m_CallBackArgs = callBackArgs;
	};
protected:

	FTCallBack					m_CallBack;
	void*						m_CallBackParam;
	TrackingArgs				m_CallBackArgs;

};
#endif