#pragma once
#include "TcAdsDef.h"
#include "TcAdsAPI.h"

#define VAL_MAX_LENGTH 1024

class CAdsExt
{
public:
	CAdsExt();
	~CAdsExt();

	//����
	BOOL Connect(const CString& strIP, const int& nPort);

	//����ͨ��
	BOOL TestComm();

	//�Ͽ�����
	BOOL Disconnect();

	//��������
	BOOL Reconnect();

	//�Ƿ�������
	BOOL IsConnected();

	//��bool������
	BOOL ReadBool(const unsigned long& lIndexGroup,const unsigned long& lIndexOffset, bool& bReadVal);
	//дbool������
	BOOL WriteBool(const unsigned long& lIndexGroup,const unsigned long& lIndexOffset, const bool& bWriteVal);
	//������bool������
	BOOL ReadBoolBatch(const CString& strKey, const int& nReadNum, std::vector<bool>& vecReadVal);
	//����дbool������
	BOOL WriteBoolBatch(const CString& strKey, std::vector<bool>& vecWriteVal);

	//������short������
	BOOL ReadShortBatch(const CString& strKey, const int& nReadNum, std::vector<short>& vecReadVal);
	//����дshort������
	BOOL WriteShortBatch(const CString& strKey, std::vector<short>& vecWriteVal);

	//��int������
	BOOL ReadInt32(const unsigned long& lIndexGroup,const unsigned long& lIndexOffset, int& nReadVal);
	//дint������
	BOOL WriteInt32(const unsigned long& lIndexGroup,const unsigned long& lIndexOffset, const int& nWriteVal);
	//������int������
	BOOL ReadInt32Batch(const CString& strKey, const int& nReadNum, std::vector<int>& vecReadVal);
	//����дint������
	BOOL WriteInt32Batch(const CString& strKey, std::vector<int>& vecWriteVal);
	//������int64������
	BOOL ReadInt64Batch(const CString& strKey, const int& nReadNum, std::vector<__int64>& vecReadVal);
	//����дint64������
	BOOL WriteInt64Batch(const CString& strKey, std::vector<__int64>& vecWriteVal);

	//��float������
	BOOL ReadFloat(const unsigned long& lIndexGroup,const unsigned long& lIndexOffset, float& fReadVal);
	//дfloat������
	BOOL WriteFloat(const unsigned long& lIndexGroup,const unsigned long& lIndexOffset, const float& fWriteVal);
	//������float������
	BOOL ReadFloatBatch(const CString& strKey, const int& nReadNum, std::vector<float>& vecReadVal);
	//����дfloat������
	BOOL WriteFloatBatch(const CString& strKey, std::vector<float>& vecWriteVal);

	//��double������
	BOOL ReadDouble(const unsigned long& lIndexGroup,const unsigned long& lIndexOffset, double& dReadVal);
	//дdouble������
	BOOL WriteDouble(const unsigned long& lIndexGroup,const unsigned long& lIndexOffset, const double& dWriteVal);
	//������double������
	BOOL ReadDoubleBatch(const CString& strKey, const int& nReadNum, std::vector<double>& vecReadVal);
	//����дdouble������
	BOOL WriteDoubleBatch(const CString& strKey, std::vector<double>& vecWriteVal);

	//������ȡstring������
	BOOL ReadString(const CString& strKey,const int& nReadNum, CString& strReadVal);
	//����д��string������
	BOOL WriteString(const CString& strKey, const CString& strWriteVal);

private:
	std::vector<int> GetIPFromString(const CString& strIP);
	BOOL CheckAddr();
private:
	CRITICAL_SECTION m_cs;//�ٽ�������ֹ�̳߳�ͻ

	CommType m_emType;

	BOOL m_bConnected;

	//PLC��IP
	CString m_strIP;
	//PLC�Ķ˿�
	int m_nPort;
	//AMS��ַ����
	AmsAddr m_Addr; 
};
