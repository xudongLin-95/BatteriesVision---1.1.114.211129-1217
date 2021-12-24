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
		eLightControlType	eLightType; //光源控制器类型
		CString				sComName;   //串口号
		int					nComIndex;  //串口下拉下标
	};
public:
	CLightSerial(void);
	~CLightSerial(void);
	virtual int DoSomething(void* message,void* wParam,void* lParam);

	int Init(LIGHTINIT &Init);

	BOOL IsOpen() { return m_SerialPort.IsOpen(); }
	BOOL Open(); //打开光源控制器串口
	BOOL Close(); //关闭光源控制器串口
	BOOL SetBrightness(const int& nChannel, const int& nBrightness);//设置光源控制器亮度
	int  GetBrightnes(const int& nChannel);//获取光源控制器的亮度
	BOOL Log(const CString& strLog, const int& nType, const int&nExcpLevel = NoExcp);
	BOOL SaveParams();//保存参数接口
	BOOL ReadParams();//获取参数接口

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
	int	 m_nCurrentStatus;//当前状态
};

extern CLightSerial* g_This;

