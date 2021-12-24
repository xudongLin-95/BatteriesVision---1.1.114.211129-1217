#pragma once
#include "DHInterFace.h"
#include "Log.in.h"
#include "Temperature.in.h"

#include <string>
#include <math.h>
#include "modbus.h"
#include <errno.h>

#define ADDRESS_START 0         //测试寄存器的起始地址
#define ADDRESS_END 2           //测试寄存器的结束地址

class CControl
{
public:
	struct tagTParam
	{
		int nComIndex;    //选中串口下标
		CString sComName; //串口名字
		int nUse;         //是否使用该模块
		int nTempNum;     //温控器个数
		int nIntervalTime;//读温度间隔时间
		int nResponseTimeout;//相应超时时间
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
	//modbus 获取当前温度
	BOOL GetTemp(std::vector<uint16_t>& vTemp);

	//tagTParam& GetTemperatureParam() { return m_tParam; }

	void GetCurrentTemperature();//获取当前的温度信息
	static UINT ThreadGetTemperature(LPVOID lpVoid); //串口循环获取温度 线程函数
	
public:
	static CRITICAL_SECTION m_csLock;
	static std::vector<uint16_t> m_vTempList; // 温度列表(有几个温控器)
	CString m_strSerialFilePath;
	CString m_strSerialFile;	
	BOOL m_nCurrentStatus; //串口是否打开

	BOOL m_bModbusClosed;//串口资源是否释放

	//modbus_t结构体指针变量
	modbus_t *ctx; 
	int regNum;    //寄存器的个数
	tagTParam m_tParam;
private:	
	TEMPERATUREINIT m_sInit;
};

extern CControl* g_This;

