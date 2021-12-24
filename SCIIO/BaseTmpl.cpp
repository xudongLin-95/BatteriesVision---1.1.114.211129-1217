#include "StdAfx.h"
#include "BaseTmpl.h"

CRITICAL_SECTION CBaseTmpl::m_csSystemFileLock;
CRITICAL_SECTION CBaseTmpl::m_csCommonFileLock;

CBaseTmpl::CBaseTmpl()
{
	InitializeCriticalSection(&m_csLock);
	InitializeCriticalSection(&m_csSystemFileLock);
	InitializeCriticalSection(&m_csCommonFileLock);
	
	//验证配置文件
	m_strSystemFilePath.Format("%s/Config/System.vis",GetCurrentAppPath());
	if (INVALID_FILE_ATTRIBUTES == GetFileAttributes(m_strSystemFilePath))
	{
		CString strMessageBox;
		strMessageBox.Format("%s\r\nFile not exists, program exits.\r\n系统配置文件未找到，程序将终止",m_strSystemFilePath);
		BCGPMessageBox(strMessageBox);
		exit(1);
	}
	SetFileAttributes(m_strSystemFilePath,FILE_ATTRIBUTE_NORMAL);
	m_strCommonFilePath.Format("%s/Config/Common.dh",GetCurrentAppPath());
	if (INVALID_FILE_ATTRIBUTES == GetFileAttributes(m_strCommonFilePath))
	{
		CString strMessageBox;
		strMessageBox.Format("%s\r\nFile not exists, program exits.\r\n公共配置文件未找到，程序将终止",m_strCommonFilePath);
		BCGPMessageBox(strMessageBox);
		exit(1);
	}
	SetFileAttributes(m_strCommonFilePath,FILE_ATTRIBUTE_NORMAL);
}

CBaseTmpl::~CBaseTmpl()
{

}

void CBaseTmpl::DeleteIniSection(CString lpAppName,eIniType type)
{
	CRITICAL_SECTION* pLk;
	CString strFilePath;
	if (SystemFile==type)
	{
		pLk = &m_csSystemFileLock;
		strFilePath = m_strSystemFilePath;
	}
	else
	{
		pLk = &m_csCommonFileLock;
		strFilePath = m_strCommonFilePath;
	}
	SetFileAttributes(strFilePath,FILE_ATTRIBUTE_NORMAL);
	CLockHelper lk(pLk);
	WritePrivateProfileString(lpAppName,NULL,NULL,strFilePath);
}

void CBaseTmpl::DeleteIniKey(CString lpAppName,CString lpKeyName,eIniType type)
{
	CRITICAL_SECTION* pLk;
	CString strFilePath;
	if (SystemFile==type)
	{
		pLk = &m_csSystemFileLock;
		strFilePath = m_strSystemFilePath;
	}
	else
	{
		pLk = &m_csCommonFileLock;
		strFilePath = m_strCommonFilePath;
	}
	SetFileAttributes(strFilePath,FILE_ATTRIBUTE_NORMAL);
	CLockHelper lk(pLk);
	WritePrivateProfileString(lpAppName,lpKeyName,NULL,strFilePath);
}

void CBaseTmpl::WriteIniString(CString lpAppName,CString lpKeyName,const CString& Value, eIniType type)
{
	CRITICAL_SECTION* pLk;
	CString strFilePath;
	if (SystemFile==type)
	{
		pLk = &m_csSystemFileLock;
		strFilePath = m_strSystemFilePath;
	}
	else
	{
		pLk = &m_csCommonFileLock;
		strFilePath = m_strCommonFilePath;
	}
	SetFileAttributes(strFilePath,FILE_ATTRIBUTE_NORMAL);
	CLockHelper lk(pLk);
	WritePrivateProfileString(lpAppName,lpKeyName,Value,strFilePath);
}

void CBaseTmpl::WriteIniInt(CString lpAppName,CString lpKeyName,const int& Value, eIniType type)
{
	CRITICAL_SECTION* pLk;
	CString strFilePath;
	if (SystemFile==type)
	{
		pLk = &m_csSystemFileLock;
		strFilePath = m_strSystemFilePath;
	}
	else
	{
		pLk = &m_csCommonFileLock;
		strFilePath = m_strCommonFilePath;
	}
	SetFileAttributes(strFilePath,FILE_ATTRIBUTE_NORMAL);
	CLockHelper lk(pLk);
	WritePrivateProfileInt(lpAppName,lpKeyName,Value,strFilePath);
}

void CBaseTmpl::WriteIniDouble(CString lpAppName,CString lpKeyName,const double& Value, eIniType type)
{
	CRITICAL_SECTION* pLk;
	CString strFilePath;
	if (SystemFile==type)
	{
		pLk = &m_csSystemFileLock;
		strFilePath = m_strSystemFilePath;
	}
	else
	{
		pLk = &m_csCommonFileLock;
		strFilePath = m_strCommonFilePath;
	}
	SetFileAttributes(strFilePath,FILE_ATTRIBUTE_NORMAL);
	CLockHelper lk(pLk);
	WritePrivateProfileDouble(lpAppName,lpKeyName,Value,strFilePath);
}

CString CBaseTmpl::GetIniString(CString lpAppName,CString lpKeyName,CString lpDefault, eIniType type)
{
	CString strFilePath;
	if (SystemFile==type)
		strFilePath = m_strSystemFilePath;
	else
		strFilePath = m_strCommonFilePath;

	return GetPrivateProfileCString(lpAppName,lpKeyName,lpDefault,strFilePath);
}

int CBaseTmpl::GetIniInt(CString lpAppName,CString lpKeyName,int nDefault, eIniType type)
{
	CString strFilePath;
	if (SystemFile==type)
		strFilePath = m_strSystemFilePath;
	else
		strFilePath = m_strCommonFilePath;

	return GetPrivateProfileInt(lpAppName,lpKeyName,nDefault,strFilePath);
}

double CBaseTmpl::GetIniDouble(CString lpAppName,CString lpKeyName,double dDefault, eIniType type)
{
	CString strFilePath;
	if (SystemFile==type)
		strFilePath = m_strSystemFilePath;
	else
		strFilePath = m_strCommonFilePath;

	return GetPrivateProfileDouble(lpAppName,lpKeyName,dDefault,strFilePath);
}
