#include "stdafx.h"
#include "LoggerManager.h"


CLoggerManager::CLoggerManager():m_bExit(FALSE), m_bInit(FALSE)
{
	m_vecPLogger.clear();
}


CLoggerManager::~CLoggerManager()
{
	m_bExit = TRUE;
	int tSize = m_vecPLogger.size();
	for (int i = tSize - 1 ; i >= 0;i++)
	{
		if (NULL != m_vecPLogger[i])
		{
			delete m_vecPLogger[i];
			m_vecPLogger[i] = NULL;
			m_vecPLogger.pop_back();
		}
	}
}

int CLoggerManager::DoSomething(void* message, void* wParam, void* lParam)
{
	CString cstrMsg = (char*)message;

	if (_T("INITIALIZE") == cstrMsg || _T("Initialize") == cstrMsg)//初始化
	{
		if (!m_bInit)
		{
			CString cstrPath = (char*)wParam;
			Init(cstrPath);
			return TRUE;
		}
	}
	else if (_T("SHOWLOG") == cstrMsg || _T("ShowLog") == cstrMsg)//设置日志
	{
		if (!m_bInit)
		{
			Init();
		}
		return DisPlayLoggerManagerSet();
	}
	else  if (_T("VIEWLOG") == cstrMsg || _T("ViewLog") == cstrMsg)//查看日志
	{
		if (!m_bInit)
		{
			Init();
		}
		ViewLog();
		return TRUE;
	}
	else if (_T("") != cstrMsg);
	{
		if (!m_bInit)
		{
			Init();
		}
		CString cstrWParam = (char*)wParam;
		return WriteLog(cstrMsg, cstrWParam);
	}
	return 1;
}

void CLoggerManager::Init(const CString &cstrPath)
{
	m_bInit = TRUE;
	ReadParam();
	AfxBeginThread((AFX_THREADPROC)ThreadDeleteOldLog, this);
}

void CLoggerManager::ReadParam()
{
	CString cstrSection, cstrKey;
	m_cstrConfigFile.Format(_T("%s\\Config\\Public\\SCILog\\Log.ini"), GetCurrentAppPath());
	cstrSection.Format(_T("LogSet"));
	cstrKey.Format(_T("nLoggerCount"));
	m_nLoggerCount = GetPrivateProfileInt(cstrSection, cstrKey, 3, m_cstrConfigFile);
	int nLogCount = min(MAX_LOG_COUNT, max(3, m_nLoggerCount));
	for (int i = 0; i < nLogCount;i++)
	{
		CString cstrName;
		BOOL bWrite;
		CString cstrLogFilePath;
		cstrSection.Format("Log%d", i);
		switch (i)
		{
			case 0:
			{
				cstrName.Format(_T("Run"));
				cstrKey.Format(_T("bWrite"));
				bWrite = GetPrivateProfileInt(cstrSection, cstrKey, 1, m_cstrConfigFile);
				cstrKey.Format(_T("LogFilePath"));
				//cstrLogFilePath = GetPrivateProfileCString(cstrSection, cstrKey, "", m_cstrConfigFile);
				cstrLogFilePath = "";
				CLogger* pLog = new CLogger(cstrName, bWrite, cstrLogFilePath);
				m_vecPLogger.push_back(pLog);
				break;
			}
			case 1:
			{
				cstrName.Format(_T("Debug"));
				cstrKey.Format(_T("bWrite"));
				bWrite = GetPrivateProfileInt(cstrSection, cstrKey, 1, m_cstrConfigFile);
				cstrKey.Format(_T("LogFilePath"));
				//cstrLogFilePath = GetPrivateProfileCString(cstrSection, cstrKey, "", m_cstrConfigFile);
				cstrLogFilePath = "";
				CLogger* pLog = new CLogger(cstrName, bWrite, cstrLogFilePath);
				m_vecPLogger.push_back(pLog);
				break;
			}
			case 2:
			{
				cstrName.Format(_T("Exception"));
				cstrKey.Format(_T("bWrite"));
				bWrite = GetPrivateProfileInt(cstrSection, cstrKey, 1, m_cstrConfigFile);
				cstrKey.Format(_T("LogFilePath"));
				//cstrLogFilePath = GetPrivateProfileCString(cstrSection, cstrKey, "", m_cstrConfigFile);
				cstrLogFilePath = "";
				CLogger* pLog = new CLogger(cstrName, bWrite, cstrLogFilePath);
				m_vecPLogger.push_back(pLog);
				break;
			}
			default:
			{
				cstrKey.Format(_T("Name"));
				cstrName = GetPrivateProfileCString(cstrSection, cstrKey, "", m_cstrConfigFile);
				if (_T("") == cstrName)
				{
					break;
				}
				cstrKey.Format(_T("bWrite"));
				bWrite = GetPrivateProfileInt(cstrSection, cstrKey, 1, m_cstrConfigFile);
				cstrKey.Format(_T("LogFilePath"));
				//cstrLogFilePath = GetPrivateProfileCString(cstrSection, cstrKey, "", m_cstrConfigFile);
				cstrLogFilePath = "";
				CLogger* pLog = new CLogger(cstrName, bWrite, cstrLogFilePath);
				m_vecPLogger.push_back(pLog);
				break;
			}
		}
	}
}

