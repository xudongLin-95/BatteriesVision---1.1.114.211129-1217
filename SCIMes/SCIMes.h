// SCIMes.h : SCIMes DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CSCIMesApp
// �йش���ʵ�ֵ���Ϣ������� SCIMes.cpp
//

class CSCIMesApp : public CWinApp
{
public:
	CSCIMesApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
