#pragma once
#include "DHInterFace.h"
#include "Log.in.h"
#include "Temperature.in.h"

#include <string>
#include <math.h>
#include "modbus.h"
#include <errno.h>

#define ADDRESS_START 0         //���ԼĴ�������ʼ��ַ
#define ADDRESS_END 2           //���ԼĴ����Ľ�����ַ

class CControl
{
public:
	struct tagTParam
	{
		int nComIndex;    //ѡ�д����±�
		CString sComName; //��������
		int nUse;         //�Ƿ�ʹ�ø�ģ��
		int nTempNum;     //�¿�������
		int nIntervalTime;//���¶ȼ��ʱ��
		int nResponseTimeout;//��Ӧ��ʱʱ��
	};

public:
	CControl();
	~CControl();

	virtual int DoSomething(void* message, void* wParam, void* lParam);

	int Init(TEMPERATUREINIT &Init);

	BOOL ShowParamDlg();
	BOOL ReadParam();
	BOOL SaveParam();

	BOOL IsOpen() { return m_nCurrentStatus; }
	BOOL Open(); 
	BOOL Close(); 
	BOOL Log(const CString& strLog, const int& nType, const int&nExcpLevel = NoExcp);
	//modbus ��ȡ��ǰ�¶�
	BOOL GetTemp(std::vector<uint16_t>& vTemp);

	//tagTParam& GetTemperatureParam() { return m_tParam; }

	void GetCurrentTemperature();//��ȡ��ǰ���¶���Ϣ
	static UINT ThreadGetTemperature(LPVOID lpVoid); //����ѭ����ȡ�¶� �̺߳���
	
public:
	static CRITICAL_SECTION m_csLock;
	static std::vector<uint16_t> m_vTempList; // �¶��б�(�м����¿���)
	CString m_strSerialFilePath;
	CString m_strSerialFile;	
	BOOL m_nCurrentStatus; //�����Ƿ��

	BOOL m_bModbusClosed;//������Դ�Ƿ��ͷ�

	//modbus_t�ṹ��ָ�����
	modbus_t *ctx; 
	int regNum;    //�Ĵ����ĸ���
	tagTParam m_tParam;
private:	
	TEMPERATUREINIT m_sInit;
};

extern CControl* g_This;

