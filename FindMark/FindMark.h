// FindMark.h : FindMark DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CFindMarkApp
// �йش���ʵ�ֵ���Ϣ������� FindMark.cpp
//

class CFindMarkApp : public CWinApp
{
public:
	CFindMarkApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
