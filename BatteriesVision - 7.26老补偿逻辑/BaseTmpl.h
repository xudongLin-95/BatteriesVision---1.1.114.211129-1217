#pragma once
//////////////////////////////////////////////////////////////////////////
////自定义锁类，用于同时修改资源时互锁
//////////////////////////////////////////////////////////////////////////
class CLockHelper
{
public:
	CLockHelper(CRITICAL_SECTION *pcs)
	{
		m_pcs = pcs;
		EnterCriticalSection(m_pcs);
	}

	~CLockHelper()
	{
		LeaveCriticalSection(m_pcs);
		m_pcs = NULL;
	}

private:
	CRITICAL_SECTION *m_pcs;
};


//////////////////////////////////////////////////////////////////////////
////BaseTmpl 工具基类,所有大类派生于此
//////////////////////////////////////////////////////////////////////////
class CBaseTmpl
{
public:
	enum eIniType
	{
		SystemFile=0,	//system file
		CommonFile,		//common file
	};
public:
	CBaseTmpl();
	~CBaseTmpl();

	virtual BOOL Init()=0;
	virtual BOOL ReadParam()=0;
	virtual BOOL WriteParam()=0;

	void DeleteIniSection(CString lpAppName,eIniType type=SystemFile);
	void DeleteIniKey(CString lpAppName,CString lpKeyName,eIniType type=SystemFile);

	void WriteIniString(CString lpAppName,CString lpKeyName,const CString& Value, eIniType type=SystemFile);
	void WriteIniInt(CString lpAppName,CString lpKeyName,const int& Value, eIniType type=SystemFile);
	void WriteIniDouble(CString lpAppName,CString lpKeyName,const double& Value, eIniType type=SystemFile);

	CString GetIniString(CString lpAppName,CString lpKeyName,CString lpDefault, eIniType type=SystemFile);
	int		GetIniInt(CString lpAppName,CString lpKeyName,int nDefault, eIniType type=SystemFile);
	double	GetIniDouble(CString lpAppName,CString lpKeyName,double dDefault, eIniType type=SystemFile);

private:
public:
	CRITICAL_SECTION m_csLock;
	CString m_strSystemFilePath;
	CString m_strCommonFilePath;

private:
	static CRITICAL_SECTION m_csSystemFileLock;
	static CRITICAL_SECTION m_csCommonFileLock;
};
