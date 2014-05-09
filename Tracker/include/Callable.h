#pragma once
#include "stdafx.h"
#include "ITracker.h"

class Callable
{
public:
	Callable() : m_CallBack(NULL), m_CallBackParam(NULL) {};
	void SetTrackerCallback(FTCallBack callBack, void* callBackParam)
	{
		m_CallBack = callBack;
		m_CallBackParam = callBackParam;
	};
protected:

	FTCallBack					m_CallBack;
	void*						m_CallBackParam;

};