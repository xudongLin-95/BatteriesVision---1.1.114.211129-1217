// SCISeparate.h : SCISeparate DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CSCISeparateApp
// �йش���ʵ�ֵ���Ϣ������� SCISeparate.cpp
//

void CPPExpDefaultExceptionHandler(const HException& except);
class CSCISeparateApp : public CWinApp
{
public:
	CSCISeparateApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};
