// CrashRecord.h : CrashRecord DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CCrashRecordApp
// �йش���ʵ�ֵ���Ϣ������� CrashRecord.cpp
//

class CCrashRecordApp : public CWinApp
{
public:
	CCrashRecordApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};
