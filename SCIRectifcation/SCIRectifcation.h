// SCIRectifcation.h : SCIRectifcation DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������



// CSCIRectifcationApp
// �йش���ʵ�ֵ���Ϣ������� SCIRectifcation.cpp
//

class CSCIRectifcationApp : public CWinApp //CWinApp  //CWinApp
{
public:
	CSCIRectifcationApp();

// ��д
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