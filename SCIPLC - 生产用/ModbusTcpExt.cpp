#include "StdAfx.h"
#include "ModbusTcpExt.h"


CModbusTcpExt::CModbusTcpExt(void)
{
	InitializeCriticalSection(&m_cs);
	m_bConnected = FALSE;
}

CModbusTcpExt::~CModbusTcpExt(void)
{
}

BOOL CModbusTcpExt::Connect(const CString& strIP, const int& nPort)
{
	m_strIP = strIP;
	m_nPort = nPort;
	try
	{
		OpenSocketConnect(HTuple(strIP), nPort, HTuple("protocol").Append("timeout"), HTuple("TCP").Append(0.1), &m_hv_Socket);
		//return ShakeHand();
		return TRUE;
	}
	catch (HException& e)
	{
		m_hv_Socket.Clear();
		return FALSE;
	}
}

BOOL CModbusTcpExt::Disconnect()
{
	try
	{
		CloseSocket(m_hv_Socket);
		m_hv_Socket.Clear();
		return TRUE;
	}
	catch (HException& e)
	{
		return FALSE;
	}
}

BOOL CModbusTcpExt::Reconnect()
{
	if (FALSE == m_bConnected)
	{
		Disconnect();
		if (m_nPort>0 && m_strIP != "")
		{
			return Connect(m_strIP, m_nPort);
		}
	}
	return TRUE;
}

BOOL CModbusTcpExt::IsConnected()
{
	return m_bConnected;
}

//////////////////////////////////////////////////////////////////////////
//ReadInt32 读取int数据
//发送数据,如：
//46494e53	0000001a	00000002	00000000	80000200	630000ef	00000101	8202ee00	0002
//命令头	命令长度	命令码		错误码		UDP发送帧	PLC和PC地址	读命令		DM地址		读取长度	
//返回数据,如： 
//46494e53	0000001a	00000002	00000000	c0000200	ef000063	00000101	0040		0016		0000
//命令头	命令长度	命令码		错误码		UDP接收帧	PC和PLC地址	读命令		成功标识	数据低16位	数据高16位
//////////////////////////////////////////////////////////////////////////
BOOL CModbusTcpExt::ReadInt32(const int& nDMAddr, int& nReadVal)
{
	nReadVal = 0;
	//if (m_hv_PCAddr.Length()==0 || m_hv_PLCAddr.Length()==0) return FALSE;

	HTuple hv_DMLower, hv_DMHigher;
	TupleBand(nDMAddr, 0xFF, &hv_DMLower);
	TupleRsh(nDMAddr, 8, &hv_DMHigher);
	int nSends[34] = { 0x46,0x49,0x4e,0x53,0x00,0x00,0x00,0x1a,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x00,0x80,0x00,0x02,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x82,hv_DMHigher.I(),hv_DMLower.I(),0x00,0x00,0x02 };
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
		if (e.ErrorCode() == 5619)
		{
			//超时重新握手
			//Disconnect();
			//Connect();
			//ShakeHand();
		}
		return FALSE;
	}
}

