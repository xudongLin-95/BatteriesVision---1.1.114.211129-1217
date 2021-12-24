#include "stdafx.h"
#include "FinsTCPExt.h"

CFinsTcpExt::CFinsTcpExt()
{
	InitializeCriticalSection(&m_cs);
	m_bConnected = FALSE;

	m_emType = CommType(0);
	m_strIP = "";
	m_nPort = 0;
}

CFinsTcpExt::~CFinsTcpExt()
{
	try
	{
		CloseSocket(m_hv_Socket);
		CloseSocket(m_hv_AcceptingSocket);
	}
	catch (HException) {}
}

BOOL CFinsTcpExt::Connect(const CString& strIP, const int& nPort, CommType emType)
{
	m_emType = emType;
	m_strIP = strIP;
	m_nPort = nPort;
	if (emType == CT_SERVER)
	{
		try
		{
			OpenSocketAccept(nPort, HTuple("protocol").Append("timeout"), HTuple("TCP4").Append(3), &m_hv_AcceptingSocket);
			SocketAcceptConnect(m_hv_AcceptingSocket, "auto", &m_hv_Socket);
			return ShakeHand();
		}
		catch (HException& e)
		{
			try
			{
				CloseSocket(m_hv_Socket);
				CloseSocket(m_hv_AcceptingSocket);
			}
			catch (HException) {}
			m_hv_Socket.Clear();
			m_hv_AcceptingSocket.Clear();
			return FALSE;
		}
	}
	else
	{
		try
		{
			OpenSocketConnect(HTuple(strIP), nPort, HTuple("protocol").Append("timeout"), HTuple("TCP").Append(0.3), &m_hv_Socket);
			return ShakeHand();
		}
		catch (HException& e)
		{
			try
			{
				CloseSocket(m_hv_Socket);
			}
			catch (HException) {}
			m_hv_Socket.Clear();
			return FALSE;
		}
	}
}

BOOL CFinsTcpExt::UpdateData(const CString& strIP, const int& nPort)
{
	m_strIP = strIP;
	m_nPort = nPort;
	return TRUE;
}

BOOL CFinsTcpExt::Disconnect()
{
	if (m_emType == CT_SERVER)
	{
		try
		{
			CloseSocket(m_hv_Socket);
			CloseSocket(m_hv_AcceptingSocket);
			m_hv_Socket.Clear();
			m_hv_AcceptingSocket.Clear();
			m_bConnected = FALSE;
			return TRUE;
		}
		catch (HException& e)
		{
			m_bConnected = FALSE;
			return FALSE;
		}
	}
	else
	{
		try
		{
			CloseSocket(m_hv_Socket);
			m_hv_Socket.Clear();
			m_bConnected = FALSE;
			return TRUE;
		}
		catch (HException& e)
		{
			m_bConnected = FALSE;
			return FALSE;
		}
	}

}

BOOL CFinsTcpExt::Reconnect()
{
	if (FALSE == m_bConnected)
	{
		Disconnect();
		if (m_nPort > 0 && m_emType >= 0)
		{
			return Connect(m_strIP, m_nPort, m_emType);
		}
	}
	return TRUE;
}

BOOL CFinsTcpExt::IsConnected()
{
	return m_bConnected;
}

//////////////////////////////////////////////////////////////////////////
//ShakeHand 握手
//发送数据,如：
//46494e53	0000000c	00000000	00000000	00000000
//命令头	命令长度	命令码		错误码		节点
//返回数据,如： 
//46494e53	00000010	00000001	00000000	000000ef	00000063 (ef是PC机地址，63是PLC地址)
//命令头	命令长度	命令码		错误码		PC机地址	PLC地址
//////////////////////////////////////////////////////////////////////////
BOOL CFinsTcpExt::ShakeHand()
{
	CriticalSectionHelper lock(&m_cs);
	if (m_emType == CT_SERVER)
	{
		int nSends[24] = { 0x46,0x49,0x4e,0x53,0x00,0x00,0x00,0x0c,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 };
		HTuple hv_SendData(nSends, 24);
		HTuple hv_RecvData, hv_From;
		try
		{
			ReceiveData(m_hv_Socket, "C20", &hv_RecvData, &hv_From);
			if (hv_RecvData.Length() != 20/* || hv_RecvData[15]!=0x00*/) return FALSE;
			//m_hv_PLCAddr = hv_RecvData[hv_RecvData.Length()-1];
			//m_hv_PCAddr = hv_RecvData[hv_RecvData.Length()-5];
			SendData(m_hv_Socket, "C24", hv_SendData, HTuple());
			m_bConnected = TRUE;
			return TRUE;
		}
		catch (HException& e)
		{
			m_bConnected = FALSE;
			try
			{
				CloseSocket(m_hv_Socket);
				CloseSocket(m_hv_AcceptingSocket);
			}
			catch (HException) {}
			return FALSE;
		}
	}
	else
	{
		int nSends[20] = { 0x46,0x49,0x4e,0x53,0x00,0x00,0x00,0x0c,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 };
		HTuple hv_SendData(nSends, 20);
		HTuple hv_RecvData, hv_From;
		try
		{
			SendData(m_hv_Socket, "C20", hv_SendData, HTuple());
			ReceiveData(m_hv_Socket, "C24", &hv_RecvData, &hv_From);
			if (hv_RecvData.Length() != 24 || hv_RecvData[15] != 0x00) return FALSE;
			m_hv_PLCAddr = hv_RecvData[hv_RecvData.Length() - 1];
			m_hv_PCAddr = hv_RecvData[hv_RecvData.Length() - 5];
			m_bConnected = TRUE;
			return TRUE;
		}
		catch (HException& e)
		{
			m_bConnected = FALSE;
			try
			{
				CloseSocket(m_hv_Socket);
			}
			catch (HException) {}
			return FALSE;
		}
	}

}

