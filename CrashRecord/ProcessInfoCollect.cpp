#include "stdafx.h"
#include "ProcessInfoCollect.h"
#include <stdio.h>
#include <tchar.h>
#include <TLHELP32.H>
#include <winsvc.h>
#include <psapi.h>
#pragma comment(lib,"psapi.lib")

#ifdef StartProcessBySysServiceEx_Flag
#include <Userenv.h>
#include <WtsApi32.h>
#include <atlbase.h>
#pragma comment(lib, "WtsApi32.lib")
#pragma comment(lib,"Userenv.lib")
#endif


#define			MAX_ID							4096			//��������
#define			MAX_CHILD_PROCESS_COUNT			256				//�ӽ�����

//�õ��ļ���(������չ��)  
const char* GetFileName(const char* pFile)
{
	if (NULL == pFile || 0 == strlen(pFile))
	{
		return "";
	}

	const char *pPos = strrchr(pFile, '\\');
	if (NULL == pPos)
	{
		pPos = strrchr(pFile, '/');

		if (NULL == pPos)
		{
			return "";
		}
	}

	return pPos + 1;
}

//�õ��ļ���(������չ��)  
void GetFileNameWithoutExtendName(const char* pFile, char *pFileDest)
{
	if (NULL == pFile || 0 == strlen(pFile) || NULL == pFileDest)
		return;

	char *pPos = (char *)strrchr(pFile, '\\');
	if (NULL == pPos)
	{
		pPos = (char *)strrchr(pFile, '/');
	}
	if (NULL == pPos)
	{
		pPos = (char *)pFile;
	}
	if (pPos != NULL)
	{
		char *pSplitFlag = strrchr(pPos, '.');
		if (pSplitFlag != NULL)
		{
			int nLenTemp = pSplitFlag - pPos - 1;
			strncpy(pFileDest, pPos + 1, nLenTemp);
			pFileDest[nLenTemp] = '\0';
		}
		else
		{
			strcpy(pFileDest, pPos + 1);
		}
	}
	else
	{
		strcpy(pFileDest, pFile);
	}
}

#define TokenLinkedToken 19

#ifdef StartProcessBySysServiceEx_Flag
DWORD GetActiveSessionID()
{
	DWORD dwSessionId = 0;
	PWTS_SESSION_INFO pSessionInfo = NULL;
	DWORD dwCount = 0;
	WTSEnumerateSessions(WTS_CURRENT_SERVER_HANDLE, 0, 1, &pSessionInfo, &dwCount);
	for (DWORD i = 0; i < dwCount; i++)
	{
		WTS_SESSION_INFO si = pSessionInfo[i];
		if (WTSActive == si.State)
		{
			dwSessionId = si.SessionId;
			break;
		}
	}
	WTSFreeMemory(pSessionInfo);
	return dwSessionId;
}
#endif

ProcessInfoCollect::ProcessInfoCollect()
{
}

ProcessInfoCollect::~ProcessInfoCollect()
{
}

//��ȡcpuʹ����
BOOL  ProcessInfoCollect::GetCPUUserRate(double & dCPUUserRate)
{
	HANDLE hProcess = ::GetCurrentProcess();
	return GetCPUUserRateEx(hProcess, dCPUUserRate);
}

//��ȡָ�����̵�cpuʹ����
BOOL    ProcessInfoCollect::GetCPUUserRate(DWORD lProcessID, double & dCPUUserRate)
{
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, lProcessID);
	if (hProcess == NULL)
		return FALSE;

	BOOL bSuccess = GetCPUUserRateEx(hProcess, dCPUUserRate);
	CloseHandle(hProcess);
	return bSuccess;
}

int ProcessInfoCollect::GetIOBytes(ULONGLONG * read_bytes, ULONGLONG * write_bytes, ULONGLONG * wct, ULONGLONG * rct)
{
	HANDLE hProcess = GetCurrentProcess();//��ȡ��ǰ���̾��
	int nRet = GetIOBytesEx(hProcess, read_bytes, write_bytes, wct, rct);
	return nRet;
}
//��ȡָ�����̵�IO����
int	ProcessInfoCollect::GetIOBytes(DWORD lProcessID, ULONGLONG * read_bytes, ULONGLONG * write_bytes, ULONGLONG * wct, ULONGLONG * rct)
{
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, lProcessID);
	if (hProcess == NULL)
		return -1;

	int nRet = GetIOBytesEx(hProcess, read_bytes, write_bytes, wct, rct);
	CloseHandle(hProcess);
	return nRet;
}

