// SCIPLC.h : SCIPLC DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CSCIPLCApp
// �йش���ʵ�ֵ���Ϣ������� SCIPLC.cpp
//

class CSCIPLCApp : public CWinApp
{
public:
	CSCIPLCApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};