//////////////////////////////////////////////////////////////////////////
//ReadInt32 读取int数据
//发送数据,如：
//46494e53	0000001a	00000002	00000000	80000200	630000ef	00000101	8202ee00	0002
//命令头	命令长度	命令码		错误码		UDP发送帧	PLC和PC地址	读命令		DM地址		读取长度	
//返回数据,如： 
//46494e53	0000001a	00000002	00000000	c0000200	ef000063	00000101	0040		0016		0000
//命令头		命令长度		命令码		错误码		UDP接收帧		PC和PLC地址	读命令		成功标识		数据低16位	数据高16位
//////////////////////////////////////////////////////////////////////////
BOOL CFinsTcpExt::ReadInt32(const int& nDMAddr, int& nReadVal)
{
	if (m_hv_PCAddr.Length() == 0 || m_hv_PLCAddr.Length() == 0 || !m_bConnected) return FALSE;

	CriticalSectionHelper lock(&m_cs);
	nReadVal = 0;

	HTuple hv_DMLower, hv_DMHigher;
	TupleBand(nDMAddr, 0xFF, &hv_DMLower);
	TupleRsh(nDMAddr, 8, &hv_DMHigher);
	int nSends[34] = { 0x46,0x49,0x4e,0x53,0x00,0x00,0x00,0x1a,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x00,0x80,0x00,0x02,0x00,
		m_hv_PLCAddr.I(),0x00,0x00,m_hv_PCAddr.I(),0x00,0x00,0x01,0x01,0x82,hv_DMHigher.I(),hv_DMLower.I(),0x00,0x00,0x02 };
	HTuple hv_SendData(nSends, 34);
	HTuple hv_RecvData, hv_From;
	try
	{
		SendData(m_hv_Socket, "C34", hv_SendData, HTuple());
		ReceiveData(m_hv_Socket, "C34", &hv_RecvData, &hv_From);
		if (hv_RecvData.Length() != 34) return FALSE;
		nReadVal = (hv_RecvData[hv_RecvData.Length() - 2].I() << 24)
			+ (hv_RecvData[hv_RecvData.Length() - 1].I() << 16)
			+ (hv_RecvData[hv_RecvData.Length() - 4].I() << 8)
			+ (hv_RecvData[hv_RecvData.Length() - 3].I());
		return TRUE;
	}
	catch (HException& e)
	{
		if (e.ErrorCode() >= 5602 && e.ErrorCode() <= 5619)
		{
			//超时重新握手
			ShakeHand();
		}
		return FALSE;
	}
}

//////////////////////////////////////////////////////////////////////////
//WriteInt32 写int数据
//发送数据,如：
//46494e53	0000001e	00000002	00000000	80000200	630000ef	00000102	8202ee00	0002		0016		0000
//命令头		命令长度		命令码		错误码		UDP发送帧		PLC和PC地址	写命令		DM地址		写入长度		数据低16位	数据高16位
//返回数据,如： 
//46494e53	00000016	00000002	00000000	c0000200	ef000063	00000102	0040
//命令头		命令长度		命令码		错误码		UDP接收帧		PC和PLC地址	写命令		成功标识
//////////////////////////////////////////////////////////////////////////
BOOL CFinsTcpExt::WriteInt32(const int& nDMAddr, const int& nWriteVal)
{
	if (!m_bConnected) return FALSE;

	CriticalSectionHelper lock(&m_cs);

	if (m_hv_PCAddr.Length() == 0 || m_hv_PLCAddr.Length() == 0) return FALSE;

	HTuple hv_DMLower, hv_DMHigher;
	TupleBand(nDMAddr, 0xFF, &hv_DMLower);
	TupleRsh(nDMAddr, 8, &hv_DMHigher);
	int nVal1 = nWriteVal & 0xff;
	int nVal2 = (nWriteVal >> 8) & 0xff;
	int nVal3 = (nWriteVal >> 16) & 0xff;
	int nVal4 = (nWriteVal >> 24) & 0xff;

	int nSends[38] = { 0x46,0x49,0x4e,0x53,0x00,0x00,0x00,0x1e,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x00,0x80,0x00,0x02,0x00,
		m_hv_PLCAddr.I(),0x00,0x00,m_hv_PCAddr.I(),0x00,0x00,0x01,0x02,0x82,hv_DMHigher.I(),hv_DMLower.I(),0x00,0x00,0x02,nVal2,nVal1,nVal4,nVal3 };
	HTuple hv_SendData(nSends, 38);
	HTuple hv_RecvData, hv_From;
	try
	{
		SendData(m_hv_Socket, "C38", hv_SendData, HTuple());
		ReceiveData(m_hv_Socket, "C30", &hv_RecvData, &hv_From);
		if (hv_RecvData.Length() != 30) return FALSE;
		if (hv_RecvData[hv_RecvData.Length() - 1].I() != 0x40
			&& hv_RecvData[hv_RecvData.Length() - 1].I() != 0x00)
			return FALSE;

		return TRUE;
	}
	catch (HException& e)
	{
		if (e.ErrorCode() >= 5602 && e.ErrorCode() <= 5619)
		{
			//超时重新握手
			ShakeHand();
		}
		return FALSE;
	}
}

//////////////////////////////////////////////////////////////////////////
//ReadInt32Batch 批量读取int型数据
//发送数据,如：
//46494e53	0000001a	00000002	00000000	80000200	630000ef	00000101	8202ee00	0004
//命令头	命令长度	命令码		错误码		UDP发送帧	PLC和PC地址	读命令		DM地址		读取长度	
//返回数据,如： 
//46494e53	0000001e	00000002	00000000	c0000200	ef000063	00000101	0040		0016		0000		0016		0000
//命令头	命令长度	命令码		错误码		UDP接收帧	PC和PLC地址	读命令		成功标识	数据低16位	数据高16位	数据低16位	数据高16位
//////////////////////////////////////////////////////////////////////////
BOOL CFinsTcpExt::ReadInt32Batch(const int& nDMAddrStart, const int& nReadNum, std::vector<int>& vecReadVal)
{
	if (m_hv_PCAddr.Length() == 0 || m_hv_PLCAddr.Length() == 0 || !m_bConnected) return FALSE;

	CriticalSectionHelper lock(&m_cs);
	vecReadVal.clear();

	HTuple hv_DMLower, hv_DMHigher, hv_LengthLower, hv_LengthHigher;
	TupleBand(nDMAddrStart, 0xFF, &hv_DMLower);
	TupleRsh(nDMAddrStart, 8, &hv_DMHigher);
	TupleBand(nReadNum * 2, 0xFF, &hv_LengthLower);
	TupleRsh(nReadNum * 2, 8, &hv_LengthHigher);
	int nSends[34] = { 0x46,0x49,0x4e,0x53,0x00,0x00,0x00,0x1a,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x00,0x80,0x00,0x02,0x00,
		m_hv_PLCAddr.I(),0x00,0x00,m_hv_PCAddr.I(),0x00,0x00,0x01,0x01,0x82,hv_DMHigher.I(),hv_DMLower.I(),0x00,hv_LengthHigher.I(),hv_LengthLower.I() };
	HTuple hv_SendData(nSends, 34);
	HTuple hv_RecvData, hv_From;
	try
	{
		CString strFormat;
		strFormat.Format("C%d", 30 + nReadNum * 4);
		SendData(m_hv_Socket, "C34", hv_SendData, HTuple());
		ReceiveData(m_hv_Socket, HTuple(strFormat), &hv_RecvData, &hv_From);
		if (hv_RecvData.Length() != 30 + nReadNum * 4) return FALSE;
		int nLength = hv_RecvData.Length();
		for (int i = 30; i < hv_RecvData.Length(); i = i + 4)
		{
			/*CString strTmp;
			strTmp.Format("%02x%02x %02x%02x %02x%02x %02x%02x ",hv_RecvData[i].I(),hv_RecvData[i+1].I(),
			hv_RecvData[i+2].I(),hv_RecvData[i+3].I(),hv_RecvData[i+4].I(),hv_RecvData[i+5].I(),
			hv_RecvData[i+6].I(),hv_RecvData[i+7].I());*/
			//strRead+=strTmp;
			int nReadVal = (hv_RecvData[i + 2].I() << 24)
				+ (hv_RecvData[i + 3].I() << 16)
				+ (hv_RecvData[i].I() << 8)
				+ (hv_RecvData[i + 1].I());
			vecReadVal.push_back(nReadVal);
		}
		//theGolbal.tLog.TimeLoging(strRead);
		if (vecReadVal.size() != nReadNum)
		{
			return FALSE;
		}

		return TRUE;
	}
	catch (HException& e)
	{
		if (e.ErrorCode() >= 5602 && e.ErrorCode() <= 5619)
		{
			//超时重新握手
			ShakeHand();
		}
		return FALSE;
	}
}

