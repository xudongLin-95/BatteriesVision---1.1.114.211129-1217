#include "StdAfx.h"
#include "TcpMes.h"

CTcpMes* CTcpMes::m_instance = NULL;

CTcpMes::CTcpMes()
{

}

CTcpMes::~CTcpMes()
{

}

//单例模式
CTcpMes& CTcpMes::getInstance()
{
	if (NULL == m_instance)
	{
		m_instance = new CTcpMes();
	}
	return *m_instance;
}

//连接MES服务器
int CTcpMes::ConnectToServer(CString IP, int Port , CString strProtocol)
{
	int nTimeOut = 30.0;
	try
	{
		try
		{
			CloseSocket(m_hv_SocketHandle);
		}
		catch (HException){}

		OpenSocketConnect(HTuple(IP), Port, (HTuple("protocol")).Append("timeout"), (HTuple(strProtocol)).Append(nTimeOut), &m_hv_SocketHandle);
		SetSocketParam(m_hv_SocketHandle, "timeout", 100);//设置连接超时时间
		m_bConnected = TRUE;
	}
	catch (HException& e)
	{
		CString strMss;
		strMss.Format("socket无法连接Mes系统:%d  %s", e.ErrorCode(), e.ErrorMessage().Text());
		//m_sInit.pLogView->DoSomething("EXCEPTION", (void*)(LPCSTR)strMss, NULL);
		m_bConnected = FALSE;
		return FALSE;
	}
	return TRUE;
}

//第一部分：向MES服务器发送心跳
int CTcpMes::Send_Heatbeat(tagMesHead mesHead)
{
	neb::CJsonObject nebCJsonSend, nebCJsonReciev;
	int res = -101;

	res = DoHeartBeat(mesHead, nebCJsonSend, nebCJsonReciev);
	{
		CString cstrMsg;
		std::string strTime;
		int nRes(-102), nCode(-103);
		nebCJsonSend.Get("SendTime", strTime);
		cstrMsg.Format("开始调用：,%s", strTime.c_str());
		//m_pSoapMesLog->HeatBeatLog(cstrMsg);

		cstrMsg.Format("发送：,%s", nebCJsonSend.ToString().c_str());
		//m_pSoapMesLog->HeatBeatLog(cstrMsg);

		nebCJsonReciev.Get("Ret", nRes);
		cstrMsg.Format("结束调用：,%s, 调用：%s , 返回码 ：%d", strTime.c_str(), SOAP_OK == nRes ? "成功" : "失败", nCode);
		//m_pSoapMesLog->HeatBeatLog(cstrMsg);

		cstrMsg.Format("接收：,%s", nebCJsonReciev.ToString().c_str());
		//m_pSoapMesLog->HeatBeatLog(cstrMsg);
	}
	return res;
}
//心跳处理函数
bool CTcpMes:: DoHeartBeat(tagMesHead mesHead, neb::CJsonObject & cJsonSend, neb::CJsonObject& cJsonRcv)
{
	neb::CJsonObject nebCJsonHeader, nebCJsonBody,nebCJsonReceice;
	CLockHelper lockHelp(&m_csSoap);
	time_t sendTime;
	time(&sendTime);
	cJsonSend.Clear();
	cJsonRcv.Clear();

	//发送的内容  转为json 传出去，方便记录日志
	{
		//公共头
		nebCJsonHeader.Add("ProtovolNO", mesHead.m_ProtocolNO.GetBuffer());
		nebCJsonHeader.Add("MsgNO", mesHead.m_MsgNO);
		nebCJsonHeader.Add("NeedReply", mesHead.m_NeedReply);
		nebCJsonHeader.Add("CmdID", mesHead.m_CmdID.GetBuffer());
		nebCJsonHeader.Add("EID", mesHead.m_EID.GetBuffer());
		//数据部分
		nebCJsonBody.Add("Heartbeat", "1");

		cJsonSend.Add(nebCJsonHeader);
		cJsonSend.AddEmptySubArray("Data");
		cJsonSend["Data"].Add(nebCJsonBody);
	}

	CString strFormat;
	strFormat.Format("C%d", cJsonSend.ToString().size());
	CString sData;
	sData = cJsonSend.ToString().c_str();
	SendData(m_hv_SocketHandle, HTuple(strFormat), sData.GetBuffer(), HTuple());

	HTuple hv_RecvData, hv_From;
	ReceiveData(m_hv_SocketHandle, "C200", &hv_RecvData, &hv_From);

	CString rData;
	rData.Format("%s",hv_RecvData.ToString().Text());
	nebCJsonReceice(rData.GetBuffer());

	cJsonRcv = nebCJsonReceice;

	return true;
}