//////////////////////////////////////////////////////////////////////////
//WriteInt32 写int数据
//发送数据,如：
//46494e53	0000001e	00000002	00000000	80000200	630000ef	00000102	8202ee00	0002		0016		0000
//命令头	命令长度	命令码		错误码		UDP发送帧	PLC和PC地址	写命令		DM地址		写入长度	数据低16位	数据高16位
//返回数据,如： 
//46494e53	00000016	00000002	00000000	c0000200	ef000063	00000102	0040
//命令头	命令长度	命令码		错误码		UDP接收帧	PC和PLC地址	写命令		成功标识
//////////////////////////////////////////////////////////////////////////
BOOL CModbusTcpExt::WriteInt32(const int& nDMAddr, const int& nWriteVal)
{

	HTuple hv_DMLower, hv_DMHigher;
	TupleBand(nDMAddr, 0xFF, &hv_DMLower);
	TupleRsh(nDMAddr, 8, &hv_DMHigher);
	int nVal1 = nWriteVal & 0xff;
	int nVal2 = (nWriteVal >> 8) & 0xff;
	int nVal3 = (nWriteVal >> 16) & 0xff;
	int nVal4 = (nWriteVal >> 24) & 0xff;

	int nSends[38] = { 0x46,0x49,0x4e,0x53,0x00,0x00,0x00,0x1e,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x00,0x80,0x00,0x02,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x02,0x82,hv_DMHigher.I(),hv_DMLower.I(),0x00,0x00,0x02,nVal2,nVal1,nVal4,nVal3 };
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
		if (e.ErrorCode() == 5619)
		{
			//超时重新握手
			//ShakeHand();
		}
		return FALSE;
	}
}
BOOL CModbusTcpExt::WriteInt16(const int& nDMAddr, const int& nWriteVal)
{

	HTuple hv_DMLower, hv_DMHigher;
	TupleBand(nDMAddr, 0xFF, &hv_DMLower);
	TupleRsh(nDMAddr, 8, &hv_DMHigher);
	int a = hv_DMLower.I();
	int b = hv_DMHigher.I();
	short int  nVal1 = nWriteVal & 0xff;
	short int  nVal2 = (nWriteVal >> 8) & 0xff;


	int nSends[12] = { 0x00,0x00,0x00,0x00,
		0x00,0x06,
		0x01,
		0x06,
		hv_DMHigher.I(),hv_DMLower.I(),
		nVal2,nVal1 };
	HTuple hv_SendData(nSends, 12);
	HTuple hv_RecvData, hv_From;
	try
	{
		SendData(m_hv_Socket, "C12", hv_SendData, HTuple());
		/*ReceiveData(m_hv_Socket,"C30",&hv_RecvData,&hv_From);
		if (hv_RecvData.Length()!=30) return FALSE;
		if (hv_RecvData[hv_RecvData.Length()-1].I() != 0x40
		&& hv_RecvData[hv_RecvData.Length()-1].I() != 0x00)
		return FALSE;*/

		return TRUE;
	}
	catch (HException& e)
	{
		if (e.ErrorCode() == 5619)
		{
			//超时重新握手
			//ShakeHand();
			//close()
		}
		return FALSE;
	}
}
void  CModbusTcpExt::hex_to_float(const float& val, HTuple hvdata)
{
	char hex[4];
	char* p = (char*)(&val);
	HTuple hv_aa;
	TupleSelect(hvdata, 13, &hv_aa);
	hex[1] = hv_aa[0].I();
	TupleSelect(hvdata, 14, &hv_aa);
	hex[0] = hv_aa[0].I();

	TupleSelect(hvdata, 15, &hv_aa);
	hex[3] = hv_aa[0].I();
	TupleSelect(hvdata, 16, &hv_aa);
	hex[2] = hv_aa[0].I();
	*p = hex[0];
	*(p + 1) = hex[1];
	*(p + 2) = hex[2];
	*(p + 3) = hex[3];
}
BOOL CModbusTcpExt::WriteFloat(const int& nDMAddr, const float& dWriteVal)
{
	HTuple hv_DMLower, hv_DMHigher;
	TupleBand(nDMAddr, 0xFF, &hv_DMLower);
	TupleRsh(nDMAddr, 8, &hv_DMHigher);


	//unsigned char temp[4]; 
	//unsigned short i=0; 
	//float floatVariable=dWriteVal; 
	//unsigned char *pdata = (unsigned char *)&floatVariable;
	//for(i=0;i<4;i++)
	//{    
	//	temp[i+1] = *pdata++;//float转BYTE 
	//} 
	/*float xval;
	unsigned int  i;
	unsigned char *S;
	xval=dWriteVal;
	S= ( unsigned char* )( &xval );
	unsigned char *buff;
	for( i=0; i<4; i++ )
	{
	buff[i]=*(S+i);
	}*/
	float fexp = dWriteVal;
	byte bAry[4];
	byte* lpAry = bAry;
	*(float*)lpAry = fexp;
	/*int nVal1 = dWriteVal & 0xff;
	int nVal2 = (dWriteVal >> 8) & 0xff;
	int nVal3 = (nWriteVal >> 16) & 0xff;
	int nVal4 = (nWriteVal >> 24) & 0xff;*/
	int nSends[17] = { 0x00,0x00,0x00,0x00,
		0x00,0x0B,
		0x01,
		0x10,
		0x00,0x00,
		0x00,0x02,0x04,
		/*hv_DMHigher.I(),hv_DMLower.I(),*/
		//	0x00,0x00,0x41,0xDC };
		bAry[1],bAry[0],bAry[3],bAry[2] };
	float bb;
	//uint32_t abc;
	HTuple hv_SendData(nSends, 17);
	hex_to_float(bb, hv_SendData);

	HTuple hv_RecvData, hv_From;
	try
	{
		SendData(m_hv_Socket, "C17", hv_SendData, HTuple());
		/////*ReceiveData(m_hv_Socket,"C30",&hv_RecvData,&hv_From);
		////if (hv_RecvData.Length()!=30) return FALSE;
		////if (hv_RecvData[hv_RecvData.Length()-1].I() != 0x40 
		////	&& hv_RecvData[hv_RecvData.Length()-1].I() != 0x00) 
		////	return FALSE;*/

		return TRUE;
	}
	catch (HException& e)
	{
		if (e.ErrorCode() == 5619)
		{
			//超时重新握手
			//ShakeHand();
			//Connect(m_pOwner->m_tPLCSet.strPLCIP, m_pOwner->m_tPLCSet.nPLCPort); 
		}
		return FALSE;
	}

}

