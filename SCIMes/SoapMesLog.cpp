#include "StdAfx.h"
#include "SoapMesLog.h"

CSoapMesLog* CSoapMesLog::m_instance = NULL;
CSoapMesLog::CSoapMesLog(void):
	m_cstrAssignmentPath(_T("")),
	m_cstrUploadRecordPath(_T("")),
	m_cstrHeatBeatPath(_T("")),
	m_cstrEquipmentProductProcessPath(_T("")),
	m_cstrRecipePath(_T("")),
	m_cstrGetRecipeListPath(_T("")),
	m_cstrGetRecipePath(_T("")),
	m_cstrRecipeVersionExaminePath(_T("")),
	m_cstrCCDFileUploadCompletePath(_T("")),
	m_nSaveDay(30),
	m_nCount(0)
{
	InitializeCriticalSection(&m_csAssignment);
	InitializeCriticalSection(&m_csUploadRecord);
	InitializeCriticalSection(&m_csHeatBeat);
	InitializeCriticalSection(&m_csEquipmentProductProcess);
	InitializeCriticalSection(&m_csRecipe);
	InitializeCriticalSection(&m_csGetRecipeList);
	InitializeCriticalSection(&m_csGetRecipe);
	InitializeCriticalSection(&m_csRecipeVersionExamine);
	InitializeCriticalSection(&m_csCCDFileUploadComplete);
	Init();
}


CSoapMesLog::~CSoapMesLog(void)
{
	DeleteCriticalSection(&m_csAssignment);
	DeleteCriticalSection(&m_csUploadRecord);
	DeleteCriticalSection(&m_csHeatBeat);
	DeleteCriticalSection(&m_csEquipmentProductProcess);
	DeleteCriticalSection(&m_csRecipe);
	DeleteCriticalSection(&m_csGetRecipeList);
	DeleteCriticalSection(&m_csGetRecipe);
	DeleteCriticalSection(&m_csRecipeVersionExamine);
	DeleteCriticalSection(&m_csCCDFileUploadComplete);
	if (m_instance)
	{
		delete m_instance;
		m_instance = NULL;
	}
}
CSoapMesLog* CSoapMesLog::getInstance() 
{
	if (NULL == m_instance)
	{
		m_instance = new CSoapMesLog();
	}
	return m_instance;
}
void CSoapMesLog::SetSaveDay( const int& nDay)
{
	if (nDay <= 0 && nDay > 365)
	{
		m_nSaveDay = 30;
	}
	else
	{
		m_nSaveDay = nDay;
	}
}

void CSoapMesLog::Init()
{
	CString cstrConfigFile;
	cstrConfigFile.Format("%s/Config/SCIMES/MES.ini",GetCurrentAppPath());
	if (!PathFileExists(cstrConfigFile))
	{
		m_cstrAssignmentPath = "./Log/MesLog/Assignment/";
		m_cstrUploadRecordPath = "./Log/MesLog/FileUpLoad/";
		m_cstrHeatBeatPath = "D:/MESlog/设备心跳/";
		m_cstrEquipmentProductProcessPath  = "D:/MESlog\\设备产品过程参数采集/";
		m_cstrRecipePath  = "D:/MESlog/开机参数采集/";
		m_cstrGetRecipeListPath = "D:/MESlog\\获取开机参数列表/";
		m_cstrGetRecipePath = "D:/MESlog/获取开机参数明细/";
		m_cstrRecipeVersionExaminePath = "D:/MESlog/开机参数版本校验/";
		m_cstrCCDFileUploadCompletePath = "D:/MESlog/CCD文件上传完成/";
		m_nSaveDay = 30;
	}
	m_cstrAssignmentPath = GetPrivateProfileCString("LogPath","AssignmentLog","./Log/MesLog/Assignment/",cstrConfigFile);
	m_cstrUploadRecordPath = GetPrivateProfileCString("LogPath","UploadRecordLg","./Log/MesLog/FileUpLoad/",cstrConfigFile);
	m_cstrHeatBeatPath = GetPrivateProfileCString("LogPath","HeatBeatLog","D:/MESlog/设备心跳/",cstrConfigFile);
	m_cstrEquipmentProductProcessPath = GetPrivateProfileCString("LogPath","EquipmentProductProcessLog","D:/MESlog\\设备产品过程参数采集/",cstrConfigFile);
	m_cstrRecipePath = GetPrivateProfileCString("LogPath","RecipeLog","D:/MESlog/开机参数采集/",cstrConfigFile);
	m_cstrGetRecipeListPath = GetPrivateProfileCString("LogPath","GetRecipeListLog","D:/MESlog\\获取开机参数列表/",cstrConfigFile);
	m_cstrGetRecipePath = GetPrivateProfileCString("LogPath","GetRecipeLog","D:/MESlog/获取开机参数明细/",cstrConfigFile);
	m_cstrRecipeVersionExaminePath = GetPrivateProfileCString("LogPath","RecipeVersionExamineLog","D:/MESlog/开机参数版本校验/",cstrConfigFile);
	m_cstrCCDFileUploadCompletePath = GetPrivateProfileCString("LogPath","CCDFileUploadCompleteLog","D:/MESlog/CCD文件上传完成/",cstrConfigFile);

}