//第二部分：状态数据的采集及上传
int CTcpMes::Send_EquipmentStateData(tagMesHead mesHead)//待实现
{
	neb::CJsonObject nebCJsonSend, nebCJsonReciev;
	int res = -101;

	res = DoEquipmentStateData(mesHead, nebCJsonSend, nebCJsonReciev);
	{
		CString cstrMsg;
		std::string strTime;
		int nRes(-102), nCode(-103);
		nebCJsonSend.Get("SendTime", strTime);
		cstrMsg.Format("开始调用：,%s", strTime.c_str());
		//m_pSoapMesLog->HeatBeatLog(cstrMsg);

		cstrMsg.Format("发送：,%s", nebCJsonSend.ToString().c_str());
		//m_pSoapMesLog->HeatBeatLog(cstrMsg);

		nebCJsonReciev.Get("Ret", nRes);
		cstrMsg.Format("结束调用：,%s, 调用：%s , 返回码 ：%d", strTime.c_str(), SOAP_OK == nRes ? "成功" : "失败", nCode);
		//m_pSoapMesLog->HeatBeatLog(cstrMsg);

		cstrMsg.Format("接收：,%s", nebCJsonReciev.ToString().c_str());
		//m_pSoapMesLog->HeatBeatLog(cstrMsg);
	}
	return true;
}

bool CTcpMes:: DoEquipmentStateData(tagMesHead mesHead, neb::CJsonObject & cJsonSend, neb::CJsonObject& cJsonRcv)
{
	neb::CJsonObject nebCJsonHeader, nebCJsonBody, nebCJsonReceice;
	CLockHelper lockHelp(&m_csSoap);
	time_t sendTime;
	time(&sendTime);
	cJsonSend.Clear();
	cJsonRcv.Clear();

	//发送的内容  转为json 传出去，方便记录日志
	{
		//公共头
		nebCJsonHeader.Add("ProtovolNO", mesHead.m_ProtocolNO.GetBuffer());
		nebCJsonHeader.Add("MsgNO", mesHead.m_MsgNO);
		nebCJsonHeader.Add("NeedReply", mesHead.m_NeedReply);
		nebCJsonHeader.Add("CmdID", mesHead.m_CmdID.GetBuffer());
		nebCJsonHeader.Add("EID", mesHead.m_EID.GetBuffer());
		//数据部分
		nebCJsonBody.Add("Code", "LXS0101040001");
		nebCJsonBody.Add("State", 1);
		nebCJsonBody.Add("Datetime", "2017-08-30 23:33:33");
		nebCJsonBody.Add("StopReasonID", 1);

		cJsonSend.Add(nebCJsonHeader);
		cJsonSend.AddEmptySubArray("Data");
		cJsonSend["Data"].Add(nebCJsonBody);
	}

	CString strFormat;
	strFormat.Format("C%d", cJsonSend.ToString().size());
	CString sData;
	sData = cJsonSend.ToString().c_str();
	SendData(m_hv_SocketHandle, HTuple(strFormat), sData.GetBuffer(), HTuple());

	HTuple hv_RecvData, hv_From;
	ReceiveData(m_hv_SocketHandle, "C200", &hv_RecvData, &hv_From);

	CString rData;
	rData.Format("%s", hv_RecvData.ToString().Text());
	cJsonRcv=nebCJsonReceice(rData.GetBuffer());

	return true;
}