//��ȡ�����
BOOL	ProcessInfoCollect::GetHandleCount(DWORD &dwHandles)
{
	return GetHandleCountEx(GetCurrentProcess(),dwHandles);
}
BOOL	ProcessInfoCollect::GetHandleCount(DWORD lProcessID, DWORD &dwHandles)
{
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, lProcessID);
	if (hProcess == NULL)
		return FALSE;

	BOOL bSuccess = GetHandleCountEx(hProcess, dwHandles);
	CloseHandle(hProcess);
	return bSuccess;
}

//��ȡ��ǰ���̵��ڴ�
BOOL	ProcessInfoCollect::GetMemoryUsed(DWORD & dwPeakWorkingSetSize, DWORD & dwWorkingSetSize)
{
	HANDLE hProcess = GetCurrentProcess();//��ȡ��ǰ���̾��
	return GetMemoryUsedEx(hProcess, dwPeakWorkingSetSize, dwWorkingSetSize);
}
//��ȡָ�����̵��ڴ�
BOOL	ProcessInfoCollect::GetMemoryUsed(DWORD lProcessID, DWORD & dwPeakWorkingSetSize, DWORD & dwWorkingSetSize)
{
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, lProcessID);
	if (hProcess == NULL)
		return FALSE;

	BOOL bSuccess = GetMemoryUsedEx(hProcess, dwPeakWorkingSetSize, dwWorkingSetSize);
	CloseHandle(hProcess);
	return bSuccess;
}

//��ȡָ�����̵�cpuʹ����
BOOL    ProcessInfoCollect::GetCPUUserRateEx(HANDLE hProccess, double & dCPUUserRate)
{
	static DWORD s_dwTickCountOld = 0;
	static LARGE_INTEGER s_lgProcessTimeOld = { 0 };
	static DWORD s_dwProcessorCoreNum = 0;
	if (!s_dwProcessorCoreNum)
	{
		SYSTEM_INFO sysInfo = { 0 };
		GetSystemInfo(&sysInfo);
		s_dwProcessorCoreNum = sysInfo.dwNumberOfProcessors;
	}
	double dbProcCpuPercent = 0;
	BOOL bRetCode = FALSE;
	FILETIME CreateTime, ExitTime, KernelTime, UserTime;
	LARGE_INTEGER lgKernelTime;
	LARGE_INTEGER lgUserTime;
	LARGE_INTEGER lgCurTime;
	bRetCode = GetProcessTimes(hProccess, &CreateTime, &ExitTime, &KernelTime, &UserTime);
	if (bRetCode)
	{
		lgKernelTime.HighPart = KernelTime.dwHighDateTime;
		lgKernelTime.LowPart = KernelTime.dwLowDateTime;
		lgUserTime.HighPart = UserTime.dwHighDateTime;
		lgUserTime.LowPart = UserTime.dwLowDateTime;
		lgCurTime.QuadPart = (lgKernelTime.QuadPart + lgUserTime.QuadPart);
		if (s_lgProcessTimeOld.QuadPart)
		{
			DWORD dwElepsedTime = ::GetTickCount() - s_dwTickCountOld;
			dbProcCpuPercent = (double)(((double)((lgCurTime.QuadPart - s_lgProcessTimeOld.QuadPart) * 100)) / dwElepsedTime) / 10000;
			dbProcCpuPercent = dbProcCpuPercent / s_dwProcessorCoreNum;
		}
		s_lgProcessTimeOld = lgCurTime;
		s_dwTickCountOld = ::GetTickCount();
	}
	dCPUUserRate = dbProcCpuPercent;
	return bRetCode;
}
//��ȡָ�����̵�IO����
int		ProcessInfoCollect::GetIOBytesEx(HANDLE hProccess, ULONGLONG * read_bytes, ULONGLONG * write_bytes, ULONGLONG * wct, ULONGLONG * rct)
{
	IO_COUNTERS io_counter;
	if (GetProcessIoCounters(hProccess, &io_counter))
	{
		if (read_bytes) *read_bytes = io_counter.ReadTransferCount;//�ֽ���
		if (write_bytes) *write_bytes = io_counter.WriteTransferCount;
		if (wct) *wct = io_counter.WriteOperationCount;//����
		if (rct) *rct = io_counter.ReadOperationCount;
		return 0;
	}
	return -1;
}

