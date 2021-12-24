#pragma once
#include "MyFileOperation.h"
class CLogger :public MyFileOperation
{
	static int m_nCount;
private:
	int m_nID;
	CRITICAL_SECTION  m_csLock;
	CStdioFile m_cFile;
	CString m_cstrWriteMessage;
	CString m_cstrReadMessage;
	SYSTEMTIME m_sysTime;
public:
	CString m_cstrLogName;
	CString m_cstrFilePath;
	CString m_cstrFullPath;
	bool m_bWriteLog;
	int m_nSaveTime;
	double m_nMaxSize;

public:
	CLogger(const CString & cstrName = "Log", const BOOL & bWrite = TRUE, const CString & cstrFilePath = "");
	~CLogger();
	void WriteMessage(const CString& cstrMessage);
	CString GetTimeString();
	CString GetDayString();
	void SaveParam(const BOOL & bWritePath = FALSE);
	void ReadLogFile(const CString & cstrDay, std::vector<CString> & vecLog);
	double GetRemainDiskGB();

private:
	CString GetLogSaveFileName(const CString& strSaveDir, const CString& strOriName, const CString& strNewName);
};

