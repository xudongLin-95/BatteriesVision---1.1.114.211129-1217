//////////////////////////////////////////////////////////////////////////
///FinsTcpExt.h 将fins协议封装成通信类，应用于与plc通信
///
///plc1个DM地址占2个字节，16位；4个地址为8个字节，64位；
///
///Windows x64系统（LLP64数据类型）下:
///char占1个字节，8位；short占2个字节，16位；int占4个字节，32位；pointer占4个字节，32位；
///long占4个字节，32位；long long占8个字节，64位；float占4个字节，32位；double占8个字节，64位
//////////////////////////////////////////////////////////////////////////

#pragma once
//#include "stdafx.h"
//#include "HalconCpp.h"
//using namespace HalconCpp;

enum CommType
{
	CT_SERVER = 0,
	CT_CLIENT,
};

class CFinsTcpExt
{
public:
	CFinsTcpExt();
	~CFinsTcpExt();

public:
	//连接
	BOOL Connect(const CString& strIP, const int& nPort, CommType emType);

	BOOL UpdateData(const CString& strIP, const int& nPort);

	//断开连接
	BOOL Disconnect();

	//重新连接
	BOOL Reconnect();

	//是否已连接
	BOOL IsConnected();

	//握手
	BOOL ShakeHand();

	//读int型数据，占4个地址，只处理前2个地址
	BOOL ReadInt32(const int& nDMAddr, int& nReadVal);

	//写int型数据，占4个地址，只处理前2个地址
	BOOL WriteInt32(const int& nDMAddr, const int& nWriteVal);

	//批量读取int型数据，每个值占4个地址，只处理前2个地址
	BOOL ReadInt32Batch(const int& nDMAddrStart, const int& nReadNum, std::vector<int>& vecReadVal);

	//批量写入int型数据，每个值占4个地址，只处理前2个地址
	BOOL WriteInt32Batch(const int& nDMAddrStart, const std::vector<int>& vecWriteVal);

	//批量读取string型数据，两个地址
	BOOL ReadStringBatch(const int& nDMAddrStart, const int& nReadNum, CString& strReadVal);
	//批量读取string型数据，高低位变换 
	BOOL ReadStringBatchInverse(const int& nDMAddrStart, const int& nReadNum, CString& strReadVal);

	//批量写入string型数据
	BOOL WriteStringBatch(const int& nDMAddrStart, const CString& strWriteVal);
	//批量写入string型数据，高低位翻转
	BOOL WriteStringBatchInverse(const int& nDMAddrStart, const CString& strWriteVal);

	//读32位REAL型数据，占2个地址
	BOOL ReadFloat32(const int& nDMAddr, float& fReadVal);
	//读double型数据，占4个地址
	BOOL ReadDouble(const int& nDMAddr, double& dReadVal);


	//读32位REAL型数据，占2个地址
	BOOL WriteFloat32(const int& nDMAddr, const float& fWriteVal);
	//写double型数据，占4个地址
	BOOL WriteDouble(const int& nDMAddr, const double& dWriteVal);

private:
	//批量读取DM区域数据
	BOOL ReadDMBatch(const int& nDMAddrStart, const int& nLength, HTuple& hvRead);

	//批量写入DM区域数据
	BOOL WriteDMBatch(const int& nDMAddrStart, const HTuple& hvWrite);

private:
	CRITICAL_SECTION m_cs;//临界区，防止线程冲突

	CommType m_emType;

	HTuple m_hv_AcceptingSocket;

	BOOL m_bConnected;

	//PLC的IP
	CString m_strIP;

	//PLC的端口
	int m_nPort;

	//PLC目标地址，从握手数据中读取
	HTuple m_hv_PLCAddr;

	//PC源地址，从握手数据中读取
	HTuple m_hv_PCAddr;
public:
	HTuple m_hv_Socket;

};