#pragma once
#include "TcAdsDef.h"
#include "TcAdsAPI.h"

#define VAL_MAX_LENGTH 1024

class CAdsExt
{
public:
	CAdsExt();
	~CAdsExt();

	//连接
	BOOL Connect(const CString& strIP, const int& nPort);

	//测试通信
	BOOL TestComm();

	//断开连接
	BOOL Disconnect();

	//重新连接
	BOOL Reconnect();

	//是否已连接
	BOOL IsConnected();

	//读bool型数据
	BOOL ReadBool(const unsigned long& lIndexGroup,const unsigned long& lIndexOffset, bool& bReadVal);
	//写bool型数据
	BOOL WriteBool(const unsigned long& lIndexGroup,const unsigned long& lIndexOffset, const bool& bWriteVal);
	//批量读bool型数据
	BOOL ReadBoolBatch(const CString& strKey, const int& nReadNum, std::vector<bool>& vecReadVal);
	//批量写bool型数据
	BOOL WriteBoolBatch(const CString& strKey, std::vector<bool>& vecWriteVal);

	//批量读short型数据
	BOOL ReadShortBatch(const CString& strKey, const int& nReadNum, std::vector<short>& vecReadVal);
	//批量写short型数据
	BOOL WriteShortBatch(const CString& strKey, std::vector<short>& vecWriteVal);

	//读int型数据
	BOOL ReadInt32(const unsigned long& lIndexGroup,const unsigned long& lIndexOffset, int& nReadVal);
	//写int型数据
	BOOL WriteInt32(const unsigned long& lIndexGroup,const unsigned long& lIndexOffset, const int& nWriteVal);
	//批量读int型数据
	BOOL ReadInt32Batch(const CString& strKey, const int& nReadNum, std::vector<int>& vecReadVal);
	//批量写int型数据
	BOOL WriteInt32Batch(const CString& strKey, std::vector<int>& vecWriteVal);
	//批量读int64型数据
	BOOL ReadInt64Batch(const CString& strKey, const int& nReadNum, std::vector<__int64>& vecReadVal);
	//批量写int64型数据
	BOOL WriteInt64Batch(const CString& strKey, std::vector<__int64>& vecWriteVal);

	//读float型数据
	BOOL ReadFloat(const unsigned long& lIndexGroup,const unsigned long& lIndexOffset, float& fReadVal);
	//写float型数据
	BOOL WriteFloat(const unsigned long& lIndexGroup,const unsigned long& lIndexOffset, const float& fWriteVal);
	//批量读float型数据
	BOOL ReadFloatBatch(const CString& strKey, const int& nReadNum, std::vector<float>& vecReadVal);
	//批量写float型数据
	BOOL WriteFloatBatch(const CString& strKey, std::vector<float>& vecWriteVal);

	//读double型数据
	BOOL ReadDouble(const unsigned long& lIndexGroup,const unsigned long& lIndexOffset, double& dReadVal);
	//写double型数据
	BOOL WriteDouble(const unsigned long& lIndexGroup,const unsigned long& lIndexOffset, const double& dWriteVal);
	//批量读double型数据
	BOOL ReadDoubleBatch(const CString& strKey, const int& nReadNum, std::vector<double>& vecReadVal);
	//批量写double型数据
	BOOL WriteDoubleBatch(const CString& strKey, std::vector<double>& vecWriteVal);

	//批量读取string型数据
	BOOL ReadString(const CString& strKey,const int& nReadNum, CString& strReadVal);
	//批量写入string型数据
	BOOL WriteString(const CString& strKey, const CString& strWriteVal);

private:
	std::vector<int> GetIPFromString(const CString& strIP);
	BOOL CheckAddr();
private:
	CRITICAL_SECTION m_cs;//临界区，防止线程冲突

	CommType m_emType;

	BOOL m_bConnected;

	//PLC的IP
	CString m_strIP;
	//PLC的端口
	int m_nPort;
	//AMS地址变量
	AmsAddr m_Addr; 
};
