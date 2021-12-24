//////////////////////////////////////////////////////////////////////////
///Log.h ������־��¼
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

	//��־
	BOOL Log(const CString &strLog,LogType emType=StdLog);
	BOOL Log(const CString &strLog,const int& nType);
	//��׼������־
	BOOL Std(const CString &strLog);
	//�쳣��־
	BOOL Exception(const CString &strLog);
	//������־
	BOOL Debug(const CString &strLog);
	//������־
	BOOL Alarm(const CString &strLog);
	BOOL Alarm(int nErrorCode);
	BOOL Alarm(CErrorInfo::tagErrorInfo tInfo);
	////ͨѶ����ר��
	//BOOL UploadAlarm(const CString &strLog);
	//������־
	BOOL Communication(const CString &strLog);
	//������־
	BOOL Operate(const double& dOld,const double& dCurrent,const CString& strOperateName);
	BOOL Operate(const int& nOld,const int& nCurrent,const CString& strOperateName);
	BOOL Operate(const CString& strOld,const CString& strCurrent,const CString& strOperateName);
	BOOL Operate(const CString& strOperation);
	
	//��־�ص�
	static BOOL WINAPI LogCallBack(const CString& strLog, const int& nLogType);
	static BOOL WINAPI LogCallBack(const CString& strLog, const int& nLogType, const int& nExcpLevel);

	//�鿴��־
	void ReviewLog() 
	{
		if (m_pLog == NULL) return;
		m_pLog->DoSomething("ViewLog", "", NULL);
	}

	//������־���öԻ���
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
	int			m_nLogLevel;			//��־��¼�ȼ���0������¼ 1��������־ 2��������־,�û�������־ 3��������־,�û�������־,������־ 4:������־,�û�������־,������־,ͨ����־ 5:������־,�û�������־,������־,ͨ����־,�쳣��־ 6:������־,�û�������־,������־,ͨ����־,�쳣��־,���������־
	int			m_nListLength;			//ʵʱ��ʾ��־����
	int			m_nListDispLevel;		//ʵʱ��־��ʾ�ȼ�
	CString		m_strLogPath;			//��־·��
	CString		m_strOperateLogPath;	//������־·��
	int			m_nSingleLogFileSize;	//��־��С

	LogList		m_vecDispLogList;		//ʵʱ��ʾ��־�б�

	CErrorInfo	m_ErrorInfo;			//������/������Ϣ��

	CRITICAL_SECTION	m_csLogPathLock;
	DHInterface*		m_pLog;
};