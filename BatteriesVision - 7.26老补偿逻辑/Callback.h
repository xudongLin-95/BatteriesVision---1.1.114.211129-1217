//////////////////////////////////////////////////////////////////////////
///Callback.h ���ڸ���dll����ϵͳ�ķ�������
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
	//ͨ��ϵͳ�ص�
	int CommonCall(eModuleType eType, const CString& strMessage, const CString& strSubMessage, void* wParam, void* lParam);
	//����ģ��ص�����
	int MarkingCall(const CString& strMessage, const CString& strSubMessage, void* wParam, void* lParam);
	//PLCģ��ص�����
	int SCIPLCCall(const CString& strMessage, const CString& strSubMessage, void* wParam, void* lParam);
	//�㷨ģ��ص�
	int SCIMeasureCall(const CString& strMessage, const CString& strSubMessage, void* wParam, void* lParam);
	//Mesģ��ص�
	int SCIMesCall(const CString& strMessage, const CString& strSubMessage, void* wParam, void* lParam);
};