int CLoggerManager::WriteParam()
{
	CString cstrSection, cstrKey, cstrConfigFilePath;
	cstrSection.Format(_T("LogSet"));
	cstrKey.Format(_T("nLoggerCount"));
	cstrConfigFilePath.Format(_T("%s\\Config\\Public\\SCILog"), GetCurrentAppPath());
	try
	{
		if (!PathFileExists(cstrConfigFilePath))
		{
			CreateMultilevelPath(cstrConfigFilePath);
		}
		WritePrivateProfileInt(cstrSection, cstrKey, m_nLoggerCount, m_cstrConfigFile);
	}
	catch (...)
	{
	}
	return 1;
}

CLogger * CLoggerManager::AddLogger(const CString & cstrName, const CString & cstrLogFilePath )
{
	CLogger* pLog = new CLogger(cstrName, 1, cstrLogFilePath);
	m_vecPLogger.push_back(pLog);
	pLog->SaveParam(FALSE);
	m_nLoggerCount = m_vecPLogger.size();
	WriteParam();
	return pLog;
}

CLogger* CLoggerManager::FindLogger(const CString & cstrName)
{
	size_t tSize = m_vecPLogger.size();
	for (int i = 0; i < tSize ;i++)
	{
		if (cstrName == m_vecPLogger[i]->m_cstrLogName)
		{
			return m_vecPLogger[i];
		}
	}
	return NULL;
}

int CLoggerManager::WriteLog(const CString & cstrName, const CString & cstrMessage)
{
	CLogger * logger = FindLogger(cstrName);
	if (NULL == logger && m_nLoggerCount )
	{
		if (m_nLoggerCount < MAX_LOG_COUNT)
		{
			logger = AddLogger(cstrName);
		}
		else
		{
			CString cstrMsg;
			cstrMsg.Format(_T("已创建%d个日志,达到上限%d，无法创建日志[%s]"), m_nLoggerCount, MAX_LOG_COUNT, cstrName);
			WriteLog("Exception", cstrMsg);
		}
	}
	if (NULL != logger)
	{
		logger->WriteMessage(cstrMessage);
	}
	return 1;
}

int CLoggerManager::DisPlayLoggerManagerSet()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CLoggerManagerDlg* ploggerManagerDlg;
	ploggerManagerDlg = new CLoggerManagerDlg();
	ploggerManagerDlg->m_vecPLogger = m_vecPLogger;
	ploggerManagerDlg->Create(IDD_LOGGERMANAGERDLG,NULL);
	ploggerManagerDlg->ShowWindow(SW_SHOW);
	
	return TRUE;
}

void CLoggerManager::DeleteOldLog(const CString & cstrLogPath, const int nSaveDay)
{
	std::vector<CString> vecFileList;
	GetFilePath(vecFileList, cstrLogPath);
	for (std::vector<CString>::iterator itr = vecFileList.begin(); itr != vecFileList.end(); itr++)
	{
		doDeleteFile(*itr, nSaveDay);
	}
}