//////////////////////////////////////////////////////////////////////////
//WriteInt32Batch 批量写入int数据
//发送数据,如：
//46494e53	00000022	00000002	00000000	80000200	630000ef	00000102	8202ee00	0004		0016		0000		0016		0000
//命令头		命令长度		命令码		错误码		UDP发送帧		PLC和PC地址	写命令		DM地址		写入长度		数据低16位	数据高16位	数据低16位	数据高16位
//返回数据,如： 
//46494e53	00000016	00000002	00000000	c0000200	ef000063	00000102	0040
//命令头		命令长度		命令码		错误码		UDP接收帧		PC和PLC地址	写命令		成功标识
//////////////////////////////////////////////////////////////////////////
BOOL CFinsTcpExt::WriteInt32Batch(const int& nDMAddrStart, const std::vector<int>& vecWriteVal)
{
	if (!m_bConnected) return FALSE;

	CriticalSectionHelper lock(&m_cs);
	int nWriteNum = vecWriteVal.size();
	if (m_hv_PCAddr.Length() == 0 || m_hv_PLCAddr.Length() == 0 || nWriteNum == 0) return FALSE;

	HTuple hv_DMLower, hv_DMHigher, hv_LengthLower, hv_LengthHigher, hv_CmdLengthLower, hv_CmdLengthHigher;
	TupleBand(nDMAddrStart, 0xFF, &hv_DMLower);
	TupleRsh(nDMAddrStart, 8, &hv_DMHigher);
	TupleBand(nWriteNum * 4, 0xFF, &hv_LengthLower);
	TupleRsh(nWriteNum * 4, 8, &hv_LengthHigher);
	TupleBand(26 + nWriteNum * 8, 0xFF, &hv_CmdLengthLower);
	TupleRsh(26 + nWriteNum * 8, 8, &hv_CmdLengthHigher);

	int nSends[34] = { 0x46,0x49,0x4e,0x53,0x00,0x00,hv_CmdLengthHigher.I(),hv_CmdLengthLower.I(),0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x00,0x80,0x00,0x02,0x00,
		m_hv_PLCAddr.I(),0x00,0x00,m_hv_PCAddr.I(),0x00,0x00,0x01,0x02,0x82,hv_DMHigher.I(),hv_DMLower.I(),0x00,hv_LengthHigher.I(),hv_LengthLower.I() };
	HTuple hv_SendData(nSends, 34);
	//CString strRead="write 8508: ";
	for (int i = 0; i < nWriteNum; i++)
	{
		int nWriteVal = vecWriteVal[i];
		int nVal1 = nWriteVal & 0xff;
		int nVal2 = (nWriteVal >> 8) & 0xff;
		int nVal3 = (nWriteVal >> 16) & 0xff;
		int nVal4 = (nWriteVal >> 24) & 0xff;

		/*CString strTmp;
		strTmp.Format("%02x%02x %02x%02x %02x%02x %02x%02x ",nVal2,nVal1,
		nVal4,nVal3,0,0,0,0);
		strRead+=strTmp;*/

		hv_SendData.Append(nVal2);
		hv_SendData.Append(nVal1);
		hv_SendData.Append(nVal4);
		hv_SendData.Append(nVal3);
		hv_SendData.Append(0);
		hv_SendData.Append(0);
		hv_SendData.Append(0);
		hv_SendData.Append(0);
	}
	//theGolbal.tLog.TimeLoging(strRead);
	if (hv_SendData.Length() != 34 + nWriteNum * 8)
	{
		return FALSE;
	}
	HTuple hv_RecvData, hv_From;
	try
	{
		CString strFormat;
		strFormat.Format("C%d", hv_SendData.Length());
		SendData(m_hv_Socket, HTuple(strFormat), hv_SendData, HTuple());
		ReceiveData(m_hv_Socket, "C30", &hv_RecvData, &hv_From);
		if (hv_RecvData.Length() != 30) return FALSE;
		if (hv_RecvData[hv_RecvData.Length() - 1].I() != 0x40
			&& hv_RecvData[hv_RecvData.Length() - 1].I() != 0x00)
			return FALSE;

		return TRUE;
	}
	catch (HException& e)
	{
		if (e.ErrorCode() >= 5602 && e.ErrorCode() <= 5619)
		{
			//超时重新握手
			ShakeHand();
		}
		return FALSE;
	}
}

//////////////////////////////////////////////////////////////////////////
//ReadStringBatch 批量读取string型数据
//发送数据,如：
//46494e53	0000001a	00000002	00000000	80000200	630000ef	00000101	8202ee00	0004
//命令头	命令长度	命令码		错误码		UDP发送帧	PLC和PC地址	读命令		DM地址		读取长度	
//返回数据,如： 
//46494e53	0000001e	00000002	00000000	c0000200	ef000063	00000101	0040		0016		0000		0016		0000
//命令头	命令长度	命令码		错误码		UDP接收帧	PC和PLC地址	读命令		成功标识	数据低16位	数据高16位	数据低16位	数据高16位
//////////////////////////////////////////////////////////////////////////
BOOL CFinsTcpExt::ReadStringBatch(const int& nDMAddrStart, const int& nReadNum, CString& strReadVal)
{
	if (m_hv_PCAddr.Length() == 0 || m_hv_PLCAddr.Length() == 0 || !m_bConnected) return FALSE;

	CriticalSectionHelper lock(&m_cs);
	strReadVal = "";

	HTuple hv_DMLower, hv_DMHigher, hv_LengthLower, hv_LengthHigher;
	TupleBand(nDMAddrStart, 0xFF, &hv_DMLower);
	TupleRsh(nDMAddrStart, 8, &hv_DMHigher);
	TupleBand(int(nReadNum*0.5 + 0.5), 0xFF, &hv_LengthLower);
	TupleRsh(int(nReadNum*0.5 + 0.5), 8, &hv_LengthHigher);
	int nSends[34] = { 0x46,0x49,0x4e,0x53,0x00,0x00,0x00,0x1a,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x00,0x80,0x00,0x02,0x00,
		m_hv_PLCAddr.I(),0x00,0x00,m_hv_PCAddr.I(),0x00,0x00,0x01,0x01,0x82,hv_DMHigher.I(),hv_DMLower.I(),0x00,hv_LengthHigher.I(),hv_LengthLower.I() };
	HTuple hv_SendData(nSends, 34);
	HTuple hv_RecvData, hv_From;
	try
	{
		CString strFormat;
		strFormat.Format("C%d", 30 + int(nReadNum + 0.5));
		SendData(m_hv_Socket, "C34", hv_SendData, HTuple());
		ReceiveData(m_hv_Socket, HTuple(strFormat), &hv_RecvData, &hv_From);
		if (hv_RecvData.Length() != 30 + int(nReadNum + 0.5)) return FALSE;
		int nLength = hv_RecvData.Length();
		for (int i = 30; i < hv_RecvData.Length(); i = i + 2)
		{
			CString strTmp;
			strTmp.Format("%c%c", hv_RecvData[i + 1].I(), hv_RecvData[i].I());
			strReadVal += strTmp;

			/*if (hv_RecvData[i + 1].I() == 0 || hv_RecvData[i].I() == 0)
			{
				break;
			}*/
		}
		//theGolbal.tLog.TimeLoging(strRead);
		/*if (vecReadVal.size()!=nReadNum)
		{
		return FALSE;
		}*/

		return TRUE;
	}
	catch (HException& e)
	{
		if (e.ErrorCode() >= 5602 && e.ErrorCode() <= 5619)
		{
			//超时重新握手
			ShakeHand();
		}
		return FALSE;
	}
}

