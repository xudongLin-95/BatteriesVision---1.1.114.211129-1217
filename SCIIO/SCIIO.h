// SCICommunicate.h : SCICommunicate DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CSCIIOApp
// �йش���ʵ�ֵ���Ϣ������� SCICommunicate.cpp
//

class CSCIIOApp : public CWinApp
{
public:
	CSCIIOApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};
