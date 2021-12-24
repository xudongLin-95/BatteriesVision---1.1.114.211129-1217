#include "stdafx.h"
#include "Logger.h"

int CLogger::m_nCount = 0;

CLogger::CLogger(const CString & cstrName, const BOOL & bWrite, const CString & cstrFilePath)
	:m_cstrLogName(cstrName), m_bWriteLog(bWrite), m_cstrFilePath(cstrFilePath), m_nID(m_nCount++),
	m_cstrWriteMessage(_T("")), m_cstrReadMessage(_T("")), m_nSaveTime(30), m_nMaxSize(5.0f)
{
	InitializeCriticalSection(&m_csLock);
	/*if (_T("") == cstrFilePath)
	{
		m_cstrFilePath.Format(_T(".\\Log\\%s"), cstrName);
	}
	else*/
	{
		m_cstrFilePath.Format(_T("%s\\Log\\%s"), GetCurrentAppPath(), cstrName);
	}
}

CLogger::~CLogger()
{
	m_nCount--;
	DeleteCriticalSection(&m_csLock);
}

CString CLogger::GetLogSaveFileName(const CString& strSaveDir, const CString& strOriName, const CString& strNewName)
{
	CString strFilePath, strFileName = strNewName;
	strFilePath.Format("%s//%s.csv", strSaveDir, strFileName);

	if (PathFileExists(strFilePath))
	{
		CFileStatus fileStatus;
		CFile::GetStatus(strFilePath, fileStatus);
		if (fileStatus.m_size > 3 * 1024 * 1024)
		{
			int nPos = strFileName.Find(strOriName);
			CString strRemain = strFileName.Mid(nPos + strOriName.GetLength());
			if (strRemain.Left(1) == "_")
				strRemain = strRemain.Right(strRemain.GetLength() - 1);

			int nSN = atoi(strRemain);
			strFileName.Format("%s_%d", strOriName, ++nSN);
			return GetLogSaveFileName(strSaveDir, strOriName, strFileName);
		}
		else
		{
			return strFileName;
		}
	}
	else
	{
		return strFileName;
	}

	return GetLogSaveFileName(strSaveDir, strOriName, strFileName);
}

void CLogger::WriteMessage(const CString& cstrMessage)
{
	if (!m_bWriteLog)
	{
		return;
	}
	//磁盘空间小于102.4m，不写入日志
	if (GetRemainDiskGB() <= 0.1) return;
	if (!PathFileExists(m_cstrFilePath))
	{
		CreateMultilevelPath(m_cstrFilePath);
	}

	//m_cstrFullPath.Format(_T("%s\\%sLog -%s.csv"), m_cstrFilePath, m_cstrLogName, GetDayString());
	CString strFileName;
	strFileName.Format("%sLog - %s", m_cstrLogName, GetDayString());
	m_cstrFullPath.Format(_T("%s\\%s.csv"), m_cstrFilePath, GetLogSaveFileName(m_cstrFilePath, strFileName, strFileName));
	m_cstrWriteMessage.Format(_T("%s,%s\n"), GetTimeString(), cstrMessage);

	try
	{
		CLockHelper lk(&m_csLock);
		CFileException fileException;
		BOOL bRet = m_cFile.Open(m_cstrFullPath, CFile::modeWrite | CFile::modeCreate | CFile::modeNoTruncate | CFile::shareDenyNone, &fileException);
		if (!bRet)
		{
			return;
		}
		m_cFile.SeekToEnd();
		m_cFile.Write(m_cstrWriteMessage.GetBuffer(m_cstrWriteMessage.GetLength()), m_cstrWriteMessage.GetLength());
		m_cstrWriteMessage.ReleaseBuffer();
		m_cFile.Close();
	}
	catch (...)
	{
		m_cFile.Close();
	}
}