void CSoapMesLog::DeleteOldLog(const CString & cstrLogFile)
{
	CString cstrLogPath;
	std::vector<CString> vecFileList;
	cstrLogPath = cstrLogFile.Left(cstrLogFile.ReverseFind('\\'));
	if ("" == cstrLogPath )
	{
		cstrLogPath = cstrLogFile.Left(cstrLogFile.ReverseFind('/'));
	}
	GetFilePath(vecFileList,cstrLogPath);
	for (std::vector<CString>::iterator itr = vecFileList.begin(); itr != vecFileList.end(); itr++)
	{
		doDeleteFile(*itr);
	}	
}
void CSoapMesLog::doDeleteFile(const CString & cstrFile)
{
	CString strFileName;
	CString strFileExtendName;
	strFileName = cstrFile.Right(cstrFile.GetLength()-cstrFile.ReverseFind('\\')-1);
	strFileExtendName = strFileName.Right(strFileName.GetLength()- strFileName.Find('.')-1);
	try	
	{
		CString cstrYear,cstrMonth,cstrDay;
		int nYear(1900),nMonth(1),nDay(1);
		SYSTEMTIME sysTime;
		GetLocalTime(&sysTime);
		cstrYear = cstrFile.Mid(cstrFile.Find("年-")-4,4);
		cstrMonth = cstrFile.Mid(cstrFile.Find("月-")-2,2);
		cstrDay = cstrFile.Mid(cstrFile.Find("日")-2,2);
		nYear = atoi(cstrYear.GetString());
		nMonth = atoi(cstrMonth);
		nDay  = atoi(cstrDay);
		if(0 == nYear || 0 == nMonth || 0 == nDay)
		{
			return;
		}
		CTime tFile(nYear,nMonth,nDay,0,0,0);
		CTime tToday(sysTime.wYear,sysTime.wMonth,sysTime.wDay,0,0,0);
		CTimeSpan tSpan = tToday - tFile;
		if (tSpan.GetDays() >= m_nSaveDay )
		{
			DeleteFile(HTuple(cstrFile));
		}
	}
	catch(...)
	{

	}
}
void CSoapMesLog::GetFilePath(std::vector<CString>& vFilePathList, const CString &strDir)
{
	CFileFind finder;

	CString strFilePath;

	strFilePath.Format(_T("%s"),strDir);

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
	while(isNotEmpty)
	{
		isNotEmpty = finder.FindNextFile();//查找文件 
		CString filename = finder.GetFilePath();//获取文件的路径，可能是文件夹，可能是文件 
		if (!(finder.IsDirectory()))
		{	
			//如果是jpg.csv文件则加入文件列表

			CString strFileName;

			CString strFileExtendName;

			strFileName = filename.Right(filename.GetLength()-filename.ReverseFind('\\')-1);

			strFileExtendName = strFileName.Right(strFileName.GetLength()- strFileName.Find('.')-1);

			if (strFileExtendName == _T("txt") || strFileExtendName == _T("log") || strFileExtendName == _T("csv"))
			{
				vFilePathList.push_back(filename);//将一个文件路径加入容器
			}

		}
		else
		{													//递归遍历用户文件夹，跳过非用户文件夹
			if (!(finder.IsDots()||finder.IsHidden()||finder.IsSystem()||finder.IsTemporary()||finder.IsReadOnly()))
			{
				GetFilePath(vFilePathList,filename+_T("\\"));
			}
		}
	}

}


void CSoapMesLog::WriteLog( const CString& cstrMsg ,const CString& cstrLogFile)
{
	if (m_nCount++ % 1000 == 0)
	{
		m_nCount = 0;
		DeleteOldLog(cstrLogFile);
	}
 
	CStdioFile stdFile;
	CString cstrMessage(cstrMsg);
	try
	{
		if (!stdFile.Open(cstrLogFile,CFile::modeWrite|CFile::modeCreate|CFile::modeNoTruncate|CFile::shareDenyNone))
		{
			return;
		}
		stdFile.SeekToEnd();

		stdFile.Write(cstrMessage.GetBuffer(cstrMessage.GetLength()),cstrMessage.GetLength());

		cstrMessage.ReleaseBuffer();

		stdFile.Close();
	}
	catch (CException* e)
	{
		stdFile.Close();
	}
}

