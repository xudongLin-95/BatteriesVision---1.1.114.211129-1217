#pragma once

//ö�ٽ���
typedef BOOL(_stdcall *ENUMPROCESS)(
	DWORD *pProcessIds,    //ָ�����ID������
	DWORD cb,              //ID����Ĵ�С�����ֽڼ���
	DWORD *pBytesReturned  //���ص��ֽ�
	);

//ö�ٽ���ģ��
typedef BOOL(_stdcall *ENUMPROCESSMODULES)(
	HANDLE  hProcess,   //���̾��
	HMODULE *lphModule, //ָ��ģ����������
	DWORD   cb,         //ģ���������С���ֽڼ���
	LPDWORD lpcbNeeded  //�洢����ģ����������ֽ���
	);

//��ý���ģ����
typedef DWORD(_stdcall *GETMODULEFILENAMEEX)(
	HANDLE  hProcess,   //���̾��
	HMODULE hModule,    //���̾��
	LPTSTR  lpFilename, //���ģ��ȫ·����
	DWORD   nSize       //lpFilename��������С���ַ�����
	);

//��ý�����
typedef DWORD(_stdcall *GETMODULEBASENAME)(
	HANDLE  hProcess,  //���̾��
	HMODULE hModule,   //ģ����
	LPTSTR  lpBaseName,//��Ž�����
	DWORD   nSize      //lpBaseName��������С
	);

//������Ϣ�ṹ
typedef struct tagProcessInfo 
{
	DWORD dwPID;//����ID
	char  szFileName[MAX_PATH];//�����ļ���
	char  szPathName[MAX_PATH];//����·����
}ProcessInfo;

class ProcessInfoCollect
{
public:
	ProcessInfoCollect();
	virtual ~ProcessInfoCollect();

	//��Ȩ
	BOOL	EnablePrivilege(HANDLE hToken, LPCSTR szPrivName);

	//ö�����н���
	BOOL	EnumAllProcess(ProcessInfo *pAll, int & nTotal);
	//��������
	BOOL	CreateNewProcess(LPCSTR pszExeName, WORD wShowWindow, HANDLE &hToken, DWORD & dwProcessID);
	//�رս���
	int		KillProcess(DWORD nProcessID);

	//��ȡָ�����̵������ӽ���
	BOOL	GetChildProcessID(DWORD dwParentPID, DWORD* pChildProcess, int & nChildProcessNum);
	//��ȡ������������
	HANDLE	GetProcessToken(DWORD dwPID);
	//���ݳ�������ȡ����ID
	DWORD	GetProcessIDByName(const char* pAppFileName);	   

	//��ȡ��ǰ���̵�cpuʹ����
	BOOL    GetCPUUserRate(double & dCPUUserRate);
	//��ȡָ�����̵�cpuʹ����
	BOOL    GetCPUUserRate(DWORD lProcessID, double & dCPUUserRate);

	//��ȡ��ǰ���̵�IO����
	int		GetIOBytes(ULONGLONG * read_bytes, ULONGLONG * write_bytes, ULONGLONG * wct, ULONGLONG * rct);
	//��ȡָ�����̵�IO����
	int		GetIOBytes(DWORD lProcessID, ULONGLONG * read_bytes, ULONGLONG * write_bytes, ULONGLONG * wct, ULONGLONG * rct);

	//��ȡ��ǰ���̵��ڴ�
	BOOL	GetMemoryUsed(DWORD & dwPeakWorkingSetSize, DWORD & dwWorkingSetSize);
	//��ȡָ�����̵��ڴ�
	BOOL	GetMemoryUsed(DWORD lProcessID, DWORD & dwPeakWorkingSetSize, DWORD & dwWorkingSetSize);

	//��ȡ�����
	BOOL	GetHandleCount(DWORD &dwHandles);
	BOOL	GetHandleCount(DWORD lProcessID, DWORD &dwHandles);

	//��ȡ�߳���
	BOOL	GetThreadCount(DWORD &dwThreads);
	BOOL	GetThreadCount(DWORD lProcessID, DWORD &dwThreads);

#ifdef StartProcessBySysServiceEx_Flag
	//��������(ͨ���������ý���)
	BOOL	StartProcessBySysServiceEx(const char* pAppFullFileName, const char* pRunParameter);
#endif

protected:
	//��ȡָ�����̵�cpuʹ����
	BOOL    GetCPUUserRateEx(HANDLE hProccess, double & dCPUUserRate);

	//��ȡָ�����̵�IO����
	int		GetIOBytesEx(HANDLE hProccess, ULONGLONG * read_bytes, ULONGLONG * write_bytes, ULONGLONG * wct, ULONGLONG * rct);

	//��ȡ�ڴ�
	//������hProccess�����̾����dwPeakWorkingSetSize:ʹ���ڴ�߷�;dwWorkingSetSize:��ǰʹ�õ��ڴ�;
	BOOL	GetMemoryUsedEx(HANDLE hProccess, DWORD & dwPeakWorkingSetSize, DWORD & dwWorkingSetSize);

	//��ȡ�����
	BOOL	GetHandleCountEx(HANDLE hProccess, DWORD &dwHandles);

	//�жϽ���
	BOOL	TerminateProcessEx(DWORD   dwPID, DWORD  dwTimeout);
	//��������
	BOOL	StartProcess(LPCSTR pAppFullFileName, DWORD &dwProcessID, WORD wShowWindow);

	//��������(ͨ���������ý���)
	HANDLE	StartProcessBySysService(const char* pAppFullFileName, const char* pRunParameter, HANDLE hToken);
};