//////////////////////////////////////////////////////////////////////////
//ReadStringBatch 批量读取string型数据
//发送数据,如：
//46494e53	0000001a	00000002	00000000	80000200	630000ef	00000101	8202ee00	0004
//命令头	命令长度	命令码		错误码		UDP发送帧	PLC和PC地址	读命令		DM地址		读取长度	
//返回数据,如： 
//46494e53	0000001e	00000002	00000000	c0000200	ef000063	00000101	0040		0016		0000		0016		0000
//命令头	命令长度	命令码		错误码		UDP接收帧	PC和PLC地址	读命令		成功标识	数据低16位	数据高16位	数据低16位	数据高16位
//////////////////////////////////////////////////////////////////////////
BOOL CFinsTcpExt::ReadStringBatchInverse(const int& nDMAddrStart, const int& nReadNum, CString& strReadVal)
{
 	if (m_hv_PCAddr.Length() == 0 || m_hv_PLCAddr.Length() == 0 || !m_bConnected) return FALSE;

	CriticalSectionHelper lock(&m_cs);
	strReadVal = "";

	HTuple hv_DMLower, hv_DMHigher, hv_LengthLower, hv_LengthHigher;
	TupleBand(nDMAddrStart, 0xFF, &hv_DMLower);
	TupleRsh(nDMAddrStart, 8, &hv_DMHigher);
	TupleBand(int(nReadNum*0.5 + 0.5), 0xFF, &hv_LengthLower);
	TupleRsh(int(nReadNum*0.5 + 0.5), 8, &hv_LengthHigher);
	int nSends[34] = { 0x46,0x49,0x4e,0x53,0x00,0x00,0x00,0x1a,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x00,0x80,0x00,0x02,0x00,
		m_hv_PLCAddr.I(),0x00,0x00,m_hv_PCAddr.I(),0x00,0x00,0x01,0x01,0x82,hv_DMHigher.I(),hv_DMLower.I(),0x00,hv_LengthHigher.I(),hv_LengthLower.I() };
	HTuple hv_SendData(nSends, 34);
	HTuple hv_RecvData, hv_From;
	try
	{
		CString strFormat;
		strFormat.Format("C%d", 30 + int(nReadNum + 0.5));
		SendData(m_hv_Socket, "C34", hv_SendData, HTuple());
		ReceiveData(m_hv_Socket, HTuple(strFormat), &hv_RecvData, &hv_From);
		if (hv_RecvData.Length() != 30 + int(nReadNum + 0.5)) return FALSE;
		int nLength = hv_RecvData.Length();
		for (int i = 30; i < hv_RecvData.Length(); i = i + 2)
		{
			CString strTmp;
			strTmp.Format("%c%c", hv_RecvData[i].I(), hv_RecvData[i+1].I());
			strReadVal += strTmp;

			/*if (hv_RecvData[i + 1].I() == 0 || hv_RecvData[i].I() == 0)
			{
				break;
			}*/
		}
		//theGolbal.tLog.TimeLoging(strRead);
		/*if (vecReadVal.size()!=nReadNum)
		{
		return FALSE;
		}*/

		return TRUE;
	}
	catch (HException& e)
	{
		if (e.ErrorCode() >= 5602 && e.ErrorCode() <= 5619)
		{
			//超时重新握手
			ShakeHand();
		}
		return FALSE;
	}
}



