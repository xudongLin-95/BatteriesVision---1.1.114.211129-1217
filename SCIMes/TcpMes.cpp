#include "StdAfx.h"
#include "TcpMes.h"

CTcpMes* CTcpMes::m_instance = NULL;

CTcpMes::CTcpMes()
{

}

CTcpMes::~CTcpMes()
{

}

//����ģʽ
CTcpMes& CTcpMes::getInstance()
{
	if (NULL == m_instance)
	{
		m_instance = new CTcpMes();
	}
	return *m_instance;
}

//����MES������
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
		SetSocketParam(m_hv_SocketHandle, "timeout", 100);//�������ӳ�ʱʱ��
		m_bConnected = TRUE;
	}
	catch (HException& e)
	{
		CString strMss;
		strMss.Format("socket�޷�����Mesϵͳ:%d  %s", e.ErrorCode(), e.ErrorMessage().Text());
		//m_sInit.pLogView->DoSomething("EXCEPTION", (void*)(LPCSTR)strMss, NULL);
		m_bConnected = FALSE;
		return FALSE;
	}
	return TRUE;
}

//��һ���֣���MES��������������
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
		cstrMsg.Format("��ʼ���ã�,%s", strTime.c_str());
		//m_pSoapMesLog->HeatBeatLog(cstrMsg);

		cstrMsg.Format("���ͣ�,%s", nebCJsonSend.ToString().c_str());
		//m_pSoapMesLog->HeatBeatLog(cstrMsg);

		nebCJsonReciev.Get("Ret", nRes);
		cstrMsg.Format("�������ã�,%s, ���ã�%s , ������ ��%d", strTime.c_str(), SOAP_OK == nRes ? "�ɹ�" : "ʧ��", nCode);
		//m_pSoapMesLog->HeatBeatLog(cstrMsg);

		cstrMsg.Format("���գ�,%s", nebCJsonReciev.ToString().c_str());
		//m_pSoapMesLog->HeatBeatLog(cstrMsg);
	}
	return res;
}
//����������
bool CTcpMes:: DoHeartBeat(tagMesHead mesHead, neb::CJsonObject & cJsonSend, neb::CJsonObject& cJsonRcv)
{
	neb::CJsonObject nebCJsonHeader, nebCJsonBody,nebCJsonReceice;
	CLockHelper lockHelp(&m_csSoap);
	time_t sendTime;
	time(&sendTime);
	cJsonSend.Clear();
	cJsonRcv.Clear();

	//���͵�����  תΪjson ����ȥ�������¼��־
	{
		//����ͷ
		nebCJsonHeader.Add("ProtovolNO", mesHead.m_ProtocolNO.GetBuffer());
		nebCJsonHeader.Add("MsgNO", mesHead.m_MsgNO);
		nebCJsonHeader.Add("NeedReply", mesHead.m_NeedReply);
		nebCJsonHeader.Add("CmdID", mesHead.m_CmdID.GetBuffer());
		nebCJsonHeader.Add("EID", mesHead.m_EID.GetBuffer());
		//���ݲ���
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

//�ڶ����֣�״̬���ݵĲɼ����ϴ�
int CTcpMes::Send_EquipmentStateData(tagMesHead mesHead)//��ʵ��
{
	neb::CJsonObject nebCJsonSend, nebCJsonReciev;
	int res = -101;

	res = DoEquipmentStateData(mesHead, nebCJsonSend, nebCJsonReciev);
	{
		CString cstrMsg;
		std::string strTime;
		int nRes(-102), nCode(-103);
		nebCJsonSend.Get("SendTime", strTime);
		cstrMsg.Format("��ʼ���ã�,%s", strTime.c_str());
		//m_pSoapMesLog->HeatBeatLog(cstrMsg);

		cstrMsg.Format("���ͣ�,%s", nebCJsonSend.ToString().c_str());
		//m_pSoapMesLog->HeatBeatLog(cstrMsg);

		nebCJsonReciev.Get("Ret", nRes);
		cstrMsg.Format("�������ã�,%s, ���ã�%s , ������ ��%d", strTime.c_str(), SOAP_OK == nRes ? "�ɹ�" : "ʧ��", nCode);
		//m_pSoapMesLog->HeatBeatLog(cstrMsg);

		cstrMsg.Format("���գ�,%s", nebCJsonReciev.ToString().c_str());
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

	//���͵�����  תΪjson ����ȥ�������¼��־
	{
		//����ͷ
		nebCJsonHeader.Add("ProtovolNO", mesHead.m_ProtocolNO.GetBuffer());
		nebCJsonHeader.Add("MsgNO", mesHead.m_MsgNO);
		nebCJsonHeader.Add("NeedReply", mesHead.m_NeedReply);
		nebCJsonHeader.Add("CmdID", mesHead.m_CmdID.GetBuffer());
		nebCJsonHeader.Add("EID", mesHead.m_EID.GetBuffer());
		//���ݲ���
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

//�������ֹ������ݲɼ�
int CTcpMes::Send_ProcessData1(tagMesHead mesHead)//�����������
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
		cstrMsg.Format("��ʼ���ã�,%s", strTime.c_str());
		//m_pSoapMesLog->HeatBeatLog(cstrMsg);

		cstrMsg.Format("���ͣ�,%s", nebCJsonSend.ToString().c_str());
		//m_pSoapMesLog->HeatBeatLog(cstrMsg);

		nebCJsonReciev.Get("Ret", nRes);
		cstrMsg.Format("�������ã�,%s, ���ã�%s , ������ ��%d", strTime.c_str(), SOAP_OK == nRes ? "�ɹ�" : "ʧ��", nCode);
		//m_pSoapMesLog->HeatBeatLog(cstrMsg);

		cstrMsg.Format("���գ�,%s", nebCJsonReciev.ToString().c_str());
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

	//���͵�����  תΪjson ����ȥ�������¼��־
	{
		//����ͷ
		nebCJsonHeader.Add("ProtovolNO", mesHead.m_ProtocolNO.GetBuffer());
		nebCJsonHeader.Add("MsgNO", mesHead.m_MsgNO);
		nebCJsonHeader.Add("NeedReply", mesHead.m_NeedReply);
		nebCJsonHeader.Add("CmdID", mesHead.m_CmdID.GetBuffer());
		nebCJsonHeader.Add("EID", mesHead.m_EID.GetBuffer());
		//���ݲ���
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
//int Send_ProcessData2();//��ȼ������
//int Send_ProcessData3();//��Ƭ�ļ�

//���Ĳ��ֱ������ݲɼ�
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
		cstrMsg.Format("��ʼ���ã�,%s", strTime.c_str());
		//m_pSoapMesLog->HeatBeatLog(cstrMsg);

		cstrMsg.Format("���ͣ�,%s", nebCJsonSend.ToString().c_str());
		//m_pSoapMesLog->HeatBeatLog(cstrMsg);

		nebCJsonReciev.Get("Ret", nRes);
		cstrMsg.Format("�������ã�,%s, ���ã�%s , ������ ��%d", strTime.c_str(), SOAP_OK == nRes ? "�ɹ�" : "ʧ��", nCode);
		//m_pSoapMesLog->HeatBeatLog(cstrMsg);

		cstrMsg.Format("���գ�,%s", nebCJsonReciev.ToString().c_str());
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

	//���͵�����  תΪjson ����ȥ�������¼��־
	{
		//����ͷ
		nebCJsonHeader.Add("ProtovolNO", mesHead.m_ProtocolNO.GetBuffer());
		nebCJsonHeader.Add("MsgNO", mesHead.m_MsgNO);
		nebCJsonHeader.Add("NeedReply", mesHead.m_NeedReply);
		nebCJsonHeader.Add("CmdID", mesHead.m_CmdID.GetBuffer());
		nebCJsonHeader.Add("EID", mesHead.m_EID.GetBuffer());
		//���ݲ���
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

//���岿���豸�䷽��������
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
		cstrMsg.Format("��ʼ���ã�,%s", strTime.c_str());
		//m_pSoapMesLog->HeatBeatLog(cstrMsg);

		cstrMsg.Format("���ͣ�,%s", nebCJsonSend.ToString().c_str());
		//m_pSoapMesLog->HeatBeatLog(cstrMsg);

		nebCJsonReciev.Get("Ret", nRes);
		cstrMsg.Format("�������ã�,%s, ���ã�%s , ������ ��%d", strTime.c_str(), SOAP_OK == nRes ? "�ɹ�" : "ʧ��", nCode);
		//m_pSoapMesLog->HeatBeatLog(cstrMsg);

		cstrMsg.Format("���գ�,%s", nebCJsonReciev.ToString().c_str());
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

	//���͵�����  תΪjson ����ȥ�������¼��־
	{
		//����ͷ
		nebCJsonHeader.Add("ProtovolNO", mesHead.m_ProtocolNO.GetBuffer());
		nebCJsonHeader.Add("MsgNO", mesHead.m_MsgNO);
		nebCJsonHeader.Add("NeedReply", mesHead.m_NeedReply);
		nebCJsonHeader.Add("CmdID", mesHead.m_CmdID.GetBuffer());
		nebCJsonHeader.Add("EID", mesHead.m_EID.GetBuffer());
		//���ݲ���
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

//���������豸�����䷽����У��
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
		cstrMsg.Format("��ʼ���ã�,%s", strTime.c_str());
		//m_pSoapMesLog->HeatBeatLog(cstrMsg);

		cstrMsg.Format("���ͣ�,%s", nebCJsonSend.ToString().c_str());
		//m_pSoapMesLog->HeatBeatLog(cstrMsg);

		nebCJsonReciev.Get("Ret", nRes);
		cstrMsg.Format("�������ã�,%s, ���ã�%s , ������ ��%d", strTime.c_str(), SOAP_OK == nRes ? "�ɹ�" : "ʧ��", nCode);
		//m_pSoapMesLog->HeatBeatLog(cstrMsg);

		cstrMsg.Format("���գ�,%s", nebCJsonReciev.ToString().c_str());
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

	//���͵�����  תΪjson ����ȥ�������¼��־
	{
		//����ͷ
		nebCJsonHeader.Add("ProtovolNO", mesHead.m_ProtocolNO.GetBuffer());
		nebCJsonHeader.Add("MsgNO", mesHead.m_MsgNO);
		nebCJsonHeader.Add("NeedReply", mesHead.m_NeedReply);
		nebCJsonHeader.Add("CmdID", mesHead.m_CmdID.GetBuffer());
		nebCJsonHeader.Add("EID", mesHead.m_EID.GetBuffer());
		//���ݲ���
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