//////////////////////////////////////////////////////////////////////////
///Callback.h 用于各个dll与主系统的反馈交互
//////////////////////////////////////////////////////////////////////////
#pragma once
#include "BaseTmpl.h"
#include "CallBack.in.h"

class CCallback
{
public:
	CCallback();
	~CCallback();

	BOOL Init();
	BOOL ReadParam();
	BOOL WriteParam();

	static int WINAPI MainCallback(eModuleType eType, const CString& strMessage, const CString& strSubMessage, void* wParam, void* lParam);

private:
	//通用系统回调
	int CommonCall(eModuleType eType, const CString& strMessage, const CString& strSubMessage, void* wParam, void* lParam);
	//喷码模块回调处理
	int MarkingCall(const CString& strMessage, const CString& strSubMessage, void* wParam, void* lParam);
	//PLC模块回调处理
	int SCIPLCCall(const CString& strMessage, const CString& strSubMessage, void* wParam, void* lParam);
	//算法模块回调
	int SCIMeasureCall(const CString& strMessage, const CString& strSubMessage, void* wParam, void* lParam);
	//Mes模块回调
	int SCIMesCall(const CString& strMessage, const CString& strSubMessage, void* wParam, void* lParam);
};