//////////////////////////////////////////////////////////////////////////
//WriteStringBatch 批量写入string数据
//发送数据,如： 
//46494e53	00000022	00000002	00000000	80000200	630000ef	00ff0102	8202ee00	0004		     0016		 0000		 0016		 0000
//命令头	命令长度	命令码		错误码		UDP发送帧	PLC和PC地址	写命令		DM地址	 写入长度（word） 数据低16位	数据高16位	数据低16位	数据高16位
//返回数据,如： 
//46494e53	00000016	00000002	00000000	c0000200	ef000063	00000102	0040
//命令头	命令长度	命令码		错误码		UDP接收帧	PC和PLC地址	写命令		成功标识
//////////////////////////////////////////////////////////////////////////
BOOL CFinsTcpExt::WriteStringBatch(const int& nDMAddrStart, const CString& strWriteVal)
{
	if (!m_bConnected) return FALSE;

	CriticalSectionHelper lock(&m_cs);
	int nWriteNum = strWriteVal.GetLength();
	if (m_hv_PCAddr.Length() == 0 || m_hv_PLCAddr.Length() == 0 || nWriteNum == 0) return FALSE;
	HTuple hv_DMLower, hv_DMHigher, hv_LengthLower, hv_LengthHigher, hv_CmdLengthLower, hv_CmdLengthHigher;
	TupleBand(nDMAddrStart, 0xFF, &hv_DMLower);
	TupleRsh(nDMAddrStart, 8, &hv_DMHigher);
	/*TupleBand(int(nWriteNum*0.5 + 0.5), 0xFF, &hv_LengthLower);
	TupleRsh(int(nWriteNum*0.5 + 0.5), 8, &hv_LengthHigher);
	TupleBand(26 + int(nWriteNum*0.5), 0xFF, &hv_CmdLengthLower);
	TupleRsh(26 + int(nWriteNum*0.5 ), 8, &hv_CmdLengthHigher);*/

	//如果字符串长度是单数，写入的长度需要*0.5，byte变成word；由于hv_LengthLower代表的word，实际是一个byte，一个byte的写入；所以，写入是单数的时候，需要+0.5（多写一个byte）
	//好凑成一个完整的word，这样才可以写入；多写的那个byte，就可以是空格或者 是0替代；
	if (1 == nWriteNum % 2)
	{
		TupleBand(int(nWriteNum*0.5 + 0.5), 0xFF, &hv_LengthLower);
		TupleRsh(int(nWriteNum*0.5 + 0.5), 8, &hv_LengthHigher);
		//hv_CmdLengthHigher这个部分就是写入数据byte的数量；如果是单数，就+1，用空格补充；
		TupleBand(26 + int(nWriteNum/**0.5*/+1), 0xFF, &hv_CmdLengthLower);
		TupleRsh(26 + int(nWriteNum/**0.5 */+1), 8, &hv_CmdLengthHigher);
	}
	//如果是双数，直接就可以不用变化；
	else
	{
		TupleBand(int(nWriteNum*0.5), 0xFF, &hv_LengthLower);
		TupleRsh(int(nWriteNum*0.5), 8, &hv_LengthHigher);

		TupleBand(26 + int(nWriteNum/**0.5*/), 0xFF, &hv_CmdLengthLower);
		TupleRsh(26 + int(nWriteNum/**0.5 */), 8, &hv_CmdLengthHigher);
	}
	
	//原版
	/*int nSends[34] = { 0x46,0x49,0x4e,0x53,0x00,0x00,hv_CmdLengthHigher.I(),hv_CmdLengthLower.I(),0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x00,0x80,0x00,0x02,0x00,
		m_hv_PLCAddr.I(),0x00,0x00,m_hv_PCAddr.I(),0x00,0x00,0x01,0x02,0x82,hv_DMHigher.I(),hv_DMLower.I(),0x00,hv_LengthHigher.I(),hv_LengthLower.I() };*/
	//测试两个word数据  是OK的；
		//int nSends[34] = { 0x46,0x49,0x4e,0x53,0x00,0x00,0x00,0x1E,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x00,0x80,0x00,0x02,0x00,
		//	0x01,0x00,0x00,0xFB,0x00,0xFF,0x01,0x02,0x82,hv_DMHigher.I(),hv_DMLower.I(),0x00,0x00,0x02 };
	//测试版
	int nSends[34] = { 0x46,0x49,0x4e,0x53,0x00,0x00,hv_CmdLengthHigher.I(),hv_CmdLengthLower.I(),0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x00,0x80,0x00,0x02,0x00,
	m_hv_PLCAddr.I(),0x00,0x00,0xFB,0x00,0xFF,0x01,0x02,0x82,hv_DMHigher.I(),hv_DMLower.I(),0x00,hv_LengthHigher.I(),hv_LengthLower.I() };
	HTuple hv_SendData(nSends, 34);
	//CString strRead="write 8508: ";
	for (int i = 0; i < nWriteNum; i = i + 2)
	{
		//输出上高低位颠倒
		/*int nStr0 = strWriteVal[i + 1];
		int nStr1 = strWriteVal[i];*/
		//输出上高低位正常
		int nStr0 = strWriteVal[i];
		int nStr1 = strWriteVal[i+1];
		hv_SendData.Append(nStr0);
		hv_SendData.Append(nStr1);
	}
	//if (1 == nWriteNum % 2)//奇数说明最后一个字符没有添加，高位需填充1个字符
	//{
	//	//int nStr0 = 32;//空格

	//	//int nStr1 = strWriteVal[strWriteVal.GetLength() - 1];

	//	//hv_SendData.Append(nStr0);
	//	//hv_SendData.Append(nStr1);
	//	
	//	//末尾插入0
	//	int nStr0 = 32;
	//	hv_SendData.Append(nStr0);
	//}
	//末尾插入0	
	//hv_SendData.Append(0);
	//theGolbal.tLog.TimeLoging(strRead);
	/*if (hv_SendData.Length()!=34+nWriteNum*8)
	{
	return FALSE;
	}*/
	HTuple hv_RecvData, hv_From;
	try
	{
		CString strFormat;
		strFormat.Format("C%d", hv_SendData.Length());
		SendData(m_hv_Socket, HTuple(strFormat), hv_SendData, HTuple());
		ReceiveData(m_hv_Socket, "C30", &hv_RecvData, &hv_From);
		//暂时屏蔽返回数据只要不为0；就是对的
		if (hv_RecvData.Length() == 0) return FALSE;
		//if (hv_RecvData.Length() != 30) return FALSE;
		if (hv_RecvData[hv_RecvData.Length() - 1].I() != 0x40
			&& hv_RecvData[hv_RecvData.Length() - 1].I() != 0x00)
			return FALSE;

		return TRUE;
	}
	catch (HException& e)
	{
		//CString strMes;
		//strMes.Format("%s", e.ErrorMessage().Text());
		if (e.ErrorCode() >= 5602 && e.ErrorCode() <= 5619)
		{
			//超时重新握手
			//ShakeHand();
			return TRUE;
		}
		return FALSE;
	}
}

