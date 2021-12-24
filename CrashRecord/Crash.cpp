#include "stdafx.h"
#include "Crash.h"
#include <windows.h>
#include"resource.h"
#include "DlgCrashInfo.h"
#include "ProcessInfoCollect.h"
#include<sys/types.h> 
#include<sys/stat.h>

#pragma comment(lib, "Dbghelp.lib")
#pragma comment(lib,"shlwapi.lib")
#pragma comment(lib,"Psapi.lib")

extern "C" __declspec(dllexport) DHInterface* Export(void)
{
	return (DHInterface*)new CCrashRecord();
}


CCrashRecord* g_CrashRecord=NULL; 
CCrashRecord::CCrashRecord()
	:m_bExit(FALSE)
{
	

	TCHAR curDirPath[MAX_PATH];
	//取当前执行模块的全路径,如果此模块是被其它程序调用的,返回的路径还是这个程序的路径
	::GetModuleFileName(NULL, curDirPath,MAX_PATH);
	//从路径中移除文件名
	PathRemoveFileSpec(curDirPath);
	TCHAR  CharFilePath[MAX_PATH];

	sprintf(CharFilePath,"%s\\Log",curDirPath);
	if (!PathIsDirectory(CharFilePath))
		CreateDirectory(CharFilePath,NULL);
	sprintf(CharFilePath,"%s\\Log\\ExceptionReport",curDirPath);
	if (!PathIsDirectory(CharFilePath))
		CreateDirectory(CharFilePath,NULL);

	SYSTEMTIME currentTime;

	GetLocalTime(&currentTime);

	sprintf(fullCharFilePath,"%s\\MemoryRecord-%04d-%02d-%02d.log",
		CharFilePath,currentTime.wYear,currentTime.wMonth,currentTime.wDay);

	g_CrashRecord = this;
	::CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)ThreadWatchMemoty,this,NULL,NULL);

}

CCrashRecord::~CCrashRecord()
{
	m_bExit = TRUE;

	Sleep(100);
}

LONG WINAPI ExceptionFilter(EXCEPTION_POINTERS* _pExcp)
{
	TCHAR curDirPath[MAX_PATH];
	//取当前执行模块的全路径,如果此模块是被其它程序调用的,返回的路径还是这个程序的路径

	::GetModuleFileName(NULL, curDirPath,MAX_PATH);
	//从路径中移除文件名
	PathRemoveFileSpec(curDirPath);
	TCHAR  CharFilePath[MAX_PATH];
	TCHAR  fullCharFilePath[MAX_PATH];
	sprintf(CharFilePath,"%s\\Log",curDirPath);
	if (!PathIsDirectory(CharFilePath))
		CreateDirectory(CharFilePath,NULL);
	sprintf(CharFilePath,"%s\\Log\\ExceptionReport",curDirPath);
	if (!PathIsDirectory(CharFilePath))
		CreateDirectory(CharFilePath,NULL);
	SYSTEMTIME currentTime;
	GetLocalTime(&currentTime);

	sprintf(fullCharFilePath,"%s\\CrashRecord-%04d-%02d-%02d-%02d-%02d-%02d.dmp",
		CharFilePath,currentTime.wYear,currentTime.wMonth,currentTime.wDay,currentTime.wHour,currentTime.wMinute,currentTime.wSecond);

	HANDLE hFile = CreateFile(  
		fullCharFilePath,   
		GENERIC_WRITE,   
		0,   
		NULL,   
		CREATE_ALWAYS,   
		FILE_ATTRIBUTE_NORMAL,   
		NULL);  
	if (INVALID_HANDLE_VALUE == hFile)  
	{
		return EXCEPTION_CONTINUE_EXECUTION;
	}

	MINIDUMP_EXCEPTION_INFORMATION einfo = {0};  
	einfo.ThreadId = ::GetCurrentThreadId();  
	einfo.ExceptionPointers = _pExcp;  
	einfo.ClientPointers = FALSE;  

	MiniDumpWriteDump(GetCurrentProcess(),   
		GetCurrentProcessId(),   
		hFile,   
		MiniDumpNormal,   
		&einfo,   
		NULL,   
		NULL);  
	CloseHandle(hFile);

	CString strExcp;
	strExcp.Format("CrashRecord: 捕获到程序崩溃,已保存记录文件(%s)", fullCharFilePath);
	if (g_CrashRecord->m_tInit.pLog != NULL)
		g_CrashRecord->m_tInit.pLog(strExcp, ExcpLog, SlightExcp);

	g_CrashRecord->m_ExcpInfo = einfo;

	//显示对话框
	g_CrashRecord->ShowCrashDlg();

	return EXCEPTION_EXECUTE_HANDLER;
}

