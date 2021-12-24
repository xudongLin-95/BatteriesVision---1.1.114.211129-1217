#pragma once
#include "DHInterFace.h"
#include "Log.in.h"

#include "winsock2.h"
#pragma comment(lib, "ws2_32.lib")

class CBetaRay
{
public:
	struct tagTParam
	{
		eBetaRayType	eType;			//����ǵ�����
		BOOL			bUseComm;		//�Ƿ�ʹ��ͨ��
		CString			strIP;			//�����IP
		int				nPort;			//����Ƕ˿�
	};

public:
	CBetaRay();
	~CBetaRay();

	virtual int DoSomething(void* message, void* wParam, void* lParam);

	int Init(BETARAYINIT &Init);

	BOOL ShowParamDlg();
	BOOL ReadParam();
	BOOL SaveParam();

	BOOL IsOpen() { return m_bConnect; }
	BOOL Open(); 
	BOOL Close(); 
	BOOL Log(const CString& strLog, const int& nType, const int&nExcpLevel = NoExcp);

private:
	//ͨ���߳�
	static void* HThreadComm(void *hcthread);
	//ͨ���߳�
	static void T_Comm();


	//���ɻ�ȡ�������ݵı���
	HTuple GenQueryBetaRayMMDataMsg();
	//���ɻ�ȡ�䷽��Ϣ/�������ı���
	HTuple GenQueryBetaRayPdInfoMsg();

	HTuple _GetCRC32T4Byte(HTuple hvInfo);

	BOOL   _GetMMDataFromRecv(char* buff, int nLength);
	BOOL   _GetPdInfoFromRecv(char* buff, int nLength);
public:
	BETARAYINIT m_sInit;
	static CRITICAL_SECTION m_csLock;
	tagTParam m_tParam;

	BetaRayPdInfo m_tBetaRayPdInfo;	//�����Ʒ����Ϣ
	HTupleVector m_hvecBetaRayDatas;//�������ݣ�ÿ��Ϳ��һ������
	HTupleVector m_hvecBetaRayDatasX;//�������������ᣬÿ��Ϳ��һ������
private:
	CString m_strConfigFile;
	SOCKET		m_socket;

	char m_chQueryMMDataMsg[100];
	int	 m_nQueryMMDataMsgLength;
	char m_chQueryPdInfoMsg[100];
	int	 m_nQueryPdInfoMsgLength;

	BOOL m_bExitThread;//�����Ƿ��˳�
	BOOL m_bConnect; //ͨ���Ƿ�����
};

extern CBetaRay* g_This;

