// PowerLimit.h : PowerLimit DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CPowerLimitApp
// �йش���ʵ�ֵ���Ϣ������� PowerLimit.cpp
//

class CPowerLimitApp : public CWinApp
{
public:
	CPowerLimitApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
