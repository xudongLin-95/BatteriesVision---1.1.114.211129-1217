#pragma once
#include "DHInterFace.h"
#include "SerialPortExt.h"
#include "Log.in.h"
#include "LightControl.in.h"
class CLightSerial : DHInterface
{
public:
	typedef struct tagTParam
	{
		eLightControlType	eLightType; //��Դ����������
		CString				sComName;   //���ں�
		int					nComIndex;  //���������±�
	};
public:
	CLightSerial(void);
	~CLightSerial(void);
	virtual int DoSomething(void* message,void* wParam,void* lParam);

	int Init(LIGHTINIT &Init);

	BOOL IsOpen() { return m_SerialPort.IsOpen(); }
	BOOL Open(); //�򿪹�Դ����������
	BOOL Close(); //�رչ�Դ����������
	BOOL SetBrightness(const int& nChannel, const int& nBrightness);//���ù�Դ����������
	int  GetBrightnes(const int& nChannel);//��ȡ��Դ������������
	BOOL Log(const CString& strLog, const int& nType, const int&nExcpLevel = NoExcp);
	BOOL SaveParams();//��������ӿ�
	BOOL ReadParams();//��ȡ�����ӿ�

	CLightSerial::tagTParam& GetLightParam() { return m_tLightParam; }
	
private:
	BOOL CheckChannel(const int& nChannel);
private:
	BOOL ShowParamDlg();
	LIGHTINIT m_sInit;
	CString m_strConfigLightSerialFile;
	CString m_strConfigLightSerialFilePath;
	tagTParam m_tLightParam;
	CSerialPortExt m_SerialPort;	

public:
	int	 m_nCurrentStatus;//��ǰ״̬
};

extern CLightSerial* g_This;

