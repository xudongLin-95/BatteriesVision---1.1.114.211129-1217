#pragma once
#include "DHInterFace.h"
#include "SerialPortExt.h"
#include "Log.in.h"
#include "UpsControl.in.h"

class CUpsSerial:DHInterface
{
public:
	typedef struct tagTParam
	{
		//eUpsControlType	eUpsType; //UPS���� ��ʱ����
		CString				nComName;   //��������
		int                 nComIndex;  //�����±�
	};
public:
	CUpsSerial();
	~CUpsSerial();
public:
	virtual int DoSomething(void* message, void* wParam, void* lParam);

	int Init(UPSINIT &Init);

	BOOL IsOpen();
	BOOL Open();  //�򿪹�Դ����������
	BOOL Close(); //�رչ�Դ����������
	BOOL SetUpsConnect(CString &recv);//UPS��������
	BOOL GetUpsStates(CString &state);//��ȡUPS�Է�������Ϣ

	BOOL SaveParams();//��������ӿ�
	BOOL ReadParams();//��ȡ�����ӿ�

	CUpsSerial::tagTParam& GetLightParam() { return m_tUpsParam; }

	void GetUpsAlram();//��ȡ�Զ�������״̬
	static UINT ThreadUpsState(LPVOID lpVoid); //UPSѭ������Զ��쳣��Ϣ �̺߳���

	CString GetUpsAlarmResult() { return UpsAlarmMsg; }//��ȡ�̼߳������Ϣ

	BOOL Log(const CString& strLog, const int& nType, const int&nExcpLevel = NoExcp);
private:
    static CString UpsAlarmMsg;
	
public:
	static CRITICAL_SECTION m_csLock;
	BOOL ShowParamDlg();

public:
	
	UPSINIT m_sInit;
	CString m_strConfigUpsSerialFile;	
	tagTParam m_tUpsParam;
	int m_checkUpsStatus;
};

extern CUpsSerial* g_This;