//��ȡ�ڴ�
//������hProccess�����̾����dwPeakWorkingSetSize:ʹ���ڴ�߷�;dwWorkingSetSize:��ǰʹ�õ��ڴ�;
BOOL	ProcessInfoCollect::GetMemoryUsedEx(HANDLE hProccess, DWORD & dwPeakWorkingSetSize, DWORD & dwWorkingSetSize)
{
	//���ݽ���ID�򿪽���
	if (hProccess)
	{
		//SYSTEM_INFO si;
		//GetSystemInfo(&si);
		//OSVERSIONINFO osvi;//����OSVERSIONINFO���ݽṹ����
		//memset(&osvi, 0, sizeof(OSVERSIONINFO));//���ռ� 
		//osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);//�����С 
		//GetVersionEx(&osvi);//��ð汾��Ϣ 
		//if (osvi.dwMajorVersion < 6)
		{
			PROCESS_MEMORY_COUNTERS_EX pmc = { 0 };
			//pmc.cb = sizeof(PROCESS_MEMORY_COUNTERS);
			//��ȡ������̵��ڴ�ʹ�������
			if (::GetProcessMemoryInfo(hProccess, (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc)))
			{
				dwWorkingSetSize = pmc.WorkingSetSize;//pmc.WorkingSetSize;
				dwPeakWorkingSetSize = pmc.PeakWorkingSetSize;
				//ȱҳ�жϴ���:pmc.PageFaultCount
				//ʹ���ڴ�߷�:pmc.PeakWorkingSetSize
				//��ǰʹ�õ��ڴ�: pmc.WorkingSetSize
				//ʹ��ҳ�滺��ظ߷�: pmc.QuotaPeakPagedPoolUsage
				//ʹ��ҳ�滺���: pmc.QuotaPagedPoolUsage
				//ʹ�÷Ƿ�ҳ����ظ߷�: pmc.QuotaPeakNonPagedPoolUsage
				//ʹ�÷Ƿ�ҳ�����: pmc.QuotaNonPagedPoolUsage
				//ʹ�÷�ҳ�ļ�:pmc.PagefileUsage
				//ʹ�÷�ҳ�ļ��ĸ߷�: pmc.PeakPagefileUsage
			}
		}
		/*else
		{
			DWORD dwMemProcess = 0;
			PSAPI_WORKING_SET_INFORMATION workSet;
			memset(&workSet, 0, sizeof(workSet));
			BOOL bOk = QueryWorkingSet(hProccess, &workSet, sizeof(workSet));
			if (bOk || (!bOk && GetLastError() == ERROR_BAD_LENGTH))
			{
				int nSize = sizeof(workSet.NumberOfEntries) + workSet.NumberOfEntries * sizeof(workSet.WorkingSetInfo);
				char* pBuf = new char[nSize];
				if (pBuf)
				{
					QueryWorkingSet(hProccess, pBuf, nSize);
					PSAPI_WORKING_SET_BLOCK* pFirst = (PSAPI_WORKING_SET_BLOCK*)(pBuf + sizeof(workSet.NumberOfEntries));
					DWORD dwMem = 0;
					for (ULONG_PTR nMemEntryCnt = 0; nMemEntryCnt < workSet.NumberOfEntries; nMemEntryCnt++, pFirst++)
					{
						if (pFirst->Shared == 0) dwMem += si.dwPageSize;
					}
					delete[] pBuf;
					pBuf = NULL;
					if(workSet.NumberOfEntries > 0)
					{
						dwMemProcess = dwMem;
						dwWorkingSetSize = dwMemProcess;
						dwPeakWorkingSetSize = dwMemProcess;
					}
				}
				else
				{
					delete[] pBuf;
					pBuf = NULL;

				}
			}
			else
			{
				return FALSE;
			}
		}*/
	}
	else
	{
		int ret = GetLastError();
		return FALSE;
	}
	return TRUE;
}

//��ȡ�����
BOOL ProcessInfoCollect::GetHandleCountEx(HANDLE hProccess, DWORD &dwHandles)
{
	return GetProcessHandleCount(hProccess, &dwHandles);
}

