// Temperature.h : Temperature DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CTemperatureApp
// �йش���ʵ�ֵ���Ϣ������� Temperature.cpp
//

class CTemperatureApp : public CWinApp
{
public:
	CTemperatureApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