LPTOP_LEVEL_EXCEPTION_FILTER CCrashRecord::SetCustomUnhandledExceptionFilter()
{
	return SetUnhandledExceptionFilter(ExceptionFilter);
}

int CCrashRecord::DoSomething(void* message,void* wParam,void* lParam)
{
	CString strMessage;
	strMessage.Format("%s",(char*)message);
	if ("INITIALIZE" == strMessage || "Initialize" == strMessage || "Init" == strMessage)
	{
		return Init(*(PCrashRecordInit)wParam);
	}
	else if ("EXCEPTIONFILTER" == strMessage || "ExceptionFilter" == strMessage  || "CrashRecord" == strMessage)
	{
		SetCustomUnhandledExceptionFilter();
		return TRUE;
	}
	else if ("GetCPURate" == strMessage)
	{
		double dRate = GetCPURate();
		*(double*)lParam = dRate;
		return TRUE;
	}
	else if ("GetRAMUseMB" == strMessage)
	{
		double dUse = GetRAMUseMB();
		*(double*)lParam = dUse;
		return TRUE;
	}
	return TRUE;
}

int CCrashRecord::Init(CrashRecordInit& tInit)
{
	m_tInit = tInit;

	return TRUE;
}

double CCrashRecord::GetCPURate()
{
	ProcessInfoCollect picProcessInfoCollect;    
	double dCPUUserRate = 0;                        //CPU使用的百分比        
	picProcessInfoCollect.GetCPUUserRate(dCPUUserRate);

	return dCPUUserRate;
}

double CCrashRecord::GetRAMUseMB()
{
	ProcessInfoCollect picProcessInfoCollect;

	DWORD             nMemoryUsed;                    //内存使用(Byte)    
	DWORD            nVirtualMemoryUsed;              //虚拟内存使用(Byte)          
	picProcessInfoCollect.GetMemoryUsed(nVirtualMemoryUsed, nMemoryUsed);

	return ((double)nMemoryUsed / 1024 / 1024);
}

BOOL CCrashRecord::RestartProcess()
{
	char pth[2048];
	GetModuleFileNameA(NULL,pth,MAX_PATH); 
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	memset((void*)&si,0,sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	si.wShowWindow = SW_SHOW;
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	CreateProcessA(NULL,pth,NULL,NULL,TRUE,0,NULL,NULL,&si,&pi);
	::TerminateProcess(::GetCurrentProcess(),NULL);//终止原先程序

	return TRUE;
}

void CCrashRecord::ShowCrashDlg()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CDlgCrashInfo dlg;
	dlg.DoModal();
}