//第三部分过程数据采集
int CTcpMes::Send_ProcessData1(tagMesHead mesHead)//常规过程数据
{
	neb::CJsonObject nebCJsonSend, nebCJsonReciev;
	bool isOnline;
	int res = -101;

	res = DoProcessData1(mesHead, nebCJsonSend, nebCJsonReciev);
	{
		CString cstrMsg;
		std::string strTime;
		int nRes(-102), nCode(-103);
		nebCJsonSend.Get("SendTime", strTime);
		cstrMsg.Format("开始调用：,%s", strTime.c_str());
		//m_pSoapMesLog->HeatBeatLog(cstrMsg);

		cstrMsg.Format("发送：,%s", nebCJsonSend.ToString().c_str());
		//m_pSoapMesLog->HeatBeatLog(cstrMsg);

		nebCJsonReciev.Get("Ret", nRes);
		cstrMsg.Format("结束调用：,%s, 调用：%s , 返回码 ：%d", strTime.c_str(), SOAP_OK == nRes ? "成功" : "失败", nCode);
		//m_pSoapMesLog->HeatBeatLog(cstrMsg);

		cstrMsg.Format("接收：,%s", nebCJsonReciev.ToString().c_str());
		//m_pSoapMesLog->HeatBeatLog(cstrMsg);
	}

	return true;
}
bool CTcpMes:: DoProcessData1(tagMesHead mesHead, neb::CJsonObject & cJsonSend, neb::CJsonObject& cJsonRcv)
{
	neb::CJsonObject nebCJsonHeader, nebCJsonBody, nebCJsonReceice;
	CLockHelper lockHelp(&m_csSoap);
	time_t sendTime;
	time(&sendTime);
	cJsonSend.Clear();
	cJsonRcv.Clear();

	//发送的内容  转为json 传出去，方便记录日志
	{
		//公共头
		nebCJsonHeader.Add("ProtovolNO", mesHead.m_ProtocolNO.GetBuffer());
		nebCJsonHeader.Add("MsgNO", mesHead.m_MsgNO);
		nebCJsonHeader.Add("NeedReply", mesHead.m_NeedReply);
		nebCJsonHeader.Add("CmdID", mesHead.m_CmdID.GetBuffer());
		nebCJsonHeader.Add("EID", mesHead.m_EID.GetBuffer());
		//数据部分
		nebCJsonBody.Add("Code", "LXS0101040001");
		nebCJsonBody.Add("ProductionCount", 1000);
		nebCJsonBody.Add("Speed", 1000);
		nebCJsonBody.Add("UnWindTensionShow", 1000);
		nebCJsonBody.Add("WindingTensionShow", 1000);
		nebCJsonBody.Add("Datetime", "2017-08-30 23:33:33");
		
		cJsonSend.Add(nebCJsonHeader);
		cJsonSend.AddEmptySubArray("Data");
		cJsonSend["Data"].Add(nebCJsonBody);
	}

	CString strFormat;
	strFormat.Format("C%d", cJsonSend.ToString().size());
	CString sData;
	sData = cJsonSend.ToString().c_str();
	SendData(m_hv_SocketHandle, HTuple(strFormat), sData.GetBuffer(), HTuple());

	HTuple hv_RecvData, hv_From;
	ReceiveData(m_hv_SocketHandle, "C200", &hv_RecvData, &hv_From);

	CString rData;
	rData.Format("%s", hv_RecvData.ToString().Text());
	cJsonRcv = nebCJsonReceice(rData.GetBuffer());

	return true;
}
//int Send_ProcessData2();//厚度监测数据
//int Send_ProcessData3();//照片文件

