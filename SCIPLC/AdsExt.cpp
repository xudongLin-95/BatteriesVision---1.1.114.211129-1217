#include "StdAfx.h"
#include "AdsExt.h"


CAdsExt::CAdsExt()
{
	InitializeCriticalSection(&m_cs);
	m_bConnected = FALSE;

	m_Addr.netId.b[0] = -1;
	m_Addr.netId.b[1] = -1;
	m_Addr.netId.b[2] = -1;
	m_Addr.netId.b[3] = -1;
	m_Addr.netId.b[4] = 1;
	m_Addr.netId.b[5] = 1;
	m_Addr.port = -1;
}

CAdsExt::~CAdsExt()
{
	Disconnect();
}

std::vector<int> CAdsExt::GetIPFromString(const CString& strIP)
{
	//192.168.1.1
	std::vector<int> vec;
	CString strIPAddr = strIP;
	int nPos = strIPAddr.Find(".");
	vec.push_back(atoi(strIPAddr.Left(nPos)));
	strIPAddr = strIPAddr.Mid(nPos + 1);
	nPos = strIPAddr.Find(".");
	vec.push_back(atoi(strIPAddr.Left(nPos)));
	strIPAddr = strIPAddr.Mid(nPos + 1);
	nPos = strIPAddr.Find(".");
	vec.push_back(atoi(strIPAddr.Left(nPos)));
	vec.push_back(atoi(strIPAddr.Mid(nPos + 1)));

	return vec;
}

BOOL CAdsExt::Connect(const CString& strIP, const int& nPort)
{
	long lPort = AdsPortOpen(); //��ADSͨѶ�˿�
	m_strIP = strIP;
	m_nPort = nPort;
	std::vector<int> vec = GetIPFromString(strIP);
	m_Addr.netId.b[0] = vec[0];      
	m_Addr.netId.b[1] = vec[1];       // IP.1.1
	m_Addr.netId.b[2] = vec[2];
	m_Addr.netId.b[3] = vec[3];
	m_Addr.netId.b[4] = 1;
	m_Addr.netId.b[5] = 1;
	m_Addr.port = nPort;
	
	return TestComm();
}

BOOL CAdsExt::CheckAddr()
{
	if (m_Addr.port < 0 ||
		m_Addr.netId.b[0] < 0 ||
		m_Addr.netId.b[1] < 0 ||
		m_Addr.netId.b[2] < 0 ||
		m_Addr.netId.b[3] < 0 )
		return FALSE;

	return TRUE;
}

BOOL CAdsExt::TestComm()
{
	if (!CheckAddr()) return FALSE;

	unsigned short nAdsState, nDevState;
	long lErr = AdsSyncReadStateReq(&m_Addr, &nAdsState, &nDevState);
	if (lErr)
	{
		m_bConnected = FALSE;
		return m_bConnected;
	}
	m_bConnected = TRUE;
	return m_bConnected;
}

BOOL CAdsExt::Disconnect()
{
	long lErr = AdsPortClose();
	/*if (lErr)
	{
		m_bConnected = TRUE;
		return FALSE;
	}*/
	
	m_bConnected = FALSE;
	return TRUE;
}

BOOL CAdsExt::Reconnect()
{
	if (IsConnected()) return TRUE;
	
	Disconnect();
	return Connect(m_strIP, m_nPort);
}

BOOL CAdsExt::IsConnected()
{
	return m_bConnected;
}