BOOL  CModbusTcpExt::ReadFloat(const int& nDMAddr, float& dReadData)
{
	if (FALSE == m_bConnected)
	{
		return FALSE;
	}
	CriticalSectionHelper lock(&m_cs);
	HTuple hv_readDataNum;
	HTuple hv_data, hv_To, hv_From;
	hv_data.Clear();
	hv_readDataNum.Clear();
	hv_readDataNum[0] = 00;
	hv_readDataNum[1] = 00;
	hv_readDataNum[2] = 00;
	hv_readDataNum[3] = 00;
	hv_readDataNum[4] = 00;
	hv_readDataNum[5] = 06;
	hv_readDataNum[6] = 01;
	hv_readDataNum[7] = 03;
	hv_readDataNum[8] = nDMAddr / 256;
	hv_readDataNum[9] = nDMAddr % 256;
	hv_readDataNum[10] = 00;
	hv_readDataNum[11] = 2;
	//读取总厂
	try
	{
		SendData(m_hv_Socket, "C12", hv_readDataNum, hv_To);
		ReceiveData(m_hv_Socket, "C13", &hv_data, &hv_From);
	}
	catch (HException& e)
	{
		if (e.ErrorCode() >= 5602 && e.ErrorCode() <= 5619)
		{
			m_bConnected = FALSE;
		}
		return FALSE;
	}

	float fval;
	hex_to_float(fval, hv_data);
	dReadData = fval;
	return TRUE;
}

BOOL CModbusTcpExt::ReadMultFloatData(const int& nDMAddr, std::vector<float>& vecWriteVal, int nNum)
{
	if (FALSE == m_bConnected)
	{
		return FALSE;
	}
	CriticalSectionHelper lock(&m_cs);
	/*************ReadMultData函数 一次读取多个寄存器的值********************
	nStartAddressIndex：读取的首个寄存器是第几个寄存器 0代表第一个
	nNum:读取几个寄存器
	hv_ReadValue：将读取到的值存储在该元组中*/
	vecWriteVal.clear();
	HTuple hv_RequestTuple;
	hv_RequestTuple.Clear();
	hv_RequestTuple.Append(0);
	hv_RequestTuple.Append(0);
	hv_RequestTuple.Append(0);
	hv_RequestTuple.Append(0);
	hv_RequestTuple.Append(0);
	hv_RequestTuple.Append(6);
	hv_RequestTuple.Append(1);
	hv_RequestTuple.Append(3);
	hv_RequestTuple.Append(nDMAddr / 256);
	hv_RequestTuple.Append(nDMAddr % 256);//起始地址
	hv_RequestTuple.Append(0);
	hv_RequestTuple.Append(nNum * 2);//多一个数据多两个寄存器
	try
	{

		HTuple hv_StrNum, hv_StrFomat;
		TupleString((nNum * 4) + 9, "d", &hv_StrNum);//每多一个寄存器需要长度增加4个
		hv_StrFomat = "C" + hv_StrNum;//接收数据的格式
		HTuple hv_To, hv_Data, hv_From;
		hv_To = HTuple();
		hv_Data = HTuple();
		SendData(m_hv_Socket, "C12", hv_RequestTuple, hv_To);
		ReceiveData(m_hv_Socket, hv_StrFomat, &hv_Data, &hv_From);

		float fReadData;
		for (int i = 0; i<(hv_Data.TupleLength() - 9) / 4; i++)
		{
			float fval;
			char hex[4];
			char* p = (char*)(&fval);
			HTuple hv_TempData;
			int nStartIndex = 9;
			TupleSelect(hv_Data, nStartIndex + 4 * i + 0, &hv_TempData);
			hex[1] = hv_TempData[0].I();
			TupleSelect(hv_Data, nStartIndex + 4 * i + 1, &hv_TempData);
			hex[0] = hv_TempData[0].I();
			TupleSelect(hv_Data, nStartIndex + 4 * i + 2, &hv_TempData);
			hex[3] = hv_TempData[0].I();
			TupleSelect(hv_Data, nStartIndex + 4 * i + 3, &hv_TempData);
			hex[2] = hv_TempData[0].I();
			*p = hex[0];
			*(p + 1) = hex[1];
			*(p + 2) = hex[2];
			*(p + 3) = hex[3];
			vecWriteVal.push_back(fval);
		}
	}
	catch (HException e)
	{
		if (e.ErrorCode() >= 5602 && e.ErrorCode() <= 5619)
		{
			m_bConnected = FALSE;
		}
		return FALSE;
	}
	return TRUE;
}


