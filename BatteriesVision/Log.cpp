#include "StdAfx.h"
#include "Log.h"

CLog::CLog()
{
	m_vecDispLogList.clear();
	m_nListLength = 0;
	InitializeCriticalSection(&m_csLogPathLock);
}

CLog::~CLog()
{

}

BOOL CLog::Init()
{
	ReadParam();

	m_pLog = LoadDHInterFaceDll("SCILogView");
	CString strPath;
	strPath.Format("%s\\Log\\", GetCurrentAppPath());
	m_pLog->DoSomething("Initialize", (void*)(LPCTSTR)strPath, NULL);

	return TRUE;
}

BOOL CLog::ReadParam()
{
	m_nLogLevel		= GetIniInt("Log","Level",1);
	m_nListLength	= GetIniInt("Log","ListLength",30);
	m_nListDispLevel	= GetIniInt("Log","DispLevel",3);
	m_nSingleLogFileSize = GetIniInt("Log","SingleLogFileSizeMB",2);
	return TRUE;
}

BOOL CLog::WriteParam()
{
	WriteIniInt("Log","Level",m_nLogLevel);
	WriteIniInt("Log","ListLength",m_nListLength);
	WriteIniInt("Log","DispLevel",m_nListDispLevel);
	WriteIniInt("Log","SingleLogFileSizeMB",m_nSingleLogFileSize);
	return TRUE;
}

BOOL CLog::Log(const CString &strLog,LogType emType)
{
	if ((int)emType>m_nLogLevel)
		return FALSE;
	CString strPath;
	strPath.Format("%s\\Log\\RunLog",GetCurrentAppPath());
	
	if (!PathFileExists(strPath)) 
		CreateMultilevelPath(strPath);
	
	SYSTEMTIME tTime;
	GetLocalTime(&tTime);
	CString strLogName,strLogInfo,strTip,logFile;

	if (StdLog==emType)
		strTip = "Std";
	else if (ExcpLog==emType)
		strTip = "Exception";
	else if (AlarmLog == emType)
		strTip = "Alarm";
	else if (DebugLog==emType)
		strTip = "Debug";
	else if (CommunicationLog == emType)
		strTip = "Communication";
	else
		strTip = "Operation";
	
	strLogName.Format("%d-%d-%d",tTime.wYear,tTime.wMonth,tTime.wDay);
	logFile.Format("%s\\Log\\RunLog\\%s.log",GetCurrentAppPath(),GetLogSaveFileName(strPath,strLogName,strLogName));
	
	strLogInfo.Format("[%02d:%02d:%02d:%03dms] [%s] %s",
		tTime.wHour,tTime.wMinute,tTime.wSecond,tTime.wMilliseconds,strTip,strLog);
	//strLogInfo.Format("[%s] %s", strTip, strLog);

	CLockHelper lk(&m_csLock);
	if (emType<=m_nListDispLevel)
	{
		tagLogInfo inf;
		inf.type = emType;
		inf.strLog = strLogInfo;
		m_vecDispLogList.push_back(inf);
	}
	if (m_vecDispLogList.size() > m_nListLength && m_vecDispLogList.size() > 0)
		m_vecDispLogList.erase(m_vecDispLogList.begin());

	if (m_pLog != NULL)
		m_pLog->DoSomething((void*)(LPCTSTR)strTip, (void*)(LPCTSTR)strLogInfo, NULL);
	
	/*CStdioFile fl;
	fl.Open(logFile,CFile::modeReadWrite| CFile::typeBinary|CFile::shareDenyNone|CFile::modeCreate|CFile::modeNoTruncate);
	fl.SeekToEnd();
	fl.Write(strLogInfo.GetBuffer(strLogInfo.GetLength()),strLogInfo.GetLength());
	strLogInfo.ReleaseBuffer();
	fl.Close();*/

	return TRUE;
}

BOOL CLog::Log(const CString &strLog,const int& nType)
{
	return Log(strLog,(LogType)nType);
}

BOOL CLog::Std(const CString &strLog)
{
	return Log(strLog,StdLog);
}

BOOL CLog::Exception(const CString &strLog)
{
	return Log(strLog,ExcpLog);
}

