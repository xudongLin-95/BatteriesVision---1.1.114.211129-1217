
// BatteriesVision.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CBatteriesVisionApp:
// �йش����ʵ�֣������ BatteriesVision.cpp
//

class CBatteriesVisionApp : public CBCGPWinApp
{
public:
	CBatteriesVisionApp();

// ��д
public:
	virtual BOOL InitInstance();
	void SetAppTheme(int nTheme);
// ʵ��
	ULONG_PTR m_gdiplusToken;//Ϊ��ʹ��GDI+
	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};

extern CBatteriesVisionApp theApp;