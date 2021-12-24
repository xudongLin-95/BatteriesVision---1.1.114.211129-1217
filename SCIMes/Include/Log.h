#pragma once
#include "stdafx.h"
class CLog
{
public:
	CLog(void);
	~CLog(void);
public:
	void TimeLoging(CString strTimeLog);
	void Exceptionloging(CString strExceptionlog);
	void ReadException(CStringArray* strArrayExcepion);
public:
	BOOL m_bWriteTimeLog;
private:
	
	void Create();
	CCriticalSection m_csTime;
	CCriticalSection m_csException;
	CCriticalSection m_csRecord;

	CStdioFile m_fileException;
	CStdioFile m_fileTime;
	CStdioFile m_fileDataRecord;

	CString m_strExceptionFileName;
	CString m_strTimeFileName;
	CString m_strRecordFileName;

};