BOOL CModbusTcpExt::WriteMultFloatData(const int& nDMAddr, std::vector<float>& vecWriteVal)
{
	if (FALSE == m_bConnected)
	{
		return FALSE;
	}
	CriticalSectionHelper lock(&m_cs);
	/*************WriteMultFloatData函数*一次写多个数值到寄存器中*******************
	nStartAddressIndex:写入的首个寄存器是第几个寄存器 0代表第一个 默认都是0
	nNum:写入寄存器的数目 默认都是1
	hv_WriteValue：要写入寄存器的数值*/
	HTuple hv_WriteTuple;
	hv_WriteTuple.Clear();
	hv_WriteTuple.Append(0);
	hv_WriteTuple.Append(0);
	hv_WriteTuple.Append(0);
	hv_WriteTuple.Append(0);
	hv_WriteTuple.Append(0);
	hv_WriteTuple.Append(7 + (int)vecWriteVal.size() * 4);//发送一个数据时是 该参数值：11
	hv_WriteTuple.Append(1);
	hv_WriteTuple.Append(16);
	hv_WriteTuple.Append(nDMAddr / 256);
	hv_WriteTuple.Append(nDMAddr % 256);//从第几个寄存器开始写0代表第一个起始地址
	hv_WriteTuple.Append(0);
	hv_WriteTuple.Append((int)vecWriteVal.size() * 2);//写寄存器数量
	hv_WriteTuple.Append((int)vecWriteVal.size() * 4);//写数据长度
	for (int k = 0; k<vecWriteVal.size(); k++)
	{

		float fexp = vecWriteVal[k];
		byte bAry[4];
		byte* lpAry = bAry;
		*(float*)lpAry = fexp;
		hv_WriteTuple.Append(bAry[1]);
		hv_WriteTuple.Append(bAry[0]);
		hv_WriteTuple.Append(bAry[3]);
		hv_WriteTuple.Append(bAry[2]);
	}

	HTuple hv_To = HTuple();
	HTuple hv_StrNum, hv_StrFomat;
	TupleString(((int)vecWriteVal.size() * 4) + 13, "d", &hv_StrNum);//每多一个寄存器需要长度增加2个
	hv_StrFomat = "C" + hv_StrNum;
	try
	{
		HTuple hv_Data, hv_From;
		SendData(m_hv_Socket, hv_StrFomat, hv_WriteTuple, hv_To);
		ReceiveData(m_hv_Socket, "C12", &hv_Data, &hv_From);
	}
	catch (HException e)
	{
		if (e.ErrorCode() >= 5602 && e.ErrorCode() <= 5619)
		{
			m_bConnected = FALSE;
		}
		return FALSE;
	}
	return TRUE;
}