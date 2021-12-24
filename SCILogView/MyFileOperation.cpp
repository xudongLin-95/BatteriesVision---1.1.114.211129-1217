#include "stdafx.h"
#include "MyFileOperation.h"

MyFileOperation::MyFileOperation()
{
}

MyFileOperation::~MyFileOperation()
{
}

BOOL MyFileOperation::CreateMultilevelPath(CString pstrFolder) //p是路径可以是相对路径或是绝对路径
{
	char *p = pstrFolder.GetBuffer(pstrFolder.GetLength());
	pstrFolder.ReleaseBuffer();
	size_t i, len;
	len = strlen(p);
	for (i = 0; i < len; i++)
	{
		if (p[i] == '\\' || p[i] == '/')
		{
			p[i] = '\0';
			_mkdir(p);
			p[i] = '\\';
		}
	}
	if (len > 0)
		_mkdir(p);
	p = NULL;
	return TRUE;

}

CString MyFileOperation::GetPrivateProfileCString(CString lpAppName, LPCTSTR lpKeyName, CString lpDefault, LPCTSTR lpFileName)
{
	CString strRead;
	GetPrivateProfileString(lpAppName, lpKeyName, lpDefault, strRead.GetBuffer(MAX_PATH), MAX_PATH, lpFileName);
	strRead.ReleaseBuffer();
	return strRead;
}

void MyFileOperation::WritePrivateProfileInt(CString lpAppName, CString lpKeyName, int Value, CString lpFileName)
{
	CString strIn;
	strIn.Format("%d", Value);
	WritePrivateProfileString(lpAppName, lpKeyName, strIn, lpFileName);
}

void MyFileOperation::WritePrivateProfileDouble(CString lpAppName, CString lpKeyName, double Value, CString lpFileName)
{
	CString strIn;
	strIn.Format("%f", Value);
	WritePrivateProfileString(lpAppName, lpKeyName, strIn, lpFileName);
}

double MyFileOperation::GetPrivateProfileDouble(LPCTSTR lpAppName, LPCTSTR lpKeyName, double nDefault, LPCTSTR lpFileName)
{
	CString strValue;
	GetPrivateProfileString(lpAppName, lpKeyName, "DefaultValue", strValue.GetBuffer(MAX_PATH), MAX_PATH, lpFileName);
	strValue.ReleaseBuffer();
	if (strValue == "DefaultValue")
	{
		return nDefault;
	}
	else
	{
		try
		{
			double dValue = atof(strValue);
			return dValue;
		}
		catch (...)
		{
			return nDefault;
		}

	}
}

CString MyFileOperation::GetCurrentAppPath()
{
	CString path;
	GetModuleFileName(NULL, path.GetBufferSetLength(MAX_PATH + 1), MAX_PATH);
	path.ReleaseBuffer();
	int pos = path.ReverseFind('\\');
	path = path.Left(pos);
	return path;
}

BOOL MyFileOperation::IsDirectory(CString pstrPath)
{
	if ("" == pstrPath)
	{
		return FALSE;
	}
	/*去除路径末尾的反斜杠*/
	CString strPath = pstrPath;
	if (strPath.Right(1) == _T('\\'))
	{
		strPath.Delete(strPath.GetLength() - 1);
	}

	CFileFind finder;
	BOOL bRet = finder.FindFile(strPath);
	if (!bRet)
	{
		return FALSE;
	}

	/*判断该路径是否是目录*/
	finder.FindNextFile();
	bRet = finder.IsDirectory();
	finder.Close();
	return bRet;
}