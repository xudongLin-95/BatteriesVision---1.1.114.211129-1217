#pragma once
class CModbusTcpExt
{
public:
	CModbusTcpExt(void);
	~CModbusTcpExt(void);
public:
	//连接
	BOOL Connect(const CString& strIP,const int& nPort);

	//重连
	BOOL Reconnect();

	//是否已连接
	BOOL IsConnected();

	//断开连接
	BOOL Disconnect();

	//握手
	//	BOOL ShakeHand();

	//读int型数据
	int ReadInt32(const int& nDMAddr,int& nReadVal);
	//写int型数据
	BOOL WriteInt32(const int& nDMAddr,const int& nWriteVal);
	//写 short int型数据
	BOOL WriteInt16(const int& nDMAddr,const int& nWriteVal);
	//读 float型数据
	BOOL ReadFloat(const int& nDMAddr,float& dReadData);
	//写 float型数据
	BOOL WriteFloat(const int& nDMAddr,const float& dWriteVal);
	//批量读 float型数据
	BOOL ReadMultFloatData(const int& nDMAddr,std::vector<float>& vecWriteVal,int nNum);
	//批量写 float型数据
	BOOL WriteMultFloatData(const int& nDMAddr,std::vector<float>& vecWriteVal);
	
//	fl
	void hex_to_float(const float& val, HTuple hvdata);
private:
	CRITICAL_SECTION m_cs;
	BOOL m_bConnected;//连接成功标志
	HTuple m_hv_Socket;
	CString m_strIP;
	int m_nPort;
};

