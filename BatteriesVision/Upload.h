#pragma once
void T_ServerSocket();
void T_ClientSocket();
void T_ServerRecvData();
void T_ClientRecvData();
struct tagTSENDDATA
{
	//double
};
//#define MAX_CONNECT 1
typedef struct tagTUploadParam
{
	BOOL	bUseDataUpload;					//是否使用网络数据交互
	CString strAddressListen;				//作为服务器，要连接的客户端IP
	int     nPortListen;					//作为服务器，要连接的客户端端口
	BOOL    bListenClient;					//是否监听客户端
	CString strAddresConnect;				//作为客户端要连接的服务器IP
	int nPortConnect;						//作为客户端要连接的服务器端口

	BOOL	bUseCompensationAuto;			//是否使用自动补偿
	double	dStartIgnoreMeter;				//开机忽略米数
	double	dToNextStationMeter;			//到下一个工位的距离(m)
	double  dCompensationInvertal;			//补偿间隔距离(m)
	double  dScale;							//在线补偿系数，每次发送的值乘以多少
	BOOL	bCompensationDataInverse;		//数据是否反向补偿
}UploadParam;

class CUpload : public CBaseTmpl//上传给服务器端口
{
public:
	CUpload();
	~CUpload();

public:
	BOOL Init();
	void Action(const C_MergeWork& C);
	BOOL ReadParam();
	BOOL WriteParam();

	//发送数据给下个工位
	void AddInfoToClient(HTuple hvInfo);
	//发送数据给上个工位
	void AddInfoToServer(HTuple hvInfo);

	//发送卷号给后工位
	void SendBatchToClient(CString strBatch);
	//接收卷号
	void RecvBatchFromServer(CString strBatch);

	//后工位发送干湿膜补偿数据给前工位
	void SendBias(HTuple hvValue, int nImageSN);
	//接收补偿数据
	void RecvBias(HTuple hvValue);

	//判断连接状态
	BOOL IsNetConnect();
	//接收膜卷号
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
	HTuple m_ServerSocket;				//作为服务器时候采用
	HTuple m_ClientSocket;				//作为客户端时候采用
	//作为客户端的时候使用
	HTuple m_Socket;
	BOOL   m_bConnect;					//客户端连接服务器是否成功
	BOOL   m_bListenSuccess;			//服务器是否开启监听
	BOOL   m_bConnectClient;			//服务器是否连接客户端成功
	BOOL   m_bSendBatchOK;				//服务器发送卷号成功
	BOOL   m_bSendCheckProductInfo;		//服务器校验客户端产品规格是否一致

	HTuple m_hvQueue;
	UploadParam m_tParam;
	HTuple m_hvSeconds, m_hvFirstSend;
	
	HTuple m_hvBiasRecv;					//接收数据值
	HTuple m_hvCompensation;				//计算出来的补偿值
	HTupleVector m_hvecCompen;				//最近的在线补偿值
	HTupleVector m_vecDataCollect;			//收集的数据
	HTupleVector m_vecRawData;				//延迟到下一个工位收集的数据，保持头部和机尾能对应上，用于计算补偿值
	BOOL	m_bAutoCompensationDisable;		//是否当前自动补偿禁用
	BOOL m_bBiasRecv;                                    //接收数据是否一样

	HTuple m_hvProductInfo;					//产品信息，服务器用于发送，客户端用于接收和校验

	static HTupleVector m_hvecInfoSendToClient;		//信息队列，发送信息到客户端
	static HTupleVector m_hvecInfoSendToServer;		//信息队列，发送信息到服务器

	HTuple		 m_hvSendToClientFailCount;		//发送失败计数
	HTuple		 m_hvSendToServerFailCount;		//发送失败计数

private:
	CString	m_strBatchRecv;					//接收的卷号
};

