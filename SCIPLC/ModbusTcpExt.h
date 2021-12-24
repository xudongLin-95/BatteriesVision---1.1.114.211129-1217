#pragma once
class CModbusTcpExt
{
public:
	CModbusTcpExt(void);
	~CModbusTcpExt(void);
public:
	//����
	BOOL Connect(const CString& strIP,const int& nPort);

	//����
	BOOL Reconnect();

	//�Ƿ�������
	BOOL IsConnected();

	//�Ͽ�����
	BOOL Disconnect();

	//����
	//	BOOL ShakeHand();

	//��int������
	int ReadInt32(const int& nDMAddr,int& nReadVal);
	//дint������
	BOOL WriteInt32(const int& nDMAddr,const int& nWriteVal);
	//д short int������
	BOOL WriteInt16(const int& nDMAddr,const int& nWriteVal);
	//�� float������
	BOOL ReadFloat(const int& nDMAddr,float& dReadData);
	//д float������
	BOOL WriteFloat(const int& nDMAddr,const float& dWriteVal);
	//������ float������
	BOOL ReadMultFloatData(const int& nDMAddr,std::vector<float>& vecWriteVal,int nNum);
	//����д float������
	BOOL WriteMultFloatData(const int& nDMAddr,std::vector<float>& vecWriteVal);
	
//	fl
	void hex_to_float(const float& val, HTuple hvdata);
private:
	CRITICAL_SECTION m_cs;
	BOOL m_bConnected;//���ӳɹ���־
	HTuple m_hv_Socket;
	CString m_strIP;
	int m_nPort;
};