//////////////////////////////////////////////////////////////////////////
//WriteStringBatch 批量写入string数据
//发送数据,如： 
//46494e53	00000022	00000002	00000000	80000200	630000ef	00ff0102	8202ee00	0004		     0016		 0000		 0016		 0000
//命令头	命令长度	命令码		错误码		UDP发送帧	PLC和PC地址	写命令		DM地址	 写入长度（word） 数据低16位	数据高16位	数据低16位	数据高16位
//返回数据,如： 
//46494e53	00000016	00000002	00000000	c0000200	ef000063	00000102	0040
//命令头	命令长度	命令码		错误码		UDP接收帧	PC和PLC地址	写命令		成功标识
//////////////////////////////////////////////////////////////////////////
BOOL CFinsTcpExt::WriteStringBatchInverse(const int& nDMAddrStart, const CString& strWriteVal)
{
	if (!m_bConnected) return FALSE;

	CriticalSectionHelper lock(&m_cs);
	int nWriteNum = strWriteVal.GetLength();
	if (m_hv_PCAddr.Length() == 0 || m_hv_PLCAddr.Length() == 0 || nWriteNum == 0) return FALSE;
	HTuple hv_DMLower, hv_DMHigher, hv_LengthLower, hv_LengthHigher, hv_CmdLengthLower, hv_CmdLengthHigher;
	TupleBand(nDMAddrStart, 0xFF, &hv_DMLower);
	TupleRsh(nDMAddrStart, 8, &hv_DMHigher);
	/*TupleBand(int(nWriteNum*0.5 + 0.5), 0xFF, &hv_LengthLower);
	TupleRsh(int(nWriteNum*0.5 + 0.5), 8, &hv_LengthHigher);
	TupleBand(26 + int(nWriteNum*0.5), 0xFF, &hv_CmdLengthLower);
	TupleRsh(26 + int(nWriteNum*0.5 ), 8, &hv_CmdLengthHigher);*/

	//如果字符串长度是单数，写入的长度需要*0.5，byte变成word；由于hv_LengthLower代表的word，实际是一个byte，一个byte的写入；所以，写入是单数的时候，需要+0.5（多写一个byte）
	//好凑成一个完整的word，这样才可以写入；多写的那个byte，就可以是空格或者 是0替代；
	if (1 == nWriteNum % 2)
	{
		TupleBand(int(nWriteNum*0.5 + 0.5), 0xFF, &hv_LengthLower);
		TupleRsh(int(nWriteNum*0.5 + 0.5), 8, &hv_LengthHigher);
		//hv_CmdLengthHigher这个部分就是写入数据byte的数量；如果是单数，就+1，用空格补充；
		TupleBand(26 + int(nWriteNum/**0.5*/ + 1), 0xFF, &hv_CmdLengthLower);
		TupleRsh(26 + int(nWriteNum/**0.5 */ + 1), 8, &hv_CmdLengthHigher);
	}
	//如果是双数，直接就可以不用变化；
	else
	{
		TupleBand(int(nWriteNum*0.5), 0xFF, &hv_LengthLower);
		TupleRsh(int(nWriteNum*0.5), 8, &hv_LengthHigher);

		TupleBand(26 + int(nWriteNum/**0.5*/), 0xFF, &hv_CmdLengthLower);
		TupleRsh(26 + int(nWriteNum/**0.5 */), 8, &hv_CmdLengthHigher);
	}

	//原版
	/*int nSends[34] = { 0x46,0x49,0x4e,0x53,0x00,0x00,hv_CmdLengthHigher.I(),hv_CmdLengthLower.I(),0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x00,0x80,0x00,0x02,0x00,
	m_hv_PLCAddr.I(),0x00,0x00,m_hv_PCAddr.I(),0x00,0x00,0x01,0x02,0x82,hv_DMHigher.I(),hv_DMLower.I(),0x00,hv_LengthHigher.I(),hv_LengthLower.I() };*/
	//测试两个word数据  是OK的；
	//int nSends[34] = { 0x46,0x49,0x4e,0x53,0x00,0x00,0x00,0x1E,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x00,0x80,0x00,0x02,0x00,
	//	0x01,0x00,0x00,0xFB,0x00,0xFF,0x01,0x02,0x82,hv_DMHigher.I(),hv_DMLower.I(),0x00,0x00,0x02 };
	//测试版
	int nSends[34] = { 0x46,0x49,0x4e,0x53,0x00,0x00,hv_CmdLengthHigher.I(),hv_CmdLengthLower.I(),0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x00,0x80,0x00,0x02,0x00,
		m_hv_PLCAddr.I(),0x00,0x00,m_hv_PCAddr.I(),0x00,0xFF,0x01,0x02,0x82,hv_DMHigher.I(),hv_DMLower.I(),0x00,hv_LengthHigher.I(),hv_LengthLower.I() };
	HTuple hv_SendData(nSends, 34);
	//CString strRead="write 8508: ";
	for (int i = 0; i < nWriteNum; i = i + 2)
	{
		//输出上高低位颠倒
		/*int nStr0 = strWriteVal[i + 1];
		int nStr1 = strWriteVal[i];*/
		//输出上高低位正常
		int nStr0 = strWriteVal[i];
		int nStr1 = strWriteVal[i + 1];
		hv_SendData.Append(nStr0);
		hv_SendData.Append(nStr1);
	}
	
	/*if (hv_SendData.Length()!=34+nWriteNum*8)
	{
	return FALSE;
	}*/
	HTuple hv_RecvData, hv_From;
	try
	{
		CString strFormat;
		strFormat.Format("C%d", hv_SendData.Length());
		SendData(m_hv_Socket, HTuple(strFormat), hv_SendData, HTuple());
		ReceiveData(m_hv_Socket, "C30", &hv_RecvData, &hv_From);
		//暂时屏蔽返回数据只要不为0；就是对的
		if (hv_RecvData.Length() == 0) return FALSE;
		//if (hv_RecvData.Length() != 30) return FALSE;
		if (hv_RecvData[hv_RecvData.Length() - 1].I() != 0x40
			&& hv_RecvData[hv_RecvData.Length() - 1].I() != 0x00)
			return FALSE;

		return TRUE;
	}
	catch (HException& e)
	{
		//CString strMes;
		//strMes.Format("%s", e.ErrorMessage().Text());
		if (e.ErrorCode() >= 5602 && e.ErrorCode() <= 5619)
		{
			//超时重新握手
			//ShakeHand();
			return TRUE;
		}
		return FALSE;
	}
}



//////////////////////////////////////////////////////////////////////////
//ReadDMBatch 批量读取DM区域数据
//发送数据,如：
//46494e53	0000001a	00000002	00000000	80000200	630000ef	00000101	8202ee00	0004
//命令头	命令长度	命令码		错误码		UDP发送帧	PLC和PC地址	读命令		DM地址		读取长度	
//返回数据,如： 
//46494e53	0000001e	00000002	00000000	c0000200	ef000063	00000101	0040		0016		0000		0016		0000
//命令头	命令长度	命令码		错误码		UDP接收帧	PC和PLC地址	读命令		成功标识	数据低16位	数据高16位	数据低16位	数据高16位
//////////////////////////////////////////////////////////////////////////
BOOL CFinsTcpExt::ReadDMBatch(const int& nDMAddrStart, const int& nLength, HTuple& hvRead)
{
	if (m_hv_PCAddr.Length() == 0 || m_hv_PLCAddr.Length() == 0 || !m_bConnected) return FALSE;

	CriticalSectionHelper lock(&m_cs);
	hvRead.Clear();

	HTuple hv_DMLower, hv_DMHigher, hv_LengthLower, hv_LengthHigher;
	TupleBand(nDMAddrStart, 0xFF, &hv_DMLower);
	TupleRsh(nDMAddrStart, 8, &hv_DMHigher);
	TupleBand(nLength, 0xFF, &hv_LengthLower);
	TupleRsh(nLength, 8, &hv_LengthHigher);
	int nSends[34] = { 0x46,0x49,0x4e,0x53,0x00,0x00,0x00,0x1a,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x00,0x80,0x00,0x02,0x00,
		m_hv_PLCAddr.I(),0x00,0x00,m_hv_PCAddr.I(),0x00,0x00,0x01,0x01,0x82,hv_DMHigher.I(),hv_DMLower.I(),0x00,hv_LengthHigher.I(),hv_LengthLower.I() };
	HTuple hv_SendData(nSends, 34);
	HTuple hv_RecvData, hv_From;
	try
	{
		CString strFormat;
		strFormat.Format("C%d", 30 + nLength * 2);
		SendData(m_hv_Socket, "C34", hv_SendData, HTuple());
		ReceiveData(m_hv_Socket, HTuple(strFormat), &hv_RecvData, &hv_From);
		if (hv_RecvData.Length() != 30 + nLength * 2) return FALSE;
		int nLength = hv_RecvData.Length();
		hvRead = hv_RecvData.TupleLastN(30);

		if (hvRead.Length() != nLength * 2)
		{
			return FALSE;
		}

		return TRUE;
	}
	catch (HException& e)
	{
		if (e.ErrorCode() >= 5602 && e.ErrorCode() <= 5619)
		{
			//超时重新握手
			ShakeHand();
		}
		return FALSE;
	}
}

