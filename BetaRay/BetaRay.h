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
		eBetaRayType	eType;			//测厚仪的类型
		BOOL			bUseComm;		//是否使用通信
		CString			strIP;			//测厚仪IP
		int				nPort;			//测厚仪端口
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
	//通信线程
	static void* HThreadComm(void *hcthread);
	//通信线程
	static void T_Comm();


	//生成获取重量数据的报文
	HTuple GenQueryBetaRayMMDataMsg();
	//生成获取配方信息/操作单的报文
	HTuple GenQueryBetaRayPdInfoMsg();

	HTuple _GetCRC32T4Byte(HTuple hvInfo);

	BOOL   _GetMMDataFromRecv(char* buff, int nLength);
	BOOL   _GetPdInfoFromRecv(char* buff, int nLength);
public:
	BETARAYINIT m_sInit;
	static CRITICAL_SECTION m_csLock;
	tagTParam m_tParam;

	BetaRayPdInfo m_tBetaRayPdInfo;	//测厚仪品种信息
	HTupleVector m_hvecBetaRayDatas;//重量数据，每个涂布一组数据
	HTupleVector m_hvecBetaRayDatasX;//重量数据坐标轴，每个涂布一组数据
private:
	CString m_strConfigFile;
	SOCKET		m_socket;

	char m_chQueryMMDataMsg[100];
	int	 m_nQueryMMDataMsgLength;
	char m_chQueryPdInfoMsg[100];
	int	 m_nQueryPdInfoMsgLength;

	BOOL m_bExitThread;//程序是否退出
	BOOL m_bConnect; //通信是否连接
};

extern CBetaRay* g_This;

