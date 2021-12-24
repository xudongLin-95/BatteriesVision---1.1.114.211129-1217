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
	BOOL	bUseDataUpload;					//�Ƿ�ʹ���������ݽ���
	CString strAddressListen[MAX_CONNECT];	//��Ϊ��������Ҫ���ӵĿͻ���IP
	int     nPortListen[MAX_CONNECT];		//��Ϊ��������Ҫ���ӵĿͻ��˶˿�
	BOOL    bListenClient[MAX_CONNECT];			//�Ƿ�����ͻ���
	CString strAddresConnect;				//��Ϊ�ͻ���Ҫ���ӵķ�����IP
	int nPortConnect;						//��Ϊ�ͻ���Ҫ���ӵķ������˿�

	BOOL	bUseCompensationAuto;			//�Ƿ�ʹ���Զ�����
	double	dStartIgnoreMeter;				//������������
	double	dToNextStationMeter;			//����һ����λ�ľ���(m)
	double  dCompensationInvertal;			//�����������(m)
	double  dScale;//���߲���ϵ����ÿ�η��͵�ֵ���Զ���
	BOOL	bCompensationDataInverse;		//�����Ƿ��򲹳�
}UploadParam;

class CUpload : public CBaseTmpl//�ϴ����������˿�
{
public:
	CUpload();
	~CUpload();

public:
	static void* HThreadUpload(void *hcthread);//io�߳�
	void T_Upload(HTuple hvQueue);
	BOOL Init();
	void Upload(C_MergeWork C);
	void Action(const C_MergeWork& C);
	BOOL ReadParam();
	BOOL WriteParam();

	void SendBatchToClient(CString strBatch);
	void RecvBatchFromServer(CString strBatch);

	void JWSendBiasTo2(HTuple hvValue, int nImageSN);//��β���Ͳ���ֵ��Ϳ2
	void To2SendBiasJT(HTuple hvValue, int nImageSN);//Ϳ2���Ͳ���ֵ����ͷ
	void RecvBias(HTuple hvValue);

	BOOL IsNetConnect();

	CString GetBatchRecv() { return m_strBatchRecv; }
	void SetBatchRecv(CString strName) { m_strBatchRecv = strName; }

	//д���յĲ�������ֵ
	void WriteBiasFile(HTuple hvBiasValue, int nImageSN);

	//�����豸�տ��������Զ�����
	void ListenStartRunIgnoreCompen();

	//�������䷽���У��
	BOOL SendProductInfoToClient();
	//�ͻ����䷽У��
	BOOL CheckProductInfoFromServer();
	//��ȡ�ͻ��˽���������Ϣ
	void RecvAlarmInfoFromClient(const HTuple& hvAlarmInfo, int nSN);
	//�ͻ��˷��ͱ�����Ϣ
	BOOL SendAlarmInfoToServer(const CErrorInfo::tagErrorInfo& tInfo, const BOOL& bStopMachine=FALSE);
public:
	//��Ϊ��������ʱ��ʹ��
	HTuple m_ServerSocket[MAX_CONNECT];				//��Ϊ������ʱ�����
	HTuple m_ClientSocket[MAX_CONNECT];				//��Ϊ�ͻ���ʱ�����
	//��Ϊ�ͻ��˵�ʱ��ʹ��
	HTuple m_Socket;
	BOOL   m_bConnect;								//�ͻ������ӷ������Ƿ�ɹ�
	BOOL   m_bListenSuccess[MAX_CONNECT];			//�������Ƿ�������
	BOOL   m_bConnectClient[MAX_CONNECT];			//�������Ƿ����ӿͻ��˳ɹ�
	BOOL   m_bSendBatchOK[MAX_CONNECT];				//���������;�ųɹ�
	BOOL   m_bSendCheckProductInfo[MAX_CONNECT];	//������У��ͻ��˲�Ʒ����Ƿ�һ��

	HTuple m_hvQueue;
	UploadParam m_tParam;
	HTuple m_hvSeconds, m_hvFirstSend;
	
	HTuple m_hvBiasRecv;					//��������ֵ
	HTuple m_hvCompensation;				//��������Ĳ���ֵ
	HTupleVector m_vecDataCollect;			//�ռ�������
	HTupleVector m_vecRawData;				//�ӳٵ���һ����λ�ռ������ݣ�����ͷ���ͻ�β�ܶ�Ӧ�ϣ����ڼ��㲹��ֵ
	BOOL	m_bAutoCompensationDisable;		//�Ƿ�ǰ�Զ���������

	HTuple m_hvProductInfo;					//��Ʒ��Ϣ�����������ڷ��ͣ��ͻ������ڽ��պ�У��
private:
	CString	m_strBatchRecv;					//���յľ��
};

