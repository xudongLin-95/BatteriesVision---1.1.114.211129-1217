
// BatteriesVision.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号


// CBatteriesVisionApp:
// 有关此类的实现，请参阅 BatteriesVision.cpp
//

class CBatteriesVisionApp : public CBCGPWinApp
{
public:
	CBatteriesVisionApp();

// 重写
public:
	virtual BOOL InitInstance();
	void SetAppTheme(int nTheme);
// 实现
	ULONG_PTR m_gdiplusToken;//为了使用GDI+
	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};

extern CBatteriesVisionApp theApp;