//��ȡ�߳���
BOOL	ProcessInfoCollect::GetThreadCount(DWORD &dwThreads)
{
	return GetThreadCount(GetCurrentProcessId(), dwThreads);
}
BOOL ProcessInfoCollect::GetThreadCount(DWORD lProcessID, DWORD &dwThreads)
{
	//��ȡ������Ϣ
	HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
		return FALSE;

	BOOL bFind = FALSE;
	char szFilePath[MAX_PATH] = { 0 };
	PROCESSENTRY32 stProcessEntry32 = { 0 };
	stProcessEntry32.dwSize = sizeof(stProcessEntry32);
	BOOL bSucceed = ::Process32First(hProcessSnap, &stProcessEntry32);;
	for (;;)
	{
		if (!bSucceed)
			break;

		if (stProcessEntry32.th32ProcessID == lProcessID)
		{
			dwThreads = stProcessEntry32.cntThreads;
			bFind = TRUE;
			break;
		}
		bSucceed = ::Process32Next(hProcessSnap, &stProcessEntry32);
	}
	::CloseHandle(hProcessSnap);
	return bFind;
}

BOOL ProcessInfoCollect::EnablePrivilege(HANDLE hToken, LPCSTR szPrivName)
{
	TOKEN_PRIVILEGES tkp;//��������Ȩ�޽ṹ����
	LUID  luid;//����Ψһ��ʶ���ṹ����

	//��ѯSE_DEBUG_NAMEȨ������Ӧ��luidֵ
	if (!LookupPrivilegeValue(NULL, szPrivName, &luid))
	{
		//	TRACE("Lookup Privilege Value Failed...\nErrorCode:%d\n",GetLastError());
		return 0;
	}

	//���Token_Privileges�ṹ
	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Luid = luid;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	//����Ȩ��
	if (!AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof(tkp), NULL, NULL))
	{
		//		TRACE("Adjust Token Privileges Failed...\nErrorCode:%d\n",GetLastError());
		return 0;
	}

	return(GetLastError() == ERROR_SUCCESS);
}

