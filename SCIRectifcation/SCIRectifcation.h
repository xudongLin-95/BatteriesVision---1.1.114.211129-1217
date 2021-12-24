// SCIRectifcation.h : SCIRectifcation DLL 的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号



// CSCIRectifcationApp
// 有关此类实现的信息，请参阅 SCIRectifcation.cpp
//

class CSCIRectifcationApp : public CWinApp //CWinApp  //CWinApp
{
public:
	CSCIRectifcationApp();

// 重写
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};
extern CSCIRectifcationApp theApp;

CString GetCurrentAppPath();
BOOL CreateMultilevelPath(CString pstrFolder);
BOOL DeleteFolder(CString pstrFolder);
BOOL IsDirectory(CString pstrPath);
double GetPrivateProfileDouble(LPCTSTR lpAppName, LPCTSTR lpKeyName, double nDefault, LPCTSTR lpFileName);
CString GetPrivateProfileCString(CString lpAppName,LPCTSTR lpKeyName,CString lpDefault,LPCTSTR lpFileName);
void WritePrivateProfileInt(CString lpAppName,CString lpKeyName,int Value, CString lpFileName);
void WritePrivateProfileDouble(CString lpAppName,CString lpKeyName,double Value, CString lpFileName);