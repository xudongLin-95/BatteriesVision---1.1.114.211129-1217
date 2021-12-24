//////////////////////////////////////////////////////////////////////////
////SerialPortExt.h 串口通信类 SW 2020.10.11
//////////////////////////////////////////////////////////////////////////

#pragma once
#include <vector>

class CSerialPortExt
{
public:
#define READ_TIMEOUT  100

public:
	CSerialPortExt();
	CSerialPortExt(CString strPortName, int nBaudRate = 9600, int nBitCount = 8, int nParity = 0, int nStopBits = 0);
	CSerialPortExt(int nComSN, int nBaudRate = 9600, int nBitCount = 8, int nParity = 0, int nStopBits = 0);

	~CSerialPortExt();

public:
	//打开串口 nBaudRate波特率 nBitCount数据位 nParity校验位 nStopBits停止位
	bool Open(CString strPortName, int nBaudRate = 9600, int nBitCount = 8, int nParity = 0, int nStopBits = 0);
	bool Open(int nComSN, int nBaudRate = 9600, int nBitCount = 8, int nParity = 0, int nStopBits = 0);

	//关闭串口
	bool Close();

	//读取串口信息
	bool Read(CString &strReceiveData, DWORD dwReceiveLength = 0);

	//写入串口信息
	bool Write(const CString &strSendData);

	//获取设备串口列表
	bool GetComList(std::vector<CString> &vecComList);

	//获取光源亮度(光源控制器)
	bool GetLightBrightness(const int &nChannel, int &nBrightness);

	//设置光源亮度(光源控制器),亮度范围0~255
	bool SetLightBrightness(const int &nChannel, int nBrightness);

	//设置光源常亮状态(光源控制器) 常亮或常灭
	bool SetLightStatus(bool bOn = true);

	bool IsOpen() { return m_bOpen; }

private:
	OVERLAPPED m_tReadovReady, m_tWriteovReady;
	HANDLE m_hComm;
	bool m_bOpen;
};