BOOL CLog::Debug(const CString &strLog)
{
	return Log(strLog,DebugLog);
}

BOOL CLog::Alarm(CErrorInfo::tagErrorInfo tInfo)
{
	//显示异常，报警， 或停机
	theGlobal.SetAlarmInfo(tInfo);

	CString strLog;
	strLog.Format("异常报警: 错误码:%d 描述%s 解决方案:%s", tInfo.nID, tInfo.strDescribe, tInfo.strSolution);
	return Log(strLog, AlarmLog);
}

BOOL CLog::Alarm(int nErrorCode)
{
	//待完善
	return TRUE;
}

BOOL CLog::Alarm(const CString &strLog)
{
	//显示异常，报警， 或停机
	theGlobal.SetAlarmInfo(strLog);

	return Log(strLog, AlarmLog);
}

//BOOL CLog::UploadAlarm(const CString &strLog)
//{
//	//显示异常，报警， 或停机
//	theGlobal.ShowUploadDlg(strLog);
//
//	return Log(strLog, AlarmLog);
//}

BOOL CLog::Communication(const CString &strLog)
{
	return Log(strLog, CommunicationLog);
}

BOOL WINAPI CLog::LogCallBack(const CString& strLog,const int& nLogType)
{
	//显示异常，报警， 或停机
	if (nLogType == AlarmLog) theGlobal.SetAlarmInfo(strLog);

	return theGlobal.m_Log.Log(strLog,LogType(nLogType));
}

BOOL WINAPI CLog::LogCallBack(const CString& strLog,const int& nLogType,const int& nExcpLevel)
{
	//显示异常，报警， 或停机
	if (nLogType == AlarmLog)
	{
		CErrorInfo::tagErrorInfo tInfo;
		tInfo.nID = 0;
		tInfo.strDescribe = strLog;
		tInfo.strSolution = "";
		theGlobal.SetAlarmInfo(tInfo);
		if (nExcpLevel == FatalExcp)
		{
			theGlobal.m_Upload.SendAlarmInfoToServer(tInfo);
		}
	}

	return theGlobal.m_Log.Log(strLog,LogType(nLogType));
}

BOOL CLog::Operate(const double& dOld,const double& dCurrent,const CString& strOperateName)
{
	if (dOld == dCurrent)
		return FALSE;

	CString strLog;
	strLog.Format("'%s'由(%0.2f)更改为(%0.2f)",strOperateName,dOld,dCurrent);
	return Log(strLog,OperateLog);
}

BOOL CLog::Operate(const int& nOld,const int& nCurrent,const CString& strOperateName)
{
	if (nOld == nCurrent)
		return FALSE;

	CString strLog;
	strLog.Format("'%s'由(%d)更改为(%d)",strOperateName,nOld,nCurrent);
	return Log(strLog,OperateLog);
}

BOOL CLog::Operate(const CString& strOld,const CString& strCurrent,const CString& strOperateName)
{
	if (strOld == strCurrent)
		return FALSE;

	CString strLog;
	strLog.Format("'%s'由(%s)更改为(%s)",strOperateName,strOld,strCurrent);
	return Log(strLog,OperateLog);
}

BOOL CLog::Operate(const CString& strOperation)
{
	return Log(strOperation, OperateLog);
}

