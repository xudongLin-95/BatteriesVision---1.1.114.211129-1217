// SCILogView.h : SCILogView DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CSCILogViewApp
// �йش���ʵ�ֵ���Ϣ������� SCILogView.cpp
//

class CSCILogViewApp : public CWinApp
{
public:
	CSCILogViewApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
