// Marking.h : Marking DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CMarkingApp
// �йش���ʵ�ֵ���Ϣ������� Marking.cpp
//

class CMarkingApp : public CWinApp
{
public:
	CMarkingApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};