CString CLog::GetLogSaveFileName(const CString& strSaveDir,const CString& strOriName,const CString& strNewName)
{
	CString strFilePath,strFileName=strNewName;
	strFilePath.Format("%s//%s.log",strSaveDir,strFileName);

	if (PathFileExists(strFilePath))
	{
		CFileStatus fileStatus;
		CFile::GetStatus(strFilePath, fileStatus);
		if (fileStatus.m_size>m_nSingleLogFileSize*1024*1024)
		{
			int nPos = strFileName.Find(strOriName);
			CString strRemain = strFileName.Mid(nPos+strOriName.GetLength());
			if (strRemain.Left(1)=="_")
				strRemain = strRemain.Right(strRemain.GetLength()-1);

			int nSN = atoi(strRemain);
			strFileName.Format("%s_%d",strOriName,++nSN);
			return GetLogSaveFileName(strSaveDir,strOriName,strFileName);
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

	return GetLogSaveFileName(strSaveDir,strOriName,strFileName);
}

BOOL CLog::ReadLogFile(const CString& strLogBeginName,HTuple* hvLog,HTuple* hvLevel)
{
	(*hvLog).Clear();
	(*hvLevel).Clear();
	CString strLogPath;
	strLogPath.Format("%s\\Log\\RunLog\\",GetCurrentAppPath());

	return FindAndReadLogFileFromFolder(strLogPath,strLogBeginName,hvLog,hvLevel);
}

BOOL CLog::FindAndReadLogFileFromFolder(const CString& strFolderPath,const CString& strSameName,HTuple* hvLog,HTuple* hvLevel)
{
	CString strFullName;
	HTuple hvFiles,hvPos,hvInc,hvGreater,hvFileSelected;
	ListFiles(HTuple(strFolderPath),HTuple("files").Append("recursive"),&hvFiles);
	TupleStrstr(hvFiles,HTuple(strSameName),&hvPos);
	TupleGreaterEqualElem(hvPos,0,&hvGreater);
	TupleFind(hvGreater,1,&hvInc);
	if(hvInc.I()==-1) return FALSE;
	TupleSelect(hvFiles,hvInc,&hvFileSelected);

	long lSize=0;
	for (int i=0;i<hvFileSelected.Length();i++)
	{
		strFullName.Format("%s",hvFileSelected[i].S().Text());
		if(strFullName.Right(3)!="log") return FALSE;

		CFileStatus fileStatus;
		if (CFile::GetStatus(strFullName, fileStatus))
		{
			lSize+=fileStatus.m_size;
			if(fileStatus.m_size > 10*1024*1024)
			{
				CString strExcp,strExcp1;
				strExcp.Format("日志文件太大(%0.1fMB)，无法读取\r\n%s",((double)fileStatus.m_size)/1024.0/1024.0,strFullName);
				strExcp1.Format("Log file is too large(%0.1fMB), and cannot be read\r\n%s", ((double)fileStatus.m_size) / 1024.0 / 1024.0, strFullName);
				theGlobal.m_Language.MessageboxError(strExcp, strExcp1, "");
				return FALSE;
			}
			if(lSize > 10*1024*1024)
			{
				CString strExcp, strExcp1;
				strExcp.Format("日志文件读取数据已超过%0.1fMB，停止读取",((double)lSize)/1024.0/1024.0);
				strExcp1.Format("The data readed from log file is over %0.1fMB, read stopped.", ((double)lSize) / 1024.0 / 1024.0);
				theGlobal.m_Language.MessageboxError(strExcp, strExcp1, "");
				return FALSE;
			}
		}
		AddLogFromFile(strFullName,hvLog,hvLevel);
	}
	return TRUE;
}

BOOL CLog::AddLogFromFile(const CString& strLogPath,HTuple* hvLog,HTuple* hvLevel)
{
	try
	{
		HTuple hvHdl,hvLine,hvEOF;
		OpenFile(HTuple(strLogPath),"input",&hvHdl);
		CString strLine;
		FreadLine(hvHdl,&hvLine,&hvEOF);
		while(!hvEOF.I())
		{
			(*hvLog).Append(HTuple(hvLine));
			strLine.Format("%s",hvLine.S().Text());
			int nLvlSN=-1;
			if(-1!=strLine.Find("[Std]"))
				nLvlSN = 0;
			else if(-1!=strLine.Find("[Operation]"))
				nLvlSN = 1;
			else if(-1!=strLine.Find("[Exception]"))
				nLvlSN = 2;
			else
				nLvlSN = 3;

			(*hvLevel).Append(nLvlSN);
			FreadLine(hvHdl,&hvLine,&hvEOF);
		}
		CloseFile(hvHdl);
	}catch(HException&)
	{
		CString strExcp, strExcp1;
		strExcp.Format("读取日志文件异常\n%s",strLogPath);
		strExcp1.Format("Read log file exceptional\n%s", strLogPath);
		theGlobal.m_Language.MessageboxError(strExcp, strExcp1, "");
		return FALSE;
	}
	return TRUE;
}