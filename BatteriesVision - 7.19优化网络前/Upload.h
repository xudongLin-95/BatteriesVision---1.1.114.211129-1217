#pragma once
void T_ServerSocket();
void T_ClientSocket();
void T_ServerRecvData();
void T_ClientRecvData();
struct tagTSENDDATA
{
	//double
};
#define MAX_CONNECT 2
typedef struct tagTUploadParam
{
	BOOL	bUseDataUpload;					//是否使用网络数据交互
	CString strAddressListen[MAX_CONNECT];	//作为服务器，要连接的客户端IP
	int     nPortListen[MAX_CONNECT];		//作为服务器，要连接的客户端端口
	BOOL    bListenClient[MAX_CONNECT];			//是否监听客户端
	CString strAddresConnect;				//作为客户端要连接的服务器IP
	int nPortConnect;						//作为客户端要连接的服务器端口

	BOOL	bUseCompensationAuto;			//是否使用自动补偿
	double	dStartIgnoreMeter;				//开机忽略米数
	double	dToNextStationMeter;			//到下一个工位的距离(m)
	double  dCompensationInvertal;			//补偿间隔距离(m)
	double  dScale;//在线补偿系数，每次发送的值乘以多少
	BOOL	bCompensationDataInverse;		//数据是否反向补偿
}UploadParam;

class CUpload : public CBaseTmpl//上传给服务器端口
{
public:
	CUpload();
	~CUpload();

public:
	static void* HThreadUpload(void *hcthread);//io线程
	void T_Upload(HTuple hvQueue);
	BOOL Init();
	void Upload(C_MergeWork C);
	void Action(const C_MergeWork& C);
	BOOL ReadParam();
	BOOL WriteParam();

	void SendBatchToClient(CString strBatch);
	void RecvBatchFromServer(CString strBatch);

	void JWSendBiasTo2(HTuple hvValue, int nImageSN);//机尾发送测量值给涂2
	void To2SendBiasJT(HTuple hvValue, int nImageSN);//涂2发送测量值给机头
	void RecvBias(HTuple hvValue);

	BOOL IsNetConnect();

	CString GetBatchRecv() { return m_strBatchRecv; }
	void SetBatchRecv(CString strName) { m_strBatchRecv = strName; }

	//写接收的补偿测量值
	void WriteBiasFile(HTuple hvBiasValue, int nImageSN);

	//监听设备刚开机禁用自动补偿
	void ListenStartRunIgnoreCompen();

	//服务器配方规格校验
	BOOL SendProductInfoToClient();
	//客户端配方校验
	BOOL CheckProductInfoFromServer();
	//获取客户端交互报警信息
	void RecvAlarmInfoFromClient(const HTuple& hvAlarmInfo, int nSN);
	//客户端发送报警信息
	BOOL SendAlarmInfoToServer(const CErrorInfo::tagErrorInfo& tInfo, const BOOL& bStopMachine=FALSE);
public:
	//作为服务器的时候使用
	HTuple m_ServerSocket[MAX_CONNECT];				//作为服务器时候采用
	HTuple m_ClientSocket[MAX_CONNECT];				//作为客户端时候采用
	//作为客户端的时候使用
	HTuple m_Socket;
	BOOL   m_bConnect;								//客户端连接服务器是否成功
	BOOL   m_bListenSuccess[MAX_CONNECT];			//服务器是否开启监听
	BOOL   m_bConnectClient[MAX_CONNECT];			//服务器是否连接客户端成功
	BOOL   m_bSendBatchOK[MAX_CONNECT];				//服务器发送卷号成功
	BOOL   m_bSendCheckProductInfo[MAX_CONNECT];	//服务器校验客户端产品规格是否一致

	HTuple m_hvQueue;
	UploadParam m_tParam;
	HTuple m_hvSeconds, m_hvFirstSend;
	
	HTuple m_hvBiasRecv;					//接收数据值
	HTuple m_hvCompensation;				//计算出来的补偿值
	HTupleVector m_vecDataCollect;			//收集的数据
	HTupleVector m_vecRawData;				//延迟到下一个工位收集的数据，保持头部和机尾能对应上，用于计算补偿值
	BOOL	m_bAutoCompensationDisable;		//是否当前自动补偿禁用

	HTuple m_hvProductInfo;					//产品信息，服务器用于发送，客户端用于接收和校验
private:
	CString	m_strBatchRecv;					//接收的卷号
};