//第四部分报警数据采集
int CTcpMes::Send_EquipmentAlarmMessage(tagMesHead mesHead)
{
	neb::CJsonObject nebCJsonSend, nebCJsonReciev;
	bool isOnline;
	int res = -101;

	res = DoEquipmentAlarmMessage(mesHead, nebCJsonSend, nebCJsonReciev);
	{
		CString cstrMsg;
		std::string strTime;
		int nRes(-102), nCode(-103);
		nebCJsonSend.Get("SendTime", strTime);
		cstrMsg.Format("开始调用：,%s", strTime.c_str());
		//m_pSoapMesLog->HeatBeatLog(cstrMsg);

		cstrMsg.Format("发送：,%s", nebCJsonSend.ToString().c_str());
		//m_pSoapMesLog->HeatBeatLog(cstrMsg);

		nebCJsonReciev.Get("Ret", nRes);
		cstrMsg.Format("结束调用：,%s, 调用：%s , 返回码 ：%d", strTime.c_str(), SOAP_OK == nRes ? "成功" : "失败", nCode);
		//m_pSoapMesLog->HeatBeatLog(cstrMsg);

		cstrMsg.Format("接收：,%s", nebCJsonReciev.ToString().c_str());
		//m_pSoapMesLog->HeatBeatLog(cstrMsg);
	}

	return true;
}
bool CTcpMes::DoEquipmentAlarmMessage(tagMesHead mesHead, neb::CJsonObject & cJsonSend, neb::CJsonObject& cJsonRcv)
{
	neb::CJsonObject nebCJsonHeader, nebCJsonBody, nebCJsonReceice;
	CLockHelper lockHelp(&m_csSoap);
	time_t sendTime;
	time(&sendTime);
	cJsonSend.Clear();
	cJsonRcv.Clear();

	//发送的内容  转为json 传出去，方便记录日志
	{
		//公共头
		nebCJsonHeader.Add("ProtovolNO", mesHead.m_ProtocolNO.GetBuffer());
		nebCJsonHeader.Add("MsgNO", mesHead.m_MsgNO);
		nebCJsonHeader.Add("NeedReply", mesHead.m_NeedReply);
		nebCJsonHeader.Add("CmdID", mesHead.m_CmdID.GetBuffer());
		nebCJsonHeader.Add("EID", mesHead.m_EID.GetBuffer());
		//数据部分
		nebCJsonBody.Add("Code", "LXS0101040001");

		nebCJsonBody.Add("AbnormalID", "0111");
		nebCJsonBody.Add("Abnormal", "0");
		nebCJsonBody.Add("Datetime", "2017-08-30 23:33:33");

		cJsonSend.Add(nebCJsonHeader);
		cJsonSend.AddEmptySubArray("Data");
		cJsonSend["Data"].Add(nebCJsonBody);
	}

	CString strFormat;
	strFormat.Format("C%d", cJsonSend.ToString().size());
	CString sData;
	sData = cJsonSend.ToString().c_str();
	SendData(m_hv_SocketHandle, HTuple(strFormat), sData.GetBuffer(), HTuple());

	HTuple hv_RecvData, hv_From;
	ReceiveData(m_hv_SocketHandle, "C200", &hv_RecvData, &hv_From);

	CString rData;
	rData.Format("%s", hv_RecvData.ToString().Text());
	cJsonRcv = nebCJsonReceice(rData.GetBuffer());

	return true;
}

//第五部分设备配方数据请求
int CTcpMes::Get_EquipmentRecipes(tagMesHead mesHead)
{
	neb::CJsonObject nebCJsonSend, nebCJsonReciev;
	bool isOnline;
	int res = -101;

	res = DoEquipmentRecipes(mesHead, nebCJsonSend, nebCJsonReciev);
	{
		CString cstrMsg;
		std::string strTime;
		int nRes(-102), nCode(-103);
		nebCJsonSend.Get("SendTime", strTime);
		cstrMsg.Format("开始调用：,%s", strTime.c_str());
		//m_pSoapMesLog->HeatBeatLog(cstrMsg);

		cstrMsg.Format("发送：,%s", nebCJsonSend.ToString().c_str());
		//m_pSoapMesLog->HeatBeatLog(cstrMsg);

		nebCJsonReciev.Get("Ret", nRes);
		cstrMsg.Format("结束调用：,%s, 调用：%s , 返回码 ：%d", strTime.c_str(), SOAP_OK == nRes ? "成功" : "失败", nCode);
		//m_pSoapMesLog->HeatBeatLog(cstrMsg);

		cstrMsg.Format("接收：,%s", nebCJsonReciev.ToString().c_str());
		//m_pSoapMesLog->HeatBeatLog(cstrMsg);
	}

	return true;
}
bool CTcpMes:: DoEquipmentRecipes(tagMesHead mesHead, neb::CJsonObject & cJsonSend, neb::CJsonObject& cJsonRcv)
{
	neb::CJsonObject nebCJsonHeader, nebCJsonBody, nebCJsonReceice;
	CLockHelper lockHelp(&m_csSoap);
	time_t sendTime;
	time(&sendTime);
	cJsonSend.Clear();
	cJsonRcv.Clear();

	//发送的内容  转为json 传出去，方便记录日志
	{
		//公共头
		nebCJsonHeader.Add("ProtovolNO", mesHead.m_ProtocolNO.GetBuffer());
		nebCJsonHeader.Add("MsgNO", mesHead.m_MsgNO);
		nebCJsonHeader.Add("NeedReply", mesHead.m_NeedReply);
		nebCJsonHeader.Add("CmdID", mesHead.m_CmdID.GetBuffer());
		nebCJsonHeader.Add("EID", mesHead.m_EID.GetBuffer());
		//数据部分
		nebCJsonBody.Add("Code", "LXS0101040001");
		nebCJsonBody.Add("Workorder", "GD-0001");
		nebCJsonBody.Add("Datetime", "2017-08-30 23:33:33");

		cJsonSend.Add(nebCJsonHeader);
		cJsonSend.AddEmptySubArray("Data");
		cJsonSend["Data"].Add(nebCJsonBody);
	}

	CString strFormat;
	strFormat.Format("C%d", cJsonSend.ToString().size());
	CString sData;
	sData = cJsonSend.ToString().c_str();
	SendData(m_hv_SocketHandle, HTuple(strFormat), sData.GetBuffer(), HTuple());

	HTuple hv_RecvData, hv_From;
	ReceiveData(m_hv_SocketHandle, "C200", &hv_RecvData, &hv_From);

	CString rData;
	rData.Format("%s", hv_RecvData.ToString().Text());
	cJsonRcv = nebCJsonReceice(rData.GetBuffer());

	return true;
}

