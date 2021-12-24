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
	long lPort = AdsPortOpen(); //打开ADS通讯端口
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
//ReadBool		读取bool
//@lIndexGroup	地址
//@lIndexOffset	地址偏移
//@bReadVal		读取bool值
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
//WriteBool		写入bool
//@lIndexGroup	地址
//@lIndexOffset	地址偏移
//@bWriteVal	写入bool值
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
//ReadBoolBatch	批量读取bool
//@strKey		数组对应标签名称
//@nReadNum		读取值的数量
//@vecReadVal	读取bool值数组
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
//WriteBoolBatch	批量写入bool
//@strKey			数组对应标签名称
//@vecWriteVal		要写入的bool值数组
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
//ReadShortBatch	批量读取short
//@strKey		数组对应标签名称
//@nReadNum		读取值的数量
//@vecReadVal	读取bool值数组
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
//WriteShortBatch	批量写入short
//@strKey			数组对应标签名称
//@vecWriteVal		要写入的short值数组
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
//ReadInt32			读取int
//@lIndexGroup		地址
//@lIndexOffset		地址偏移
//@nReadVal			要写读取的int值
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
//WriteInt32		写入int
//@lIndexGroup		地址
//@lIndexOffset		地址偏移
//@nWriteVal		要写入的int值
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
//ReadInt32Batch	批量读取int
//@strKey			数组对应标签名称
//@nReadVal			要写读取的int值
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
//WriteInt32Batch	批量写入int
//@lIndexGroup		地址
//@lIndexOffset		地址偏移
//@vecWriteVal		要批量写入的int值
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
//ReadInt64Batch	批量读取int64
//@strKey			数组对应标签名称
//@nReadVal			要写读取的int64值
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
//WriteInt64Batch	批量写入int65
//@lIndexGroup		地址
//@lIndexOffset		地址偏移
//@vecWriteVal		要批量写入的int65值
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
//ReadFloat			读取float
//@lIndexGroup		地址
//@lIndexOffset		地址偏移
//@nReadVal			要写读取的float值
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
//WriteFloat		写入float
//@lIndexGroup		地址
//@lIndexOffset		地址偏移
//@nWriteVal		要写入的float值
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
//ReadFloatBatch	批量读取float
//@strKey			数组对应标签名称
//@nReadVal			要写读取的float值
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
//WriteFloatBatch	批量写入float
//@lIndexGroup		地址
//@lIndexOffset		地址偏移
//@vecWriteVal		要批量写入的float值
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
//ReadDouble		读取double
//@lIndexGroup		地址
//@lIndexOffset		地址偏移
//@dReadVal			要读取的值
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
//WriteDouble		写入double
//@lIndexGroup		地址
//@lIndexOffset		地址偏移
//@dWriteVal		要写入的值
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
//ReadDoubleBatch	批量读取double
//@strKey			数组对应标签名称
//@vecReadVal		要写读取的数组
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

//批量写double型数据
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
//ReadString		读取string
//@strKey			字符对应标签名称
//@strReadVal		要写读取的string值
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
//WriteString		写入string
//@strKey			字符对应标签名称
//@strWriteVal		要写读取的string值
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