//////////////////////////////////////////////////////////////////////////
//ReadBool		��ȡbool
//@lIndexGroup	��ַ
//@lIndexOffset	��ַƫ��
//@bReadVal		��ȡboolֵ
//////////////////////////////////////////////////////////////////////////
BOOL CAdsExt::ReadBool(const unsigned long& lIndexGroup,const unsigned long& lIndexOffset, bool& bReadVal)
{
	if (!CheckAddr()) return FALSE;

	CriticalSectionHelper lock(&m_cs);
	long lErr = AdsSyncReadReq(&m_Addr, lIndexGroup, lIndexOffset, 0x1, &bReadVal);
	if (lErr)
	{
		TestComm();
		return FALSE;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//WriteBool		д��bool
//@lIndexGroup	��ַ
//@lIndexOffset	��ַƫ��
//@bWriteVal	д��boolֵ
//////////////////////////////////////////////////////////////////////////
BOOL CAdsExt::WriteBool(const unsigned long& lIndexGroup,const unsigned long& lIndexOffset, const bool& bWriteVal)
{
	if (!CheckAddr()) return FALSE;

	CriticalSectionHelper lock(&m_cs);
	bool bVal = bWriteVal;
	long lErr = AdsSyncWriteReq(&m_Addr, lIndexGroup, lIndexOffset, 0x1, &bVal);
	if (lErr)
	{
		TestComm();
		return FALSE;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//ReadBoolBatch	������ȡbool
//@strKey		�����Ӧ��ǩ����
//@nReadNum		��ȡֵ������
//@vecReadVal	��ȡboolֵ����
//////////////////////////////////////////////////////////////////////////
BOOL CAdsExt::ReadBoolBatch(const CString& strKey, const int& nReadNum, std::vector<bool>& vecReadVal)
{
	if (!CheckAddr()) return FALSE;

	CriticalSectionHelper lock(&m_cs);
	unsigned long lHdlVar;
	char *chkey = (LPSTR)(LPCTSTR)strKey;
	bool bArr[VAL_MAX_LENGTH];
	long lErr = AdsSyncReadWriteReq(&m_Addr, ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(lHdlVar), &lHdlVar, strKey.GetLength()*sizeof(char), chkey);
	if (lErr)
	{
		TestComm();
		return FALSE;
	}
	
	lErr = AdsSyncReadReq(&m_Addr, ADSIGRP_SYM_VALBYHND, lHdlVar, nReadNum * 0x1, bArr);
	if (lErr)
	{
		TestComm();
		return FALSE;
	}
	vecReadVal.clear();
	for (int i = 0; i < nReadNum; i++)
		vecReadVal.push_back(bArr[i]);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//WriteBoolBatch	����д��bool
//@strKey			�����Ӧ��ǩ����
//@vecWriteVal		Ҫд���boolֵ����
//////////////////////////////////////////////////////////////////////////
BOOL CAdsExt::WriteBoolBatch(const CString& strKey, std::vector<bool>& vecWriteVal)
{
	if (vecWriteVal.size() <= 0) return FALSE;
	if (!CheckAddr()) return FALSE;

	CriticalSectionHelper lock(&m_cs);

	unsigned long lHdlVar;
	char *chkey = (LPSTR)(LPCTSTR)strKey;
	bool bArr[VAL_MAX_LENGTH];
	for (int i = 0; i < vecWriteVal.size(); i++)
		bArr[i] = vecWriteVal[i];

	long lErr = AdsSyncReadWriteReq(&m_Addr, ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(lHdlVar), &lHdlVar, strKey.GetLength() * sizeof(char), chkey);
	if (lErr)
	{
		TestComm();
		return FALSE;
	}

	lErr = AdsSyncWriteReq(&m_Addr, ADSIGRP_SYM_VALBYHND, lHdlVar, vecWriteVal.size() * 0x1, bArr);
	if (lErr)
	{
		TestComm();
		return FALSE;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//ReadShortBatch	������ȡshort
//@strKey		�����Ӧ��ǩ����
//@nReadNum		��ȡֵ������
//@vecReadVal	��ȡboolֵ����
//////////////////////////////////////////////////////////////////////////
BOOL CAdsExt::ReadShortBatch(const CString& strKey, const int& nReadNum, std::vector<short>& vecReadVal)
{
	if (!CheckAddr()) return FALSE;

	CriticalSectionHelper lock(&m_cs);
	unsigned long lHdlVar;
	char *chkey = (LPSTR)(LPCTSTR)strKey;
	short nArr[VAL_MAX_LENGTH];
	long lErr = AdsSyncReadWriteReq(&m_Addr, ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(lHdlVar), &lHdlVar, strKey.GetLength() * sizeof(char), chkey);
	if (lErr)
	{
		TestComm();
		return FALSE;
	}

	lErr = AdsSyncReadReq(&m_Addr, ADSIGRP_SYM_VALBYHND, lHdlVar, nReadNum * 0x2, nArr);
	if (lErr)
	{
		TestComm();
		return FALSE;
	}
	vecReadVal.clear();
	for (int i = 0; i < nReadNum; i++)
		vecReadVal.push_back(nArr[i]);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//WriteShortBatch	����д��short
//@strKey			�����Ӧ��ǩ����
//@vecWriteVal		Ҫд���shortֵ����
//////////////////////////////////////////////////////////////////////////
BOOL CAdsExt::WriteShortBatch(const CString& strKey, std::vector<short>& vecWriteVal)
{
	if (vecWriteVal.size() <= 0) return FALSE;
	if (!CheckAddr()) return FALSE;

	CriticalSectionHelper lock(&m_cs);

	unsigned long lHdlVar;
	char *chkey = (LPSTR)(LPCTSTR)strKey;
	short nArr[VAL_MAX_LENGTH];
	for (int i = 0; i < vecWriteVal.size(); i++)
		nArr[i] = vecWriteVal[i];

	long lErr = AdsSyncReadWriteReq(&m_Addr, ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(lHdlVar), &lHdlVar, strKey.GetLength() * sizeof(char), chkey);
	if (lErr)
	{
		TestComm();
		return FALSE;
	}

	lErr = AdsSyncWriteReq(&m_Addr, ADSIGRP_SYM_VALBYHND, lHdlVar, vecWriteVal.size() * 0x2, nArr);
	if (lErr)
	{
		TestComm();
		return FALSE;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//ReadInt32			��ȡint
//@lIndexGroup		��ַ
//@lIndexOffset		��ַƫ��
//@nReadVal			Ҫд��ȡ��intֵ
//////////////////////////////////////////////////////////////////////////
BOOL CAdsExt::ReadInt32(const unsigned long& lIndexGroup,const unsigned long& lIndexOffset, int& nReadVal)
{
	if (!CheckAddr()) return FALSE;

	CriticalSectionHelper lock(&m_cs);
	long lErr = AdsSyncReadReq(&m_Addr, lIndexGroup, lIndexOffset, 0x4, &nReadVal);
	if (lErr)
	{
		TestComm();
		return FALSE;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//WriteInt32		д��int
//@lIndexGroup		��ַ
//@lIndexOffset		��ַƫ��
//@nWriteVal		Ҫд���intֵ
//////////////////////////////////////////////////////////////////////////
BOOL CAdsExt::WriteInt32(const unsigned long& lIndexGroup,const unsigned long& lIndexOffset, const int& nWriteVal)
{
	if (!CheckAddr()) return FALSE;

	CriticalSectionHelper lock(&m_cs);
	int nVal = nWriteVal;
	long lErr = AdsSyncWriteReq(&m_Addr, lIndexGroup, lIndexOffset, 0x4, &nVal);
	if (lErr)
	{
		TestComm();
		return FALSE;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//ReadInt32Batch	������ȡint
//@strKey			�����Ӧ��ǩ����
//@nReadVal			Ҫд��ȡ��intֵ
//////////////////////////////////////////////////////////////////////////
BOOL CAdsExt::ReadInt32Batch(const CString& strKey, const int& nReadNum, std::vector<int>& vecReadVal)
{
	if (!CheckAddr()) return FALSE;

	CriticalSectionHelper lock(&m_cs);
	unsigned long lHdlVar;
	char *chkey = (LPSTR)(LPCTSTR)strKey;
	int nArr[VAL_MAX_LENGTH];
	long lErr = AdsSyncReadWriteReq(&m_Addr, ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(lHdlVar), &lHdlVar, strKey.GetLength() * sizeof(char), chkey);
	if (lErr)
	{
		TestComm();
		return FALSE;
	}
	lErr = AdsSyncReadReq(&m_Addr, ADSIGRP_SYM_VALBYHND, lHdlVar, nReadNum*0x4, nArr);
	if (lErr)
	{
		TestComm();
		return FALSE;
	}
	vecReadVal.clear();
	for (int i = 0; i < nReadNum; i++)
		vecReadVal.push_back(nArr[i]);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//WriteInt32Batch	����д��int
//@lIndexGroup		��ַ
//@lIndexOffset		��ַƫ��
//@vecWriteVal		Ҫ����д���intֵ
//////////////////////////////////////////////////////////////////////////
BOOL CAdsExt::WriteInt32Batch(const CString& strKey, std::vector<int>& vecWriteVal)
{
	if (vecWriteVal.size() <= 0) return FALSE;
	if (!CheckAddr()) return FALSE;

	CriticalSectionHelper lock(&m_cs);

	unsigned long lHdlVar;
	char *chkey = (LPSTR)(LPCTSTR)strKey;
	int nArr[VAL_MAX_LENGTH];
	for (int i=0;i<vecWriteVal.size();i++)
		nArr[i] = vecWriteVal[i];

	long lErr = AdsSyncReadWriteReq(&m_Addr, ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(lHdlVar), &lHdlVar, strKey.GetLength() * sizeof(char), chkey);
	if (lErr)
	{
		TestComm();
		return FALSE;
	}

	lErr = AdsSyncWriteReq(&m_Addr, ADSIGRP_SYM_VALBYHND, lHdlVar, vecWriteVal.size()*0x4, nArr);
	if (lErr)
	{
		TestComm();
		return FALSE;
	}
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//ReadInt64Batch	������ȡint64
//@strKey			�����Ӧ��ǩ����
//@nReadVal			Ҫд��ȡ��int64ֵ
//////////////////////////////////////////////////////////////////////////
BOOL CAdsExt::ReadInt64Batch(const CString& strKey, const int& nReadNum, std::vector<__int64>& vecReadVal)
{
	if (!CheckAddr()) return FALSE;

	CriticalSectionHelper lock(&m_cs);
	unsigned long lHdlVar;
	char *chkey = (LPSTR)(LPCTSTR)strKey;
	__int64 nArr[VAL_MAX_LENGTH];
	long lErr = AdsSyncReadWriteReq(&m_Addr, ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(lHdlVar), &lHdlVar, strKey.GetLength() * sizeof(char), chkey);
	if (lErr)
	{
		TestComm();
		return FALSE;
	}
	lErr = AdsSyncReadReq(&m_Addr, ADSIGRP_SYM_VALBYHND, lHdlVar, nReadNum * 0x8, nArr);
	if (lErr)
	{
		TestComm();
		return FALSE;
	}
	vecReadVal.clear();
	for (int i = 0; i < nReadNum; i++)
		vecReadVal.push_back(nArr[i]);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//WriteInt64Batch	����д��int65
//@lIndexGroup		��ַ
//@lIndexOffset		��ַƫ��
//@vecWriteVal		Ҫ����д���int65ֵ
//////////////////////////////////////////////////////////////////////////
BOOL CAdsExt::WriteInt64Batch(const CString& strKey, std::vector<__int64>& vecWriteVal)
{
	if (vecWriteVal.size() <= 0) return FALSE;
	if (!CheckAddr()) return FALSE;

	CriticalSectionHelper lock(&m_cs);

	unsigned long lHdlVar;
	char *chkey = (LPSTR)(LPCTSTR)strKey;
	__int64 nArr[VAL_MAX_LENGTH];
	for (int i = 0; i < vecWriteVal.size(); i++)
		nArr[i] = vecWriteVal[i];

	long lErr = AdsSyncReadWriteReq(&m_Addr, ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(lHdlVar), &lHdlVar, strKey.GetLength() * sizeof(char), chkey);
	if (lErr)
	{
		TestComm();
		return FALSE;
	}

	lErr = AdsSyncWriteReq(&m_Addr, ADSIGRP_SYM_VALBYHND, lHdlVar, vecWriteVal.size() * 0x8, nArr);
	if (lErr)
	{
		TestComm();
		return FALSE;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//ReadFloat			��ȡfloat
//@lIndexGroup		��ַ
//@lIndexOffset		��ַƫ��
//@nReadVal			Ҫд��ȡ��floatֵ
//////////////////////////////////////////////////////////////////////////
BOOL CAdsExt::ReadFloat(const unsigned long& lIndexGroup,const unsigned long& lIndexOffset, float& fReadVal)
{
	if (!CheckAddr()) return FALSE;

	CriticalSectionHelper lock(&m_cs);
	long lErr = AdsSyncReadReq(&m_Addr, lIndexGroup, lIndexOffset, 0x4, &fReadVal);
	if (lErr)
	{
		TestComm();
		return FALSE;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//WriteFloat		д��float
//@lIndexGroup		��ַ
//@lIndexOffset		��ַƫ��
//@nWriteVal		Ҫд���floatֵ
//////////////////////////////////////////////////////////////////////////
BOOL CAdsExt::WriteFloat(const unsigned long& lIndexGroup,const unsigned long& lIndexOffset, const float& fWriteVal)
{
	if (!CheckAddr()) return FALSE;

	CriticalSectionHelper lock(&m_cs);
	float fVal = fWriteVal;
	long lErr = AdsSyncWriteReq(&m_Addr, lIndexGroup, lIndexOffset, 0x4, &fVal);
	if (lErr)
	{
		TestComm();
		return FALSE;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//ReadFloatBatch	������ȡfloat
//@strKey			�����Ӧ��ǩ����
//@nReadVal			Ҫд��ȡ��floatֵ
//////////////////////////////////////////////////////////////////////////
BOOL CAdsExt::ReadFloatBatch(const CString& strKey, const int& nReadNum, std::vector<float>& vecReadVal)
{
	if (!CheckAddr()) return FALSE;

	CriticalSectionHelper lock(&m_cs);
	unsigned long lHdlVar;
	char *chkey = (LPSTR)(LPCTSTR)strKey;
	float fArr[VAL_MAX_LENGTH];
	long lErr = AdsSyncReadWriteReq(&m_Addr, ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(lHdlVar), &lHdlVar, strKey.GetLength() * sizeof(char), chkey);
	if (lErr)
	{
		TestComm();
		return FALSE;
	}

	lErr = AdsSyncReadReq(&m_Addr, ADSIGRP_SYM_VALBYHND, lHdlVar, nReadNum * 0x4, fArr);
	if (lErr)
	{
		TestComm();
		return FALSE;
	}
	vecReadVal.clear();
	for (int i = 0; i < nReadNum; i++)
		vecReadVal.push_back(fArr[i]);

	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
//WriteFloatBatch	����д��float
//@lIndexGroup		��ַ
//@lIndexOffset		��ַƫ��
//@vecWriteVal		Ҫ����д���floatֵ
//////////////////////////////////////////////////////////////////////////
BOOL CAdsExt::WriteFloatBatch(const CString& strKey, std::vector<float>& vecWriteVal)
{
	if (vecWriteVal.size() <= 0) return FALSE;
	if (!CheckAddr()) return FALSE;

	CriticalSectionHelper lock(&m_cs);

	unsigned long lHdlVar;
	char *chkey = (LPSTR)(LPCTSTR)strKey;
	float fArr[VAL_MAX_LENGTH];
	for (int i = 0; i < vecWriteVal.size(); i++)
		fArr[i] = vecWriteVal[i];

	long lErr = AdsSyncReadWriteReq(&m_Addr, ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(lHdlVar), &lHdlVar, strKey.GetLength() * sizeof(char), chkey);
	if (lErr)
	{
		TestComm();
		return FALSE;
	}

	lErr = AdsSyncWriteReq(&m_Addr, ADSIGRP_SYM_VALBYHND, lHdlVar, vecWriteVal.size() * 0x4, fArr);
	if (lErr)
	{
		TestComm();
		return FALSE;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//ReadDouble		��ȡdouble
//@lIndexGroup		��ַ
//@lIndexOffset		��ַƫ��
//@dReadVal			Ҫ��ȡ��ֵ
//////////////////////////////////////////////////////////////////////////
BOOL CAdsExt::ReadDouble(const unsigned long& lIndexGroup,const unsigned long& lIndexOffset, double& dReadVal)
{
	if (!CheckAddr()) return FALSE;

	CriticalSectionHelper lock(&m_cs);
	long lErr = AdsSyncReadReq(&m_Addr, lIndexGroup, lIndexOffset, 0x8, &dReadVal);
	if (lErr)
	{
		TestComm();
		return FALSE;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//WriteDouble		д��double
//@lIndexGroup		��ַ
//@lIndexOffset		��ַƫ��
//@dWriteVal		Ҫд���ֵ
//////////////////////////////////////////////////////////////////////////
BOOL CAdsExt::WriteDouble(const unsigned long& lIndexGroup,const unsigned long& lIndexOffset, const double& dWriteVal)
{
	if (!CheckAddr()) return FALSE;

	CriticalSectionHelper lock(&m_cs);
	double dVal = dWriteVal;
	long lErr = AdsSyncWriteReq(&m_Addr, lIndexGroup, lIndexOffset, 0x8, &dVal);
	if (lErr)
	{
		TestComm();
		return FALSE;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//ReadDoubleBatch	������ȡdouble
//@strKey			�����Ӧ��ǩ����
//@vecReadVal		Ҫд��ȡ������
//////////////////////////////////////////////////////////////////////////
BOOL CAdsExt::ReadDoubleBatch(const CString& strKey, const int& nReadNum, std::vector<double>& vecReadVal)
{
	if (!CheckAddr()) return FALSE;

	CriticalSectionHelper lock(&m_cs);
	unsigned long lHdlVar;
	char *chkey = (LPSTR)(LPCTSTR)strKey;
	double dArr[VAL_MAX_LENGTH];
	long lErr = AdsSyncReadWriteReq(&m_Addr, ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(lHdlVar), &lHdlVar, strKey.GetLength() * sizeof(char), chkey);
	if (lErr)
	{
		TestComm();
		return FALSE;
	}
	lErr = AdsSyncReadReq(&m_Addr, ADSIGRP_SYM_VALBYHND, lHdlVar, nReadNum*0x8, dArr);
	if (lErr)
	{
		TestComm();
		return FALSE;
	}
	vecReadVal.clear();
	for (int i = 0; i < nReadNum; i++)
		vecReadVal.push_back(dArr[i]);

	return TRUE;
}

//����дdouble������
BOOL CAdsExt::WriteDoubleBatch(const CString& strKey, std::vector<double>& vecWriteVal)
{
	if (vecWriteVal.size() <= 0) return FALSE;
	if (!CheckAddr()) return FALSE;

	CriticalSectionHelper lock(&m_cs);

	unsigned long lHdlVar;
	char *chkey = (LPSTR)(LPCTSTR)strKey;
	double dArr[VAL_MAX_LENGTH];
	for (int i=0;i<vecWriteVal.size();i++)
		dArr[i] = vecWriteVal[i];

	long lErr = AdsSyncReadWriteReq(&m_Addr, ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(lHdlVar), &lHdlVar, strKey.GetLength() * sizeof(char), chkey);
	if (lErr)
	{
		TestComm();
		return FALSE;
	}

	lErr = AdsSyncWriteReq(&m_Addr, ADSIGRP_SYM_VALBYHND, lHdlVar, vecWriteVal.size()*0x8, dArr);
	if (lErr)
	{
		TestComm();
		return FALSE;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//ReadString		��ȡstring
//@strKey			�ַ���Ӧ��ǩ����
//@strReadVal		Ҫд��ȡ��stringֵ
//////////////////////////////////////////////////////////////////////////
BOOL CAdsExt::ReadString(const CString& strKey, const int& nReadNum, CString& strReadVal)
{
	if (!CheckAddr()) return FALSE;

	CriticalSectionHelper lock(&m_cs);
	unsigned long lHdlVar;
	char *chkey = (LPSTR)(LPCTSTR)strKey;
	char ch[VAL_MAX_LENGTH];
	long lErr = AdsSyncReadWriteReq(&m_Addr, ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(lHdlVar), &lHdlVar, strKey.GetLength(), chkey);
	if (lErr)
	{
		TestComm();
		return FALSE;
	}
	lErr = AdsSyncReadReq(&m_Addr, ADSIGRP_SYM_VALBYHND, lHdlVar, nReadNum*sizeof(char), ch);
	if (lErr)
	{
		TestComm();
		return FALSE;
	}
	ch[nReadNum] = 0;
	strReadVal.Format("%s", ch);
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//WriteString		д��string
//@strKey			�ַ���Ӧ��ǩ����
//@strWriteVal		Ҫд��ȡ��stringֵ
//////////////////////////////////////////////////////////////////////////
BOOL CAdsExt::WriteString(const CString& strKey, const CString& strWriteVal)
{
	if (strWriteVal.GetLength()==0) return FALSE;
	if (!CheckAddr()) return FALSE;

	CriticalSectionHelper lock(&m_cs);
	unsigned long lHdlVar;
	char *chkey = (LPSTR)(LPCTSTR)strKey;
	char ch[VAL_MAX_LENGTH];
	strcpy(ch,(LPSTR)(LPCTSTR)strWriteVal);
	ch[strWriteVal.GetLength()] = 0;
	long lErr = AdsSyncReadWriteReq(&m_Addr, ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(lHdlVar), &lHdlVar, strKey.GetLength(), chkey);
	if (lErr)
	{
		TestComm();
		return FALSE;
	}
	lErr = AdsSyncWriteReq(&m_Addr, ADSIGRP_SYM_VALBYHND, lHdlVar, (strWriteVal.GetLength()+1)*sizeof(char), ch);
	if (lErr)
	{
		TestComm();
		return FALSE;
	}

	return TRUE;
}