BOOL ProcessInfoCollect::EnumAllProcess(ProcessInfo *pAll, int & nTotal)
{
	ENUMPROCESS         pEnumProcess;
	ENUMPROCESSMODULES  pEnumProcessModules;
	GETMODULEFILENAMEEX pGetModuleFileNameEx;
	GETMODULEBASENAME   pGetModuleBaseName;

	DWORD dwMajorVersion = 0;
	DWORD dwMinorVersion = 0;
	OSVERSIONINFOEX osver = { 0 };
	osver.dwOSVersionInfoSize = sizeof(osver);
	::GetVersionEx((OSVERSIONINFO*)&osver);
	dwMajorVersion = osver.dwMajorVersion;
	dwMinorVersion = osver.dwMinorVersion;

	HANDLE hToken = NULL;//�������ƾ��

	DWORD dwPID[MAX_ID] = { 0 };//���ڽ���ID������
	DWORD cbReturn = 0;//���ص��ֽ���
	DWORD dwPCount = 0;//������
	DWORD i;
	char  szFileName[MAX_PATH] = { 0 };//�ļ���
	char  szPathName[MAX_PATH] = { 0 };//·����
	char  Id[] = "ID", Pid[] = "PID", Exe[] = "ProcessName", Path[] = "Path";

	HANDLE hProcess = NULL; //���̾��
	HMODULE hModule = NULL;
	HINSTANCE hPsDll = NULL;//ʵ�����

	hPsDll = LoadLibrary("PSAPI.DLL");//���DLL��ʵ��
	if (hPsDll == NULL)
	{
		//FreeLibrary(hPsDll);
		return FALSE;
	}

	//��ú����ĵ�ַ
	pEnumProcess = (ENUMPROCESS)GetProcAddress(hPsDll, "EnumProcesses");
	pEnumProcessModules = (ENUMPROCESSMODULES)GetProcAddress(hPsDll, "EnumProcessModules");
	pGetModuleFileNameEx = (GETMODULEFILENAMEEX)GetProcAddress(hPsDll, "GetModuleFileNameExA");
	pGetModuleBaseName = (GETMODULEBASENAME)GetProcAddress(hPsDll, "GetModuleBaseNameA");

	if (!(pEnumProcess && pEnumProcessModules && pGetModuleFileNameEx &&pGetModuleBaseName))
	{
		FreeLibrary(hPsDll);
		return FALSE;
	}

	//�򿪵�ǰ���̷�������
	if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken))
	{
		if (EnablePrivilege(hToken, SE_DEBUG_NAME))
		{
			pEnumProcess(dwPID, sizeof(dwPID), &cbReturn);//ö�ٽ���
			dwPCount = cbReturn / sizeof(DWORD);//�������/�߳�����
			if (dwPCount > nTotal)
			{
				dwPCount = nTotal;
			}
			else
			{
				nTotal = dwPCount;
			}
			for (i = 0; i < dwPCount; i++)
			{
				//�򿪽���
				hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,//����Ȩ��
					false,//�Ƿ�����õ��Ľ��̾�����󴴽����ӽ��̼̳�
					dwPID[i]);//����ID
				pAll[i].dwPID = dwPID[i];
				if (hProcess)
				{
					//ö�ٽ���ģ��
					DWORD dwError = 0;
					BOOL bSuccessProcess = pEnumProcessModules(hProcess, &hModule, sizeof(hModule), &cbReturn);
					if (!bSuccessProcess)
					{
						dwError = GetLastError();
					}
					memset(pAll[i].szFileName, 0, sizeof(char)*MAX_PATH);
					memset(pAll[i].szPathName, 0, sizeof(char)*MAX_PATH);
					memset(szFileName, 0, sizeof(szFileName));
					memset(szPathName, 0, sizeof(szPathName));
					if (hModule != NULL)
					{
						//��ý���ģ���ļ���(����·��)
						if (dwMajorVersion < 5)		//< window2000
						{
							//�����ļ���
							//DWORD dwGetModuleBaseName = pGetModuleFileNameEx(hProcess, hModule, szPathName, sizeof(szPathName));
							DWORD dwGetModuleBaseName = GetModuleFileNameEx(hProcess, hModule, szPathName, sizeof(szPathName));
							GetFileNameWithoutExtendName(szPathName, pAll[i].szFileName);
						}
						if (dwMajorVersion == 5)  //xp��Windows Server2003
						{
							GetProcessImageFileName(hProcess, szPathName, sizeof(szPathName));
							GetFileNameWithoutExtendName(szPathName, pAll[i].szFileName);
						}
						else if (osver.dwMajorVersion >= 6)		//win7��win7����
						{
							DWORD dwPathNameSize = sizeof(szPathName);
							bSuccessProcess = QueryFullProcessImageName(hProcess, 0, szPathName, &dwPathNameSize);
							GetFileNameWithoutExtendName(szPathName, pAll[i].szFileName);
						}
						if (!bSuccessProcess)
						{
							dwError = GetLastError();
						}
						strcpy(pAll[i].szPathName, szPathName);
					}
				}
				if (hProcess)
					CloseHandle(hProcess);
				//if(hModule)
				//	CloseHandle(hModule);
			}
		}
	}

	//�رվ����ʵ��
	if (hToken)
		CloseHandle(hToken);
	FreeLibrary(hPsDll);

	return TRUE;
}

/*
����˵������ȡָ�����̵������ӽ���
����˵����dwParentPID��������ID;m_pChildProcess,[out] �ӽ����б�nChildProcessNum��[out]�ӽ�������
����ֵ���ӽ���ID�б�
*/
BOOL ProcessInfoCollect::GetChildProcessID(DWORD dwParentPID, DWORD* pChildProcess, int & nChildProcessNum)
{
	//��ȡ������Ϣ
	HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
		return FALSE;

	/*DWORD dwProcessID = 0;*/
	PROCESSENTRY32 stProcessEntry32 = { 0 };
	stProcessEntry32.dwSize = sizeof(stProcessEntry32);
	BOOL bSucceed = ::Process32First(hProcessSnap, &stProcessEntry32);;

	int  i = 0;
	for (;;)
	{
		if (!bSucceed)
			break;

		bSucceed = ::Process32Next(hProcessSnap, &stProcessEntry32);

		if (stProcessEntry32.th32ParentProcessID == dwParentPID)
		{
			if (i < MAX_CHILD_PROCESS_COUNT)
			{
				pChildProcess[i++] = stProcessEntry32.th32ProcessID;
			}
			else
			{
				break;
			}
		}
	}
	::CloseHandle(hProcessSnap);
	nChildProcessNum = i;

	return TRUE;
}

BOOL   CALLBACK   CloseWindowByPID(HWND  hwnd, LPARAM   lParam)
{
	BOOL bRet = FALSE;
	DWORD   dwID;

	GetWindowThreadProcessId(hwnd, &dwID);

	if (dwID == (DWORD)lParam)
	{
		if (PostMessage(hwnd, WM_CLOSE, 0, 0))
		{
			bRet = TRUE;
		}
	}

	return   TRUE;
}