//////////////////////////////////////////////////////////////////////////
//WriteDMBatch 批量写入DM区域数据
//发送数据,如：
//46494e53	00000022	00000002	00000000	80000200	630000ef	00000102	8202ee00	0004		0016		0000		0016		0000
//命令头	命令长度	命令码		错误码		UDP发送帧	PLC和PC地址	写命令		DM地址		写入长度	数据低16位	数据高16位	数据低16位	数据高16位
//返回数据,如： 
//46494e53	00000016	00000002	00000000	c0000200	ef000063	00000102	0040
//命令头	命令长度	命令码		错误码		UDP接收帧	PC和PLC地址	写命令		成功标识
//////////////////////////////////////////////////////////////////////////
BOOL CFinsTcpExt::WriteDMBatch(const int& nDMAddrStart, const HTuple& hvWrite)
{
	if (!m_bConnected) return FALSE;
	HTuple hvWriteTmp = hvWrite;
	CriticalSectionHelper lock(&m_cs);
	int nWriteNum = hvWriteTmp.Length();
	if (m_hv_PCAddr.Length() == 0 || m_hv_PLCAddr.Length() == 0 || nWriteNum == 0) return FALSE;
	if (nWriteNum % 2 == 1)
	{
		hvWriteTmp = hvWriteTmp.TupleInsert(hvWriteTmp.Length() - 2, 0);
		nWriteNum = hvWriteTmp.Length();
	}

	HTuple hv_DMLower, hv_DMHigher, hv_LengthLower, hv_LengthHigher, hv_CmdLengthLower, hv_CmdLengthHigher;
	TupleBand(nDMAddrStart, 0xFF, &hv_DMLower);
	TupleRsh(nDMAddrStart, 8, &hv_DMHigher);
	TupleBand(int(nWriteNum*0.5 + 0.5), 0xFF, &hv_LengthLower);
	TupleRsh(int(nWriteNum*0.5 + 0.5), 8, &hv_LengthHigher);
	TupleBand(26 + nWriteNum, 0xFF, &hv_CmdLengthLower);
	TupleRsh(26 + nWriteNum, 8, &hv_CmdLengthHigher);
	/*int nSends[34] = { 0x46,0x49,0x4e,0x53,0x00,0x00,hv_CmdLengthHigher.I(),hv_CmdLengthLower.I(),0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x00,0x80,0x00,0x02,0x00,
		m_hv_PCAddr.I(),0x00,0x00,m_hv_PLCAddr.I(),0x00,0x00,0x01,0x02,0x82,hv_DMHigher.I(),hv_DMLower.I(),0x00,hv_LengthHigher.I(),hv_LengthLower.I() };*/
	int nSends[34] = { 0x46,0x49,0x4e,0x53,0x00,0x00,hv_CmdLengthHigher.I(),hv_CmdLengthLower.I(),0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x00,0x80,0x00,0x02,0x00,
		m_hv_PLCAddr.I(),0x00,0x00,m_hv_PCAddr.I(),0x00,0x00,0x01,0x02,0x82,hv_DMHigher.I(),hv_DMLower.I(),0x00,hv_LengthHigher.I(),hv_LengthLower.I() };
	HTuple hv_SendData(nSends, 34);
	hv_SendData.Append(hvWriteTmp);
	//theGolbal.tLog.TimeLoging(strRead);
	if (hv_SendData.Length() != 34 + nWriteNum)
	{
		return FALSE;
	}
	HTuple hv_RecvData, hv_From;
	try
	{
		CString strFormat;
		strFormat.Format("C%d", hv_SendData.Length());
		SendData(m_hv_Socket, HTuple(strFormat), hv_SendData, HTuple());
		ReceiveData(m_hv_Socket, "C30", &hv_RecvData, &hv_From);
		if (hv_RecvData.Length() != 30) return FALSE;
		if (hv_RecvData[hv_RecvData.Length() - 1].I() != 0x40
			&& hv_RecvData[hv_RecvData.Length() - 1].I() != 0x00)
			return FALSE;

		return TRUE;
	}
	catch (HException& e)
	{
		if (e.ErrorCode() >= 5602 && e.ErrorCode() <= 5619)
		{
			//超时重新握手
			ShakeHand();
		}
		return FALSE;
	}
}

//////////////////////////////////////////////////////////////////////////
//ReadDouble 读取double数据
//发送数据,如：
//46494e53	0000001a	00000002	00000000	80000200	630000ef	00000101	8202ee00	0002
//命令头	命令长度	命令码		错误码		UDP发送帧	PLC和PC地址	读命令		DM地址		读取长度	
//返回数据,如： 
//46494e53	0000001a	00000002	00000000	c0000200	ef000063	00000101	0040		0016		0000		0016		0000
//命令头		命令长度		命令码		错误码		UDP接收帧		PC和PLC地址	读命令		成功标识		数据低16位	数据高16位		数据低16位	数据高16位
//////////////////////////////////////////////////////////////////////////
BOOL CFinsTcpExt::ReadDouble(const int& nDMAddr, double& dReadVal)
{
	if (m_hv_PCAddr.Length() == 0 || m_hv_PLCAddr.Length() == 0 || !m_bConnected) return FALSE;

	CriticalSectionHelper lock(&m_cs);
	dReadVal = 0;

	HTuple hv_DMLower, hv_DMHigher;
	TupleBand(nDMAddr, 0xFF, &hv_DMLower);
	TupleRsh(nDMAddr, 8, &hv_DMHigher);
	int nSends[34] = { 0x46,0x49,0x4e,0x53,0x00,0x00,0x00,0x1a,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x00,0x80,0x00,0x02,0x00,
		m_hv_PLCAddr.I(),0x00,0x00,m_hv_PCAddr.I(),0x00,0x00,0x01,0x01,0x82,hv_DMHigher.I(),hv_DMLower.I(),0x00,0x00,0x04 };
	HTuple hv_SendData(nSends, 34);
	HTuple hv_RecvData, hv_From;
	try
	{
		SendData(m_hv_Socket, "C34", hv_SendData, HTuple());
		ReceiveData(m_hv_Socket, "C38", &hv_RecvData, &hv_From);
		if (hv_RecvData.Length() != 38) return FALSE;
		unsigned char chData[8];
		chData[0] = hv_RecvData[hv_RecvData.Length() - 1].I();
		chData[1] = hv_RecvData[hv_RecvData.Length() - 2].I();
		chData[2] = hv_RecvData[hv_RecvData.Length() - 3].I();
		chData[3] = hv_RecvData[hv_RecvData.Length() - 4].I();
		chData[4] = hv_RecvData[hv_RecvData.Length() - 5].I();
		chData[5] = hv_RecvData[hv_RecvData.Length() - 6].I();
		chData[6] = hv_RecvData[hv_RecvData.Length() - 7].I();
		chData[7] = hv_RecvData[hv_RecvData.Length() - 8].I();
		dReadVal = *(double*)chData;
		return TRUE;
	}
	catch (HException& e)
	{
		if (e.ErrorCode() >= 5602 && e.ErrorCode() <= 5619)
		{
			//超时重新握手
			ShakeHand();
		}
		return FALSE;
	}
}