//第六部分设备开机配方参数校验
int CTcpMes::Check_EquipmentRecipes(tagMesHead mesHead)
{
	neb::CJsonObject nebCJsonSend, nebCJsonReciev;
	bool isOnline;
	int res = -101;

	res = DoCheckEquipmentRecipes(mesHead, nebCJsonSend, nebCJsonReciev);
	{
		CString cstrMsg;
		std::string strTime;
		int nRes(-102), nCode(-103);
		nebCJsonSend.Get("SendTime", strTime);
		cstrMsg.Format("开始调用：,%s", strTime.c_str());
		//m_pSoapMesLog->HeatBeatLog(cstrMsg);

		cstrMsg.Format("发送：,%s", nebCJsonSend.ToString().c_str());
		//m_pSoapMesLog->HeatBeatLog(cstrMsg);

		nebCJsonReciev.Get("Ret", nRes);
		cstrMsg.Format("结束调用：,%s, 调用：%s , 返回码 ：%d", strTime.c_str(), SOAP_OK == nRes ? "成功" : "失败", nCode);
		//m_pSoapMesLog->HeatBeatLog(cstrMsg);

		cstrMsg.Format("接收：,%s", nebCJsonReciev.ToString().c_str());
		//m_pSoapMesLog->HeatBeatLog(cstrMsg);
	}

	return true;
}
bool CTcpMes:: DoCheckEquipmentRecipes(tagMesHead mesHead, neb::CJsonObject & cJsonSend, neb::CJsonObject& cJsonRcv)
{
	neb::CJsonObject nebCJsonHeader, nebCJsonBody, nebCJsonReceice;
	CLockHelper lockHelp(&m_csSoap);
	time_t sendTime;
	time(&sendTime);
	cJsonSend.Clear();
	cJsonRcv.Clear();

	//发送的内容  转为json 传出去，方便记录日志
	{
		//公共头
		nebCJsonHeader.Add("ProtovolNO", mesHead.m_ProtocolNO.GetBuffer());
		nebCJsonHeader.Add("MsgNO", mesHead.m_MsgNO);
		nebCJsonHeader.Add("NeedReply", mesHead.m_NeedReply);
		nebCJsonHeader.Add("CmdID", mesHead.m_CmdID.GetBuffer());
		nebCJsonHeader.Add("EID", mesHead.m_EID.GetBuffer());
		//数据部分
		nebCJsonBody.Add("Code", "LXS0101040001");
		nebCJsonBody.Add("Workorder", "GD-0001");
		nebCJsonBody.Add("Version", "V3.0");

		cJsonSend.Add(nebCJsonHeader);
		cJsonSend.AddEmptySubArray("Data");
		cJsonSend["Data"].Add(nebCJsonBody);
	}

	CString strFormat;
	strFormat.Format("C%d", cJsonSend.ToString().size());
	CString sData;
	sData = cJsonSend.ToString().c_str();
	SendData(m_hv_SocketHandle, HTuple(strFormat), sData.GetBuffer(), HTuple());

	HTuple hv_RecvData, hv_From;
	ReceiveData(m_hv_SocketHandle, "C200", &hv_RecvData, &hv_From);

	CString rData;
	rData.Format("%s", hv_RecvData.ToString().Text());
	cJsonRcv = nebCJsonReceice(rData.GetBuffer());

	return true;
}