//��������
BOOL ProcessInfoCollect::TerminateProcessEx(DWORD   dwPID, DWORD  dwTimeout)
{
	HANDLE       hProc;
	BOOL       bRet = TRUE;

	//   If   we   can 't   open   the   process   with   PROCESS_TERMINATE   rights,
	//   then   we   give   up   immediately.
	hProc = OpenProcess(SYNCHRONIZE | PROCESS_TERMINATE, FALSE,
		dwPID);

	if (hProc == NULL)
	{
		return   FALSE;
	}

	//   WindowEnumCallBack()   posts   WM_CLOSE   to   all   windows   whose   PID
	//   matches  dwPID.
	EnumWindows((WNDENUMPROC)CloseWindowByPID, (LPARAM)dwPID);

	//   Wait   on   the   handle.   If   it   signals,   great.   If   it   times   out,
	//   then   you   kill   it.
	if (WaitForSingleObject(hProc, dwTimeout) != WAIT_OBJECT_0)
	{
		bRet = TerminateProcess(hProc, 0);
		WaitForSingleObject(hProc, dwTimeout);
	}

	CloseHandle(hProc);
	return   bRet;
}

//��ȡ������������
HANDLE ProcessInfoCollect::GetProcessToken(DWORD dwPID)
{
	HANDLE       hProc;
	HANDLE       hToken = NULL;

	//   If   we   can 't   open   the   process   with   PROCESS_TERMINATE   rights,
	//   then   we   give   up   immediately. if(!OpenProcessToken(hProc,TOKEN_QUERY|TOKEN_QUERY_SOURCE, &hToken))
	hProc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE,
		dwPID);

	if (hProc != NULL)
	{
		if (!OpenProcessToken(hProc, TOKEN_ALL_ACCESS, &hToken))
		{
			DWORD dwRet = GetLastError();
		}
		CloseHandle(hProc);
	}

	return hToken;

}

//�����½���
BOOL ProcessInfoCollect::CreateNewProcess(LPCSTR pszExeName, WORD wShowWindow, HANDLE &hToken, DWORD & dwProcessID)
{
	if (hToken == NULL)
	{
		BOOL bRet = StartProcess(pszExeName, dwProcessID, wShowWindow);
		if (bRet)
		{
			HANDLE hNewToken = GetProcessToken(dwProcessID);
			if (hToken != NULL)
				CloseHandle(hToken);

			hToken = hNewToken;
		}
		return bRet;
	}
	else
	{
		HANDLE hNewToken = StartProcessBySysService(pszExeName, "", hToken);
		if (hToken != NULL)
			CloseHandle(hToken);

		hToken = hNewToken;
		return TRUE;
	}
}

//���ݳ�������ȡ����ID
DWORD ProcessInfoCollect::GetProcessIDByName(const char* pAppFileName)
{
	//��ȡ������Ϣ
	HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
		return 0;

	DWORD dwProcessID = 0;
	PROCESSENTRY32 stProcessEntry32 = { 0 };
	stProcessEntry32.dwSize = sizeof(stProcessEntry32);
	BOOL bSucceed = ::Process32First(hProcessSnap, &stProcessEntry32);;
	for (;;)
	{
		if (!bSucceed)
			break;

		bSucceed = ::Process32Next(hProcessSnap, &stProcessEntry32);
		if (::_stricmp(stProcessEntry32.szExeFile, pAppFileName) == 0)
		{
			dwProcessID = stProcessEntry32.th32ProcessID;
			break;
		}
	}
	::CloseHandle(hProcessSnap);
	return dwProcessID;
}

//�رս���
int ProcessInfoCollect::KillProcess(DWORD nProcessID)
{
	HANDLE hProcessHandle = NULL;
	hProcessHandle = ::OpenProcess(PROCESS_TERMINATE, FALSE, nProcessID);
	if (hProcessHandle == NULL)
		return 0;

	//�ر��ӽ���
	DWORD *pChildProcess = new DWORD[MAX_CHILD_PROCESS_COUNT];
	memset(pChildProcess, 0, MAX_CHILD_PROCESS_COUNT);
	int nChildProcessNum = 0;
	BOOL bRet = GetChildProcessID(nProcessID, pChildProcess, nChildProcessNum);
	if (bRet)
	{
		for (int i = 0; i < nChildProcessNum; ++i)
		{
			if (pChildProcess[i] > 0)
			{
				TerminateProcessEx(pChildProcess[i], 200);
			}
		}
	}
	if (pChildProcess != NULL)
	{
		delete[]pChildProcess;
		pChildProcess = NULL;
	}

	bRet = ::TerminateProcess(hProcessHandle, 200);
	if (hProcessHandle != NULL)
		CloseHandle(hProcessHandle);

	return bRet;
}