void CSoapMesLog::AssignmentLog( const CString& cstrMsg)
{
	CString cstrLogFile;
	CString cstrMessage;
	SYSTEMTIME systemTime;
	GetLocalTime(&systemTime);
	cstrLogFile.Format("%sAssignmentLog-%04d年-%02d月-%02d日.csv",m_cstrAssignmentPath,
		systemTime.wYear,systemTime.wMonth,systemTime.wDay);

	cstrMessage.Format("%04d/%02d/%02d	%02d:%02d:%02d.%03d	,%s\n",
		systemTime.wYear,systemTime.wMonth,systemTime.wDay,
		systemTime.wHour,systemTime.wMinute,systemTime.wSecond,systemTime.wMilliseconds,
		cstrMsg);
	if (!PathFileExists(m_cstrAssignmentPath))
	{
		CreateMultilevelPath(m_cstrAssignmentPath);
	}
	CLockHelper lockHelper(&m_csAssignment);
	WriteLog(cstrMessage,cstrLogFile);
}
void CSoapMesLog::UploadFileLog( const CString& cstrMsg)
{
	CString cstrLogFile;
	CString cstrMessage;
	SYSTEMTIME systemTime;
	GetLocalTime(&systemTime);
	cstrLogFile.Format("%sUploadFileLog-%04d年-%02d月-%02d日.csv",m_cstrUploadRecordPath,
		systemTime.wYear,systemTime.wMonth,systemTime.wDay);

	cstrMessage.Format("%04d/%02d/%02d	%02d:%02d:%02d.%03d	,%s\n",
		systemTime.wYear,systemTime.wMonth,systemTime.wDay,
		systemTime.wHour,systemTime.wMinute,systemTime.wSecond,systemTime.wMilliseconds,
		cstrMsg);
	if (!PathFileExists(m_cstrUploadRecordPath))
	{
		CreateMultilevelPath(m_cstrUploadRecordPath);
	}
	CLockHelper lockHelper(&m_csUploadRecord);
	WriteLog(cstrMessage,cstrLogFile);
}
void CSoapMesLog::HeatBeatLog( const CString& cstrMsg)
{
	CString cstrLogFile;
	CString cstrMessage;
	SYSTEMTIME systemTime;
	GetLocalTime(&systemTime);
	cstrLogFile.Format("%s设备心跳_log-%04d年-%02d月-%02d日.csv",m_cstrHeatBeatPath,
		systemTime.wYear,systemTime.wMonth,systemTime.wDay);

	cstrMessage.Format("%04d/%02d/%02d	%02d:%02d:%02d.%03d	,%s\n",
		systemTime.wYear,systemTime.wMonth,systemTime.wDay,
		systemTime.wHour,systemTime.wMinute,systemTime.wSecond,systemTime.wMilliseconds,
		cstrMsg);
	if (!PathFileExists(m_cstrHeatBeatPath))
	{
		CreateMultilevelPath(m_cstrHeatBeatPath);
	}
	CLockHelper lockHelper(&m_csHeatBeat);
	WriteLog(cstrMessage,cstrLogFile);
}
void CSoapMesLog::EquipmentProductProcessLog( const CString& cstrMsg)
{
	CString cstrLogFile;
	CString cstrMessage;
	SYSTEMTIME systemTime;
	GetLocalTime(&systemTime);
	cstrLogFile.Format("%s设备产品过程参数采集_log-%04d年-%02d月-%02d日.csv",m_cstrEquipmentProductProcessPath,
		systemTime.wYear,systemTime.wMonth,systemTime.wDay);

	cstrMessage.Format("%04d/%02d/%02d	%02d:%02d:%02d.%03d	,%s\n",
		systemTime.wYear,systemTime.wMonth,systemTime.wDay,
		systemTime.wHour,systemTime.wMinute,systemTime.wSecond,systemTime.wMilliseconds,
		cstrMsg);
	if (!PathFileExists(m_cstrEquipmentProductProcessPath))
	{
		CreateMultilevelPath(m_cstrEquipmentProductProcessPath);
	}
	CLockHelper lockHelper(&m_csEquipmentProductProcess);
	WriteLog(cstrMessage,cstrLogFile);
}
void CSoapMesLog::RecipeLog( const CString& cstrMsg)
{
	CString cstrLogFile;
	CString cstrMessage;
	SYSTEMTIME systemTime;
	GetLocalTime(&systemTime);
	cstrLogFile.Format("%s开机参数采集_log-%04d年-%02d月-%02d日.csv",m_cstrRecipePath,
		systemTime.wYear,systemTime.wMonth,systemTime.wDay);

	cstrMessage.Format("%04d/%02d/%02d	%02d:%02d:%02d.%03d	,%s\n",
		systemTime.wYear,systemTime.wMonth,systemTime.wDay,
		systemTime.wHour,systemTime.wMinute,systemTime.wSecond,systemTime.wMilliseconds,
		cstrMsg);
	if (!PathFileExists(m_cstrRecipePath))
	{
		CreateMultilevelPath(m_cstrRecipePath);
	}
	CLockHelper lockHelper(&m_csRecipe);
	WriteLog(cstrMessage,cstrLogFile);
}
void CSoapMesLog::GetRecipeListLog( const CString& cstrMsg)
{
	CString cstrLogFile;
	CString cstrMessage;
	SYSTEMTIME systemTime;
	GetLocalTime(&systemTime);
	cstrLogFile.Format("%s获取开机参数列表_log-%04d年-%02d月-%02d日.csv",m_cstrGetRecipeListPath,
		systemTime.wYear,systemTime.wMonth,systemTime.wDay);

	cstrMessage.Format("%04d/%02d/%02d	%02d:%02d:%02d.%03d	,%s\n",
		systemTime.wYear,systemTime.wMonth,systemTime.wDay,
		systemTime.wHour,systemTime.wMinute,systemTime.wSecond,systemTime.wMilliseconds,
		cstrMsg);
	if (!PathFileExists(m_cstrGetRecipeListPath))
	{
		CreateMultilevelPath(m_cstrGetRecipeListPath);
	}
	CLockHelper lockHelper(&m_csGetRecipeList);
	WriteLog(cstrMessage,cstrLogFile);
}
void CSoapMesLog::GetRecipeLog( const CString& cstrMsg)
{
	CString cstrLogFile;
	CString cstrMessage;
	SYSTEMTIME systemTime;
	GetLocalTime(&systemTime);
	cstrLogFile.Format("%s获取开机参数明细_log-%04d年-%02d月-%02d日.csv",m_cstrGetRecipePath,
		systemTime.wYear,systemTime.wMonth,systemTime.wDay);

	cstrMessage.Format("%04d/%02d/%02d	%02d:%02d:%02d.%03d	,%s\n",
		systemTime.wYear,systemTime.wMonth,systemTime.wDay,
		systemTime.wHour,systemTime.wMinute,systemTime.wSecond,systemTime.wMilliseconds,
		cstrMsg);
	if (!PathFileExists(m_cstrGetRecipePath))
	{
		CreateMultilevelPath(m_cstrGetRecipePath);
	}
	CLockHelper lockHelper(&m_csGetRecipe);
	WriteLog(cstrMessage,cstrLogFile);
}
void CSoapMesLog::RecipeVersionExamineLog( const CString& cstrMsg)
{
	CString cstrLogFile;
	CString cstrMessage;
	SYSTEMTIME systemTime;
	GetLocalTime(&systemTime);
	cstrLogFile.Format("%s开机参数版本校验_log-%04d年-%02d月-%02d日.csv",m_cstrRecipeVersionExaminePath,
		systemTime.wYear,systemTime.wMonth,systemTime.wDay);

	cstrMessage.Format("%04d/%02d/%02d	%02d:%02d:%02d.%03d	,%s\n",
		systemTime.wYear,systemTime.wMonth,systemTime.wDay,
		systemTime.wHour,systemTime.wMinute,systemTime.wSecond,systemTime.wMilliseconds,
		cstrMsg);
	if (!PathFileExists(m_cstrRecipeVersionExaminePath))
	{
		CreateMultilevelPath(m_cstrRecipeVersionExaminePath);
	}
	CLockHelper lockHelper(&m_csRecipeVersionExamine);
	WriteLog(cstrMessage,cstrLogFile);
}
void CSoapMesLog::CCDFileUploadCompleteLog( const CString& cstrMsg)
{
	CString cstrLogFile;
	CString cstrMessage;
	SYSTEMTIME systemTime;
	GetLocalTime(&systemTime);
	cstrLogFile.Format("%sCCD文件上传完成_log-%04d年-%02d月-%02d日.csv",m_cstrCCDFileUploadCompletePath,
		systemTime.wYear,systemTime.wMonth,systemTime.wDay);

	cstrMessage.Format("%04d/%02d/%02d	%02d:%02d:%02d.%03d	,%s\n",
		systemTime.wYear,systemTime.wMonth,systemTime.wDay,
		systemTime.wHour,systemTime.wMinute,systemTime.wSecond,systemTime.wMilliseconds,
		cstrMsg);
	if (!PathFileExists(m_cstrCCDFileUploadCompletePath))
	{
		CreateMultilevelPath(m_cstrCCDFileUploadCompletePath);
	}
	CLockHelper lockHelper(&m_csCCDFileUploadComplete);
	WriteLog(cstrMessage,cstrLogFile);
}