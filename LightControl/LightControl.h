// LightControl.h : LightControl DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CLightControlApp
// �йش���ʵ�ֵ���Ϣ������� LightControl.cpp
//

class CLightControlApp : public CWinApp//, public CBCGPWorkspace
{
public:
	CLightControlApp();

// ��д
public:
	virtual BOOL InitInstance();
	//virtual int ExitInstance();
	DECLARE_MESSAGE_MAP()
};
