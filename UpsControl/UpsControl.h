// UpsControl.h : UpsControl DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CUpsControlApp
// �йش���ʵ�ֵ���Ϣ������� UpsControl.cpp
//

class CUpsControlApp : public CWinApp
{
public:
	CUpsControlApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
