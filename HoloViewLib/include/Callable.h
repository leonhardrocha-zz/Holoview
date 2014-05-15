#ifndef _CALLABLE_H
#define _CALLABLE_H

typedef void (*FTCallBack)(void* lpParam, void* args);

class Callable
{
public:
	Callable() : m_CallBack(0), m_CallBackParam(0) {};
	void SetTrackerCallback(FTCallBack callBack, void* callBackParam, void* callBackArgs=0)
	{
		m_CallBack = callBack;
		m_CallBackParam = callBackParam;
		m_CallBackParam = callBackArgs;
	};
protected:

	FTCallBack					m_CallBack;
	void*						m_CallBackParam;
	void*						m_CallBackArgs;

};
#endif