//��������

BOOL ProcessInfoCollect::StartProcess(LPCSTR pAppFullFileName, DWORD &dwProcessID, WORD wShowWindow)
{
	BOOL bRet = FALSE;
	STARTUPINFO   stStartupInformation = { 0 };
	stStartupInformation.cb = sizeof(stStartupInformation);
	stStartupInformation.dwFlags = STARTF_USESHOWWINDOW;
	stStartupInformation.wShowWindow = wShowWindow;//SW_SHOW;

	//stStartupInformation.lpDesktop="WinSta0\\Winlogon";

	stStartupInformation.lpDesktop = "winsta0\\default";

	PROCESS_INFORMATION   stProcessInformation = { 0 };
	//Ŀ¼����·��
	char szProgrammeWorkPath[MAX_PATH] = { 0 };
	::strcpy_s(szProgrammeWorkPath, MAX_PATH - 1, pAppFullFileName);//+1,��c:\..��ʼ
	if (::strchr(szProgrammeWorkPath, ':') != NULL &&
		::strrchr(szProgrammeWorkPath, '\\') != NULL)
	{
		(_tcsrchr(szProgrammeWorkPath, '\\'))[1] = 0;
	}

	bRet = ::CreateProcess((LPTSTR)pAppFullFileName, NULL, NULL, NULL, FALSE, 0, NULL, szProgrammeWorkPath,
		&stStartupInformation, &stProcessInformation);

	dwProcessID = stProcessInformation.dwProcessId;
	if (stProcessInformation.hProcess != NULL)
		CloseHandle(stProcessInformation.hProcess);

	if (stProcessInformation.dwThreadId != NULL)
		CloseHandle(stProcessInformation.hThread);

	return bRet;
}

//��������(����)
HANDLE ProcessInfoCollect::StartProcessBySysService(const char* pAppFullFileName, const char* pRunParameter, HANDLE hToken)
{
	HANDLE hNewToken = NULL;

	char szSoftWare[1024] = { 0 };
	sprintf_s(szSoftWare, 1024, "\"%s\" %s", pAppFullFileName, pRunParameter);

	STARTUPINFO   stStartupInformation = { 0 };
	stStartupInformation.cb = sizeof(stStartupInformation);
	stStartupInformation.dwFlags = STARTF_USESHOWWINDOW;
	stStartupInformation.wShowWindow = SW_SHOW;
	stStartupInformation.lpDesktop = "winsta0\\default";

	PROCESS_INFORMATION   stProcessInformation = { 0 };

	//Ŀ¼����·��
	char szProgrammeWorkPath[MAX_PATH] = { 0 };
	::strcpy_s(szProgrammeWorkPath, MAX_PATH - 1, pAppFullFileName);//+1,��c:\..��ʼ
	if (::strchr(szProgrammeWorkPath, ':') != NULL &&
		::strrchr(szProgrammeWorkPath, '\\') != NULL)
	{
		(_tcsrchr(szProgrammeWorkPath, '\\'))[1] = 0;
	}


	if (DuplicateTokenEx(hToken, MAXIMUM_ALLOWED, NULL,
		SecurityImpersonation, TokenPrimary, &hNewToken))
	{
		if (CreateProcessAsUser(hNewToken, NULL,
			szSoftWare, NULL, NULL, FALSE,
			0, NULL, szProgrammeWorkPath, &stStartupInformation, &stProcessInformation))
		{
			if (stProcessInformation.hProcess != NULL)
				CloseHandle(stProcessInformation.hProcess);
			if (stProcessInformation.dwThreadId != NULL)
				CloseHandle(stProcessInformation.hThread);
		}
	}
	return hNewToken;
}