UINT CCrashRecord::ThreadWatchMemoty(void* pVoid)
{
	CCrashRecord * pCrash = (CCrashRecord *)pVoid;
	CStdioFile fileMemory;
	CString cstrMessage;

	bool bWrited = false;

	while(!pCrash->m_bExit)
	{
		SYSTEMTIME currentTime;
		GetLocalTime(&currentTime);
		if (currentTime.wMinute%10 == 9 && bWrited)
			bWrited = false;

		if (currentTime.wMinute%10 != 0 || bWrited)
		{
			Sleep(500);
			continue;
		}


		/*
		SYSTEM_INFO si;
		GetSystemInfo(&si);
		DWORD pid = GetCurrentProcessId();
		
		PROCESS_MEMORY_COUNTERS pmc;

		GetProcessMemoryInfo(handle, &pmc, sizeof(pmc));
		*/
		HANDLE handle = GetCurrentProcess();
		TCHAR szProcessName[MAX_PATH];
		memset(szProcessName,0,MAX_PATH);

		GetModuleFileNameEx(handle,NULL,szProcessName,MAX_PATH);

		ProcessInfoCollect picProcessInfoCollect;
		int nRet = 0;    
		DWORD             nMemoryUsed;                    //内存使用(Byte)    
		DWORD            nVirtualMemoryUsed;                //虚拟内存使用(Byte)    
		DWORD            nHandleNumber;                    //句柄数量
		DWORD dwCurrentProcessThreadCount;        //线程数量    
		ULONGLONG ullIo_read_bytes;                        //IO读字节数    
		ULONGLONG ullIo_write_bytes;                    //IO写字节数    
		ULONGLONG ullIo_wct;                            //IO写次数    
		ULONGLONG ullIo_rct;                            //IO读次数        
		double dCPUUserRate = 0;                        //CPU使用的百分比        
		picProcessInfoCollect.GetCPUUserRate(dCPUUserRate);    

		picProcessInfoCollect.GetMemoryUsed(nVirtualMemoryUsed, nMemoryUsed);
		nVirtualMemoryUsed = nVirtualMemoryUsed;    
		nMemoryUsed = nMemoryUsed;    
		picProcessInfoCollect.GetThreadCount(dwCurrentProcessThreadCount);    
		picProcessInfoCollect.GetHandleCount(nHandleNumber);    
		picProcessInfoCollect.GetIOBytes(&ullIo_read_bytes, &ullIo_write_bytes, &ullIo_wct, &ullIo_rct);

		cstrMessage.Format("%04d/%02d/%02d-%02d:%02d:%02d:%03d\t进程ID：%d\t进程名：%s\t内存：%0.2f MB\tCPU：%0.2f%%\t线程数：%d\r\n",
			currentTime.wYear,currentTime.wMonth,currentTime.wDay,currentTime.wHour,currentTime.wMinute,currentTime.wSecond,currentTime.wMilliseconds,GetCurrentProcessId(),szProcessName,((double)nMemoryUsed/1024/1024),dCPUUserRate,dwCurrentProcessThreadCount);

		try
		{
			pCrash->IsMaxFileSize();

			fileMemory.Open(pCrash->fullCharFilePath,CFile::modeWrite|CFile::modeCreate|CFile::modeNoTruncate);

			fileMemory.SeekToEnd();

			fileMemory.Write(cstrMessage.GetBuffer(cstrMessage.GetLength()),cstrMessage.GetLength());

			fileMemory.Close();
		}
		catch (...)
		{
			fileMemory.Close();
		}
		bWrited = true;

		Sleep(500);
		//GetSystemMemoryInfo();
	}
	
	return 0;
}

BOOL CCrashRecord::IsMaxFileSize(int const& nSize ) 
{
	CString cstrMessageWrite;

	CString cstrTime;

	SYSTEMTIME currentTime;

	GetLocalTime(&currentTime);

	struct _stat info;

	CString cstrFilePath,cstrName,cstrNewName,cstrExtName,cstrNewFileFullName;

	TCHAR curDirPath[MAX_PATH];
	//取当前执行模块的全路径,如果此模块是被其它程序调用的,返回的路径还是这个程序的路径
	::GetModuleFileName(NULL, curDirPath,MAX_PATH);
	//从路径中移除文件名
	PathRemoveFileSpec(curDirPath);
	TCHAR  CharFilePath[MAX_PATH];

	sprintf(CharFilePath,"%s\\Log",curDirPath);
	if (!PathIsDirectory(CharFilePath))
		CreateDirectory(CharFilePath,NULL);
	sprintf(CharFilePath,"%s\\Log\\ExceptionReport",curDirPath);
	if (!PathIsDirectory(CharFilePath))
		CreateDirectory(CharFilePath,NULL);
	
	cstrNewName.Format(_T("%s\\MemoryRecord-%04d-%02d-%02d-%02d-%02d-%02d.log"),CharFilePath,currentTime.wYear,currentTime.wMonth,currentTime.wDay,currentTime.wHour,currentTime.wMinute,currentTime.wSecond);

	_stat(fullCharFilePath, &info);

	double size = info.st_size;

	if (((double)(size)/1024/1024) > nSize)		//1500000
	{
		try
		{
			sprintf(fullCharFilePath,"%s",cstrNewName);
		}
		catch (CException* e)
		{
			return FALSE;
		}
	}
	
	return TRUE;
}

void CCrashRecord::Release()
{
	delete this;
}