BOOL CFinsTcpExt::ReadFloat32(const int& nDMAddr, float& fReadVal)
{
	if (m_hv_PCAddr.Length() == 0 || m_hv_PLCAddr.Length() == 0 || !m_bConnected) return FALSE;

	CriticalSectionHelper lock(&m_cs);
	fReadVal = 0;

	HTuple hv_DMLower, hv_DMHigher;
	TupleBand(nDMAddr, 0xFF, &hv_DMLower);
	TupleRsh(nDMAddr, 8, &hv_DMHigher);
	int nSends[34] = { 0x46,0x49,0x4e,0x53,0x00,0x00,0x00,0x1a,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x00,0x80,0x00,0x02,0x00,
		m_hv_PLCAddr.I(),0x00,0x00,m_hv_PCAddr.I(),0x00,0x00,0x01,0x01,0x82,hv_DMHigher.I(),hv_DMLower.I(),0x00,0x00,0x02 };
	HTuple hv_SendData(nSends, 34);
	HTuple hv_RecvData, hv_From;
	try
	{
		SendData(m_hv_Socket, "C34", hv_SendData, HTuple());
		ReceiveData(m_hv_Socket, "C34", &hv_RecvData, &hv_From);
		if (hv_RecvData.Length() != 34) return FALSE;
		unsigned char chData[4];
		chData[1] = hv_RecvData[hv_RecvData.Length() - 4].I();
		chData[0] = hv_RecvData[hv_RecvData.Length() - 3].I();
		chData[3] = hv_RecvData[hv_RecvData.Length() - 2].I();
		chData[2] = hv_RecvData[hv_RecvData.Length() - 1].I();
		fReadVal = *(float*)chData;
		return TRUE;
	}
	catch (HException& e)
	{
		if (e.ErrorCode() >= 5602 && e.ErrorCode() <= 5619)
		{
			//超时重新握手
			ShakeHand();
		}
		return FALSE;
	}
}

//////////////////////////////////////////////////////////////////////////
//WriteDouble 写double数据
//发送数据,如：
//46494e53	0000001e	00000002	00000000	80000200	630000ef	00000102	8202ee00	0002		0016		0000		0016		0000
//命令头		命令长度		命令码		错误码		UDP发送帧		PLC和PC地址	写命令		DM地址		写入长度		数据低16位	数据高16位		数据低16位	数据高16位
//返回数据,如： 
//46494e53	00000016	00000002	00000000	c0000200	ef000063	00000102	0040
//命令头		命令长度		命令码		错误码		UDP接收帧		PC和PLC地址	写命令		成功标识
//////////////////////////////////////////////////////////////////////////
BOOL CFinsTcpExt::WriteDouble(const int& nDMAddr, const double& dWriteVal)
{
	if (!m_bConnected) return FALSE;

	CriticalSectionHelper lock(&m_cs);

	if (m_hv_PCAddr.Length() == 0 || m_hv_PLCAddr.Length() == 0) return FALSE;

	HTuple hv_DMLower, hv_DMHigher;
	TupleBand(nDMAddr, 0xFF, &hv_DMLower);
	TupleRsh(nDMAddr, 8, &hv_DMHigher);
	int nVal1/* = dWriteVal & 0xff*/;
	int nVal2/* = (dWriteVal >> 8) & 0xff*/;
	int nVal3/* = (dWriteVal >> 16) & 0xff*/;
	int nVal4/* = (dWriteVal >> 24) & 0xff*/;

	int nSends[42] = { 0x46,0x49,0x4e,0x53,0x00,0x00,0x00,0x1e,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x00,0x80,0x00,0x02,0x00,
		m_hv_PLCAddr.I(),0x00,0x00,m_hv_PCAddr.I(),0x00,0x00,0x01,0x02,0x82,hv_DMHigher.I(),hv_DMLower.I(),0x00,0x00,0x02,
		nVal2,nVal1,nVal4,nVal3,nVal2,nVal1,nVal4,nVal3 };
	HTuple hv_SendData(nSends, 42);
	HTuple hv_RecvData, hv_From;
	try
	{
		SendData(m_hv_Socket, "C38", hv_SendData, HTuple());
		ReceiveData(m_hv_Socket, "C30", &hv_RecvData, &hv_From);
		if (hv_RecvData.Length() != 30) return FALSE;
		if (hv_RecvData[hv_RecvData.Length() - 1].I() != 0x40
			&& hv_RecvData[hv_RecvData.Length() - 1].I() != 0x00)
			return FALSE;

		return TRUE;
	}
	catch (HException& e)
	{
		if (e.ErrorCode() >= 5602 && e.ErrorCode() <= 5619)
		{
			//超时重新握手
			ShakeHand();
		}
		return FALSE;
	}
}

BOOL CFinsTcpExt::WriteFloat32(const int& nDMAddr, const float& fWriteVal)
{
	if (!m_bConnected) return FALSE;

	CriticalSectionHelper lock(&m_cs);

	if (m_hv_PCAddr.Length() == 0 || m_hv_PLCAddr.Length() == 0) return FALSE;

	HTuple hv_DMLower, hv_DMHigher;
	TupleBand(nDMAddr, 0xFF, &hv_DMLower);
	TupleRsh(nDMAddr, 8, &hv_DMHigher);
	byte bAry[4];
	byte* lpAry = bAry;
	*(float*)lpAry = fWriteVal;

	int nSends[38] = { 0x46,0x49,0x4e,0x53,0x00,0x00,0x00,0x1e,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x00,0x80,0x00,0x02,0x00,
		m_hv_PLCAddr.I(),0x00,0x00,m_hv_PCAddr.I(),0x00,0x00,0x01,0x02,0x82,hv_DMHigher.I(),hv_DMLower.I(),0x00,0x00,0x02,bAry[1],bAry[0],bAry[3],bAry[2] };
	HTuple hv_SendData(nSends, 38);
	HTuple hv_RecvData, hv_From;
	try
	{
		SendData(m_hv_Socket, "C38", hv_SendData, HTuple());
		ReceiveData(m_hv_Socket, "C30", &hv_RecvData, &hv_From);
		if (hv_RecvData.Length() != 30) return FALSE;
		if (hv_RecvData[hv_RecvData.Length() - 1].I() != 0x40
			&& hv_RecvData[hv_RecvData.Length() - 1].I() != 0x00)
			return FALSE;

		return TRUE;
	}
	catch (HException& e)
	{
		if (e.ErrorCode() >= 5602 && e.ErrorCode() <= 5619)
		{
			//超时重新握手
			ShakeHand();
		}
		return FALSE;
	}
}