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
		//eUpsControlType	eUpsType; //UPS类型 暂时不用
		CString				nComName;   //串口名字
		int                 nComIndex;  //串口下标
	};
public:
	CUpsSerial();
	~CUpsSerial();
public:
	virtual int DoSomething(void* message, void* wParam, void* lParam);

	int Init(UPSINIT &Init);

	BOOL IsOpen();
	BOOL Open();  //打开光源控制器串口
	BOOL Close(); //关闭光源控制器串口
	BOOL SetUpsConnect(CString &recv);//UPS建立连接
	BOOL GetUpsStates(CString &state);//获取UPS自发报警信息

	BOOL SaveParams();//保存参数接口
	BOOL ReadParams();//获取参数接口

	CUpsSerial::tagTParam& GetLightParam() { return m_tUpsParam; }

	void GetUpsAlram();//获取自动报警的状态
	static UINT ThreadUpsState(LPVOID lpVoid); //UPS循环监测自动异常信息 线程函数

	CString GetUpsAlarmResult() { return UpsAlarmMsg; }//获取线程监测结果信息

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

