#pragma once

class MyFileOperation
{
public:
	MyFileOperation();
	~MyFileOperation();
	BOOL CreateMultilevelPath(CString pstrFolder);
	CString GetPrivateProfileCString(CString lpAppName, LPCTSTR lpKeyName, CString lpDefault, LPCTSTR lpFileName);
	void WritePrivateProfileInt(CString lpAppName, CString lpKeyName, int Value, CString lpFileName);
	void WritePrivateProfileDouble(CString lpAppName, CString lpKeyName, double Value, CString lpFileName);
	double GetPrivateProfileDouble(LPCTSTR lpAppName, LPCTSTR lpKeyName, double nDefault, LPCTSTR lpFileName);
	CString GetCurrentAppPath();
	BOOL IsDirectory(CString pstrPath);
};

