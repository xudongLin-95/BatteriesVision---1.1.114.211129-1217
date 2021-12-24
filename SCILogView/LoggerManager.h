#pragma once
#include "DHInterFace.h"
#include "Logger.h"
#include "LoggerManagerDlg.h"
//#include "LogViewDlg.h"
#include "resource.h"
#define		MAX_LOG_COUNT  100

class CLoggerManager :public DHInterface ,public MyFileOperation
{
private:
	int m_nLoggerCount;
	CString m_cstrConfigFile;
	std::vector<CLogger*> m_vecPLogger;
	CLogger* m_pLogger;
	BOOL m_bExit;
	BOOL m_bInit;
private:
	void Init(const CString& cstrPath = "");

public:
	CLoggerManager();
	~CLoggerManager();
	virtual int DoSomething(void* message, void* wParam, void* lParam);
	
private:
	static void ThreadDeleteOldLog(void* pVoid);
	void ReadParam();
	int WriteParam();
	int	 DisPlayLoggerManagerSet();
	void DeleteOldLog(const CString & cstrLogPath, const int nSaveDay = 30);
	void doDeleteFile(const CString & cstrFile, const int nSaveDay = 30);
	void GetFilePath(std::vector<CString>& vFilePathList, const CString &strDir);
	CLogger * AddLogger(const CString & cstrName, const CString & cstrLogFilePath = "");
	int WriteLog(const CString & cstrName, const CString & cstrMessage);
	CLogger* FindLogger(const CString & cstrName);
public:
	void ViewLog();
};

