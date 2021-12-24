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
	BOOL	bUseDataUpload;					//�Ƿ�ʹ���������ݽ���
	CString strAddressListen;				//��Ϊ��������Ҫ���ӵĿͻ���IP
	int     nPortListen;					//��Ϊ��������Ҫ���ӵĿͻ��˶˿�
	BOOL    bListenClient;					//�Ƿ�����ͻ���
	CString strAddresConnect;				//��Ϊ�ͻ���Ҫ���ӵķ�����IP
	int nPortConnect;						//��Ϊ�ͻ���Ҫ���ӵķ������˿�

	BOOL	bUseCompensationAuto;			//�Ƿ�ʹ���Զ�����
	double	dStartIgnoreMeter;				//������������
	double	dToNextStationMeter;			//����һ����λ�ľ���(m)
	double  dCompensationInvertal;			//�����������(m)
	double  dScale;							//���߲���ϵ����ÿ�η��͵�ֵ���Զ���
	BOOL	bCompensationDataInverse;		//�����Ƿ��򲹳�
}UploadParam;

class CUpload : public CBaseTmpl//�ϴ����������˿�
{
public:
	CUpload();
	~CUpload();

public:
	BOOL Init();
	void Action(const C_MergeWork& C);
	BOOL ReadParam();
	BOOL WriteParam();

	//�������ݸ��¸���λ
	void AddInfoToClient(HTuple hvInfo);
	//�������ݸ��ϸ���λ
	void AddInfoToServer(HTuple hvInfo);

	//���;�Ÿ���λ
	void SendBatchToClient(CString strBatch);
	//���վ��
	void RecvBatchFromServer(CString strBatch);

	//��λ���͸�ʪĤ�������ݸ�ǰ��λ
	void SendBias(HTuple hvValue, int nImageSN);
	//���ղ�������
	void RecvBias(HTuple hvValue);

	//�ж�����״̬
	BOOL IsNetConnect();
	//����Ĥ���
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
	HTuple m_ServerSocket;				//��Ϊ������ʱ�����
	HTuple m_ClientSocket;				//��Ϊ�ͻ���ʱ�����
	//��Ϊ�ͻ��˵�ʱ��ʹ��
	HTuple m_Socket;
	BOOL   m_bConnect;					//�ͻ������ӷ������Ƿ�ɹ�
	BOOL   m_bListenSuccess;			//�������Ƿ�������
	BOOL   m_bConnectClient;			//�������Ƿ����ӿͻ��˳ɹ�
	BOOL   m_bSendBatchOK;				//���������;�ųɹ�
	BOOL   m_bSendCheckProductInfo;		//������У��ͻ��˲�Ʒ����Ƿ�һ��

	HTuple m_hvQueue;
	UploadParam m_tParam;
	HTuple m_hvSeconds, m_hvFirstSend;
	
	HTuple m_hvBiasRecv;					//��������ֵ
	HTuple m_hvCompensation;				//��������Ĳ���ֵ
	HTupleVector m_hvecCompen;				//��������߲���ֵ
	HTupleVector m_vecDataCollect;			//�ռ�������
	HTupleVector m_vecRawData;				//�ӳٵ���һ����λ�ռ������ݣ�����ͷ���ͻ�β�ܶ�Ӧ�ϣ����ڼ��㲹��ֵ
	BOOL	m_bAutoCompensationDisable;		//�Ƿ�ǰ�Զ���������
	BOOL m_bBiasRecv;                                    //���������Ƿ�һ��

	HTuple m_hvProductInfo;					//��Ʒ��Ϣ�����������ڷ��ͣ��ͻ������ڽ��պ�У��

	static HTupleVector m_hvecInfoSendToClient;		//��Ϣ���У�������Ϣ���ͻ���
	static HTupleVector m_hvecInfoSendToServer;		//��Ϣ���У�������Ϣ��������

	HTuple		 m_hvSendToClientFailCount;		//����ʧ�ܼ���
	HTuple		 m_hvSendToServerFailCount;		//����ʧ�ܼ���

private:
	CString	m_strBatchRecv;					//���յľ��
};

