//////////////////////////////////////////////////////////////////////////
///Log.h 用于日志记录
//////////////////////////////////////////////////////////////////////////
#pragma once
#include <vector>
#include "BaseTmpl.h"
#include "Log.in.h"
#include "ErrorInfo.h"

class CLog : public CBaseTmpl
{
public:
	struct tagLogInfo
	{
		LogType type;
		CString strLog;
	}LogInfo,*PLogInfo;
	typedef std::vector<tagLogInfo> LogList;

public:
	CLog();
	~CLog();

	BOOL Init();
	BOOL ReadParam();
	BOOL WriteParam();

	//日志
	BOOL Log(const CString &strLog,LogType emType=StdLog);
	BOOL Log(const CString &strLog,const int& nType);
	//标准运行日志
	BOOL Std(const CString &strLog);
	//异常日志
	BOOL Exception(const CString &strLog);
	//调试日志
	BOOL Debug(const CString &strLog);
	//报警日志
	BOOL Alarm(const CString &strLog);
	BOOL Alarm(int nErrorCode);
	BOOL Alarm(CErrorInfo::tagErrorInfo tInfo);
	////通讯断联专用
	//BOOL UploadAlarm(const CString &strLog);
	//报警日志
	BOOL Communication(const CString &strLog);
	//操作日志
	BOOL Operate(const double& dOld,const double& dCurrent,const CString& strOperateName);
	BOOL Operate(const int& nOld,const int& nCurrent,const CString& strOperateName);
	BOOL Operate(const CString& strOld,const CString& strCurrent,const CString& strOperateName);
	BOOL Operate(const CString& strOperation);
	
	//日志回调
	static BOOL WINAPI LogCallBack(const CString& strLog, const int& nLogType);
	static BOOL WINAPI LogCallBack(const CString& strLog, const int& nLogType, const int& nExcpLevel);

	//查看日志
	void ReviewLog() 
	{
		if (m_pLog == NULL) return;
		m_pLog->DoSomething("ViewLog", "", NULL);
	}

	//进入日志设置对话框
	void DispSetLogDialog()
	{
		if (m_pLog == NULL) return;
		m_pLog->DoSomething("ShowLog", "", NULL);
	}

	
	CString	GetLogPath(){return m_strLogPath;}
	CString	GetOperateLogPath(){return m_strOperateLogPath;}
	const LogList & GetDispLogList()
	{
		//CLockHelper lk(&m_csLock);
		//LogList lst = m_vecDispLogList;
		return m_vecDispLogList;
	}

	void SetLogLevel(const int& nLevel){m_nLogLevel=nLevel;}
	int  GetLogLevel(){return m_nLogLevel;}
	void SetListDispLevel(const int& nLevel){m_nListDispLevel=nLevel;}
	int  GetListDispLevel(){return m_nListDispLevel;}

	BOOL ReadLogFile(const CString& strLogBeginName,HTuple* hvLog,HTuple* hvLevel);

private:
	CString GetLogSaveFileName(const CString& strSaveDir,const CString& strOriName,const CString& strNewName);
	BOOL FindAndReadLogFileFromFolder(const CString& strFolderPath,const CString& strSameName,HTuple* hvLog,HTuple* hvLevel);
	BOOL AddLogFromFile(const CString& strLogPath,HTuple* hvLog,HTuple* hvLevel);

private:
	int			m_nLogLevel;			//日志记录等级，0：不记录 1：运行日志 2：运行日志,用户报警日志 3：运行日志,用户报警日志,操作日志 4:运行日志,用户报警日志,操作日志,通信日志 5:运行日志,用户报警日志,操作日志,通信日志,异常日志 6:运行日志,用户报警日志,操作日志,通信日志,异常日志,程序调试日志
	int			m_nListLength;			//实时显示日志长度
	int			m_nListDispLevel;		//实时日志显示等级
	CString		m_strLogPath;			//日志路径
	CString		m_strOperateLogPath;	//操作日志路径
	int			m_nSingleLogFileSize;	//日志大小

	LogList		m_vecDispLogList;		//实时显示日志列表

	CErrorInfo	m_ErrorInfo;			//错误码/错误信息类

	CRITICAL_SECTION	m_csLogPathLock;
	DHInterface*		m_pLog;
};