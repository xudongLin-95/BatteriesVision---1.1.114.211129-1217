#pragma once
#include <DbgHelp.h>
#include "shlwapi.h"
#include "CrashRecord.in.h"

class CCrashRecord : public DHInterface
{
public:
	CCrashRecord();
	~CCrashRecord();
	virtual int DoSomething(void* message,void* wParam,void* lParam);

public:
	LPTOP_LEVEL_EXCEPTION_FILTER SetCustomUnhandledExceptionFilter();
	BOOL RestartProcess();
	void ShowCrashDlg();
	BOOL IsMaxFileSize(int const & nSize = 3 ) ;
	void Release();
public:
	MINIDUMP_EXCEPTION_INFORMATION m_ExcpInfo;
	BOOL m_bExit;
	CrashRecordInit m_tInit;
private:
	TCHAR  fullCharFilePath[MAX_PATH];
	
private:
	int Init(CrashRecordInit& tInit);
	double GetCPURate();
	double GetRAMUseMB();
	static UINT ThreadWatchMemoty(void* pVoid);

};
extern CCrashRecord* g_CrashRecord;