void CLoggerManager::doDeleteFile(const CString & cstrFile, const int nSaveDay)
{
	CString strFileName;
	CString strFileExtendName;
	strFileName = cstrFile.Right(cstrFile.GetLength() - cstrFile.ReverseFind('\\') - 1);
	strFileExtendName = strFileName.Right(strFileName.GetLength() - strFileName.Find('.') - 1);
	try
	{
		CString cstrYear, cstrMonth, cstrDay;
		int nYear(1900), nMonth(1), nDay(1);
		SYSTEMTIME sysTime;
		GetLocalTime(&sysTime);
		cstrYear = cstrFile.Mid(cstrFile.Find("Log -") + 6, 4);
		cstrMonth = cstrFile.Mid(cstrFile.Find("Log -") + 11, 2);
		cstrDay = cstrFile.Mid(cstrFile.Find("Log -") + 14, 2);
		nYear = atoi(cstrYear.GetString());
		nMonth = atoi(cstrMonth);
		nDay = atoi(cstrDay);
		if (0 == nYear || 0 == nMonth || 0 == nDay)
		{
			return;
		}
		CTime tFile(nYear, nMonth, nDay, 0, 0, 0);
		CTime tToday(sysTime.wYear, sysTime.wMonth, sysTime.wDay, 0, 0, 0);
		CTimeSpan tSpan = tToday - tFile;
		if (tSpan.GetDays() >= nSaveDay)
		{
			DeleteFile(cstrFile);
		}
	}
	catch (...)
	{

	}
}

void CLoggerManager::GetFilePath(std::vector<CString>& vFilePathList, const CString &strDir)
{
	CFileFind finder;

	CString strFilePath;

	strFilePath.Format(_T("%s"), strDir);

	if (strFilePath == _T(""))
	{
		return;
	}
	else
	{
		if (strFilePath.Right(1) != _T(""))
		{
			strFilePath += _T("\\");
		}
		strFilePath += _T("*.*");
	}

	BOOL isNotEmpty = finder.FindFile(strFilePath);//总文件夹，开始遍历 
	while (isNotEmpty)
	{
		isNotEmpty = finder.FindNextFile();//查找文件 
		CString filename = finder.GetFilePath();//获取文件的路径，可能是文件夹，可能是文件 
		if (!(finder.IsDirectory()))
		{
			//如果是jpg.csv文件则加入文件列表

			CString strFileName;

			CString strFileExtendName;

			strFileName = filename.Right(filename.GetLength() - filename.ReverseFind('\\') - 1);

			strFileExtendName = strFileName.Right(strFileName.GetLength() - strFileName.ReverseFind('.') - 1);

			if (strFileExtendName == _T("txt") || strFileExtendName == _T("log") || strFileExtendName == _T("csv"))
			{
				vFilePathList.push_back(filename);//将一个文件路径加入容器
			}

		}
		else
		{													//递归遍历用户文件夹，跳过非用户文件夹
			if (!(finder.IsDots() || finder.IsHidden() || finder.IsSystem() || finder.IsTemporary() || finder.IsReadOnly()))
			{
				GetFilePath(vFilePathList, filename + _T("\\"));
			}
		}
	}
}

void CLoggerManager::ThreadDeleteOldLog(void* pVoid)
{
	CLoggerManager* pLoggerManager = (CLoggerManager*)pVoid;
	int nSize = 0;
	while (!pLoggerManager->m_bExit)
	{
		nSize = pLoggerManager->m_vecPLogger.size();
		for (int i = 0; i < nSize ; i++)
		{
			try
			{
				pLoggerManager->DeleteOldLog(pLoggerManager->m_vecPLogger[i]->m_cstrFilePath, pLoggerManager->m_vecPLogger[i]->m_nSaveTime);
			}
			catch (...)
			{
			}
		}
		Sleep(1000 * 60 * 60 * 2);
	}
}


void CLoggerManager::ViewLog()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CLogViewDlg* plogViewDlg;
	plogViewDlg = new CLogViewDlg();
	plogViewDlg->m_vecLogger = m_vecPLogger;
	plogViewDlg->Create(IDD_LOGVIEWDLG, NULL);
	plogViewDlg->ShowWindow(SW_SHOW);
}