void CLogger::ReadLogFile(const CString & cstrDay, std::vector<CString> & vecLog)
{
	CString cstrLogFile, cstrFileFind;
	cstrFileFind.Format(_T("%s\\%sLog - %s*.csv"), m_cstrFilePath, m_cstrLogName, cstrDay);

	CStdioFile tFile;
	try
	{
		CLockHelper lk(&m_csLock);
		CFileException fileException;
		BOOL bRet = tFile.Open(cstrLogFile, CFile::modeRead | CFile::shareDenyNone, &fileException);
		if (!bRet)
		{
			return;
		}
		CString cstrRead;
		vecLog.clear();
		int nCount = 0;
		while (tFile.ReadString(cstrRead) && nCount < 10000)
		{
			nCount++;
			vecLog.push_back(cstrRead);
		}
		tFile.Close();
	}
	catch (...)
	{
		tFile.Close();
	}
}

CString CLogger::GetTimeString()
{
	CString cstrTime;
	GetLocalTime(&m_sysTime);
	cstrTime.Format(_T("%4d/%02d/%02d %02d:%02d:%02d.%03d"), m_sysTime.wYear, m_sysTime.wMonth, m_sysTime.wDay,
		m_sysTime.wHour, m_sysTime.wMinute, m_sysTime.wSecond, m_sysTime.wMilliseconds);
	return cstrTime;
}

CString CLogger::GetDayString()
{
	CString cstrTime;
	GetLocalTime(&m_sysTime);
	cstrTime.Format(_T("%4d-%02d-%02d"), m_sysTime.wYear, m_sysTime.wMonth, m_sysTime.wDay);
	return cstrTime;
}

void CLogger::SaveParam(const BOOL & bWritePath)
{
	try
	{
		CString cstrConfigFilePath, cstrConfigFile, cstrSection, cstrKey;
		cstrConfigFilePath.Format(_T("%s\\Config\\Public\\SCILog"), GetCurrentAppPath());
		if (!PathFileExists(cstrConfigFilePath))
		{
			CreateMultilevelPath(cstrConfigFilePath);
		}
		cstrConfigFile.Format(_T("%s\\Config\\Public\\SCILog\\Log.ini"), GetCurrentAppPath());
		cstrSection.Format(_T("Log%d"), m_nID);
		cstrKey.Format(_T("Name"));
		WritePrivateProfileString(cstrSection, cstrKey, m_cstrLogName, cstrConfigFile);
		cstrKey.Format(_T("bWrite"));
		WritePrivateProfileInt(cstrSection, cstrKey, m_bWriteLog, cstrConfigFile);
		cstrKey.Format(_T("nSaveTime"));
		WritePrivateProfileInt(cstrSection, cstrKey, m_nSaveTime, cstrConfigFile);
		if (bWritePath)
		{
			cstrKey.Format(_T("LogFilePath"));
			WritePrivateProfileString(cstrSection, cstrKey, m_cstrFilePath, cstrConfigFile);
		}
	}
	catch (...)
	{
	}
}

double CLogger::GetRemainDiskGB()
{
	DWORD dwTotalDiskSpace, dwFreeDiskSpace, dwUsedDiskSpace;
	ULARGE_INTEGER uiFreeBytesAvailableToCaller;
	ULARGE_INTEGER uiTotalNumberOfBytes;
	ULARGE_INTEGER uiTotalNumberOfFreeBytes;
	CString strPath = GetCurrentAppPath();
	int pos = strPath.Find('\\');
	strPath = strPath.Left(pos);
	CString strFreeDiskSpace;
	if (GetDiskFreeSpaceEx(strPath, &uiFreeBytesAvailableToCaller,
		&uiTotalNumberOfBytes,
		&uiTotalNumberOfFreeBytes))
	{
		dwTotalDiskSpace = (DWORD)(uiTotalNumberOfBytes.QuadPart / 1024 / 1024);
		dwFreeDiskSpace = (DWORD)(uiFreeBytesAvailableToCaller.QuadPart >> 20);
		dwUsedDiskSpace = dwTotalDiskSpace - dwFreeDiskSpace;
		float fds = (float)dwFreeDiskSpace / 1024;

		return fds;
	}
	else
	{
		return 9999;
	}
}