#ifdef StartProcessBySysServiceEx_Flag
BOOL	ProcessInfoCollect::StartProcessBySysServiceEx(const char* pAppFullFileName, const char* pRunParameter)
{
	DWORD dwProcesses = 0;
	BOOL bResult = FALSE;
	char szSoftWare[1024] = { 0 };
	sprintf_s(szSoftWare, 1024, _T("\"%s\" %s"), pAppFullFileName, pRunParameter);

	DWORD dwSid = GetActiveSessionID();
	DWORD dwRet = 0;
	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	HANDLE hProcess = NULL, hPToken = NULL, hUserTokenDup = NULL;
	if (!WTSQueryUserToken(dwSid, &hPToken))
	{
		PROCESSENTRY32 procEntry;
		DWORD dwPid = 0;
		HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (hSnap == INVALID_HANDLE_VALUE)
		{
			return FALSE;
		}

		procEntry.dwSize = sizeof(PROCESSENTRY32);
		if (Process32First(hSnap, &procEntry))
		{
			do
			{
				if (_tcsicmp(procEntry.szExeFile, _T("explorer.exe")) == 0)
				{
					DWORD exeSessionId = 0;
					if (ProcessIdToSessionId(procEntry.th32ProcessID, &exeSessionId) && exeSessionId == dwSid)
					{
						dwPid = procEntry.th32ProcessID;
						break;
					}
				}

			} while (Process32Next(hSnap, &procEntry));
		}
		CloseHandle(hSnap);

		// explorer���̲�����
		if (dwPid == 0)
		{
			return FALSE;
		}

		hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwPid);
		if (hProcess == NULL)
		{
			return FALSE;
		}

		if (!::OpenProcessToken(hProcess, TOKEN_ALL_ACCESS_P, &hPToken))
		{
			CloseHandle(hProcess);
			return FALSE;
		}
	}

	if (hPToken == NULL)
		return FALSE;

	TOKEN_LINKED_TOKEN admin;
	bResult = GetTokenInformation(hPToken, (TOKEN_INFORMATION_CLASS)TokenLinkedToken, &admin, sizeof(TOKEN_LINKED_TOKEN), &dwRet);

	if (!bResult) // vista ��ǰ�汾��֧��TokenLinkedToken
	{
		TOKEN_PRIVILEGES tp;
		LUID luid;
		if (LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid))
		{
			tp.PrivilegeCount = 1;
			tp.Privileges[0].Luid = luid;
			tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
		}
		//����token
		DuplicateTokenEx(hPToken, MAXIMUM_ALLOWED, NULL, SecurityIdentification, TokenPrimary, &hUserTokenDup);
	}
	else
	{
		hUserTokenDup = admin.LinkedToken;
	}

	LPVOID pEnv = NULL;
	DWORD dwCreationFlags = CREATE_PRESERVE_CODE_AUTHZ_LEVEL;

	// hUserTokenDupΪ��ǰ��½�û�������
	if (CreateEnvironmentBlock(&pEnv, hUserTokenDup, TRUE))
	{
		//��������˻�������������CreateProcessAsUser��
		//dwCreationFlags������Ҫ����CREATE_UNICODE_ENVIRONMENT��
		//����MSDN��ȷ˵����
		dwCreationFlags |= CREATE_UNICODE_ENVIRONMENT;
	}
	else
	{
		//������������ʧ����Ȼ���Դ������̣�
		//����Ӱ�쵽����Ľ��̻�ȡ������������
		pEnv = NULL;
	}

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;
	ZeroMemory(&pi, sizeof(pi));
	//USES_CONVERSION;
	bResult = CreateProcessAsUser(
		hUserTokenDup,                     // client's access token
		NULL,    // file to execute
		(LPTSTR)A2T(szSoftWare),                 // command line
		NULL,            // pointer to process SECURITY_ATTRIBUTES
		NULL,               // pointer to thread SECURITY_ATTRIBUTES
		FALSE,              // handles are not inheritable
		dwCreationFlags,     // creation flags
		pEnv,               // pointer to new environment block
		NULL,               // name of current directory
		&si,               // pointer to STARTUPINFO structure
		&pi                // receives information about new process
		);


	if (pi.hProcess)
	{
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
	}

	if (hUserTokenDup != NULL)
		CloseHandle(hUserTokenDup);
	if (hProcess != NULL)
		CloseHandle(hProcess);
	if (hPToken != NULL)
		CloseHandle(hPToken);
	if (pEnv != NULL)
		DestroyEnvironmentBlock(pEnv);

	return TRUE;
}

#endif