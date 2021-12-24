#include "StdAfx.h"
#include "Upload.h"

#define EPS 1e-12	//���弫�ޣ������ж�����double�Ƿ����

CUpload::CUpload() :
	m_vecDataCollect(1)
	, m_vecRawData(1)
{
	m_bConnect = FALSE;
	m_hvSeconds = 0;
	m_hvFirstSend = TRUE;
	m_vecDataCollect.Clear();

	CString strCounterPath;
	strCounterPath.Format("%s\\Config\\Public\\Count\\Count0.ini", GetCurrentAppPath());
	m_strBatchRecv = GetPrivateProfileCString("Batch", "Name", "None", strCounterPath);
}
CUpload::~CUpload()
{
	//WriteParam();
}
void T_ServerSocket()//��ͷ��ʱ�����
{
	if (!theGlobal.m_Upload.m_tParam.bUseDataUpload) return;
	try
	{
		for (int i = 0; i < MAX_CONNECT; i++)
		{
			if(!theGlobal.m_Upload.m_tParam.bListenClient[i]) continue;

			theGlobal.m_Upload.m_bListenSuccess[i] = FALSE;
			theGlobal.m_Upload.m_bConnectClient[i] = FALSE;
			theGlobal.m_Upload.m_bSendBatchOK[i] = TRUE;
			theGlobal.m_Upload.m_bSendCheckProductInfo[i] = TRUE;

			CString strLog;
			strLog.Format("������λ%d�ļ���:%s:%d", i + 2, theGlobal.m_Upload.m_tParam.strAddressListen[i], theGlobal.m_Upload.m_tParam.nPortListen[i]);
			theGlobal.m_Log.Communication(strLog);
		}
	}
	catch (HException e)
	{

	}
	while (FALSE == theGlobal.m_bExitThread)
	{
		for (int i = 0; i < MAX_CONNECT; i++)
		{
			try
			{
				if (!theGlobal.m_Upload.m_tParam.bListenClient[i]) continue;

				if (theGlobal.m_Upload.m_bListenSuccess[i] == FALSE)
				{
					//�����Ͽ������´򿪼���
					HALCON_TRY(CloseSocket(theGlobal.m_Upload.m_ClientSocket[i]));
					HALCON_TRY(CloseSocket(theGlobal.m_Upload.m_ServerSocket[i]));
					theGlobal.m_Upload.m_bConnectClient[i] = FALSE;
					try
					{
						HTuple hvName, hvValue;
						hvName.Append("protocol");
						hvName.Append("timeout");
						hvValue.Append("HALCON4");
						hvValue.Append(5);
						OpenSocketAccept(theGlobal.m_Upload.m_tParam.nPortListen[i], hvName, hvValue, &theGlobal.m_Upload.m_ServerSocket[i]);

						//CString strLog;
						//strLog.Format("�ѿ�����λ%d�ļ���:%s:%d",i+2,theGlobal.m_Upload.m_tParam.strAddressListen[i],theGlobal.m_Upload.m_tParam.nPortListen[i]);
						////theGlobal.m_Log.Communication(strLog);
						theGlobal.m_Upload.m_bListenSuccess[i] = TRUE;
					}
					catch (HException e)
					{

					}
				}
				if (theGlobal.m_Upload.m_bConnectClient[i] == FALSE)
				{
					HTuple hvClient;
					try
					{
						SocketAcceptConnect(theGlobal.m_Upload.m_ServerSocket[i], "false", &hvClient);

						HTuple hvValue1;
						GetSocketParam(hvClient, "address_info", &hvValue1);
						//SetSocketParam(hvClient, "timeout", 0.05);

						theGlobal.m_Upload.m_ClientSocket[i] = hvClient;
						theGlobal.m_Upload.m_bConnectClient[i] = TRUE;

						CString strlog;
						strlog.Format("�빤λ%d���罨���ɹ�.(IP:%s:%d)---(IP:%s:%d)", i + 2, hvValue1[0].S().Text(), hvValue1[1].I(), hvValue1[2].S().Text(), hvValue1[3].I());
						theGlobal.m_Log.Communication(strlog);
					}
					catch (HException&e)
					{
						CString strMessage;
						strMessage.Format("%s(%s)", e.ErrorMessage().Text(), e.ProcName().Text());
						//theGlobal.m_Log.Std(strMessage);
						theGlobal.m_Upload.m_bConnectClient[i] = FALSE;
						//������η���ʧ�ܣ��Ͽ��������������������ӿͻ���
						/*if (++nDisconnectCount[i] > 60)
						{
							theGlobal.m_Upload.m_bListenSuccess[i] = FALSE;
							nDisconnectCount[i] = 0;
						}*/
					}
				}
				else
				{
					static HTuple s_hvCountShake = HTuple().TupleGenConst(MAX_CONNECT, 0);
					//���ӳɹ�����������Ϣ����ֹ��ͻ��˵���
					
					static HTuple s_hvTimeStamp[MAX_CONNECT];
					try
					{
						s_hvCountShake[i] += 1;
						if (s_hvCountShake[i].I() >= 5)//1�뷢һ��������Ϣ
						{
							s_hvCountShake[i] = 0;
							HTuple hvSendData;
							hvSendData.Append("ShakeHand");
							SendTuple(theGlobal.m_Upload.m_ClientSocket[i], hvSendData);
							CString strlog;
							strlog.Format("�빤λ%d����������Ϣ�ɹ�", i + 2);
							//theGlobal.m_Log.Communication(strlog);

							CountSeconds(&s_hvTimeStamp[i]);
						}
					}
					catch (HException&)
					{
						HTuple hvTimeTmp;
						CountSeconds(&hvTimeTmp);
						if ((hvTimeTmp.D() - s_hvTimeStamp[i].D() >= 60*3))
						{
							CString strMessage;
							strMessage.Format("��λ%d��������ʧ�ܣ��ѶϿ�����", i + 2);
							theGlobal.m_Log.Communication(strMessage);
							theGlobal.m_Log.Alarm(strMessage);
							CountSeconds(&s_hvTimeStamp[i]);
							theGlobal.m_Upload.m_bConnectClient[i] = FALSE;
						}
					}

					//������û���յ�������
					static HTuple s_hvCount = HTuple().TupleGenConst(MAX_CONNECT, 0);
					s_hvCount[i] += 1;
					if (theGlobal.m_Upload.m_bSendBatchOK[i] == FALSE)
					{
						try
						{
							if (s_hvCount[i].I() % 5 == 0)
							{
								HTuple hvSendData;
								hvSendData.Append("ChangeBatch");
								hvSendData.Append(HTuple(theGlobal.GetCurrentBatch()));
								SendTuple(theGlobal.m_Upload.m_ClientSocket[i], hvSendData);

								CString strLog;
								strLog.Format("��λ%d�������:%s�ɹ�", i + 2, theGlobal.GetCurrentBatch());
								//theGlobal.m_Log.Communication(strLog);
							}
						}
						catch (HException&)
						{
							CString strLog;
							strLog.Format("�빤λ%d���ӶϿ��������;��:%sʧ��", i + 2, theGlobal.GetCurrentBatch());
							theGlobal.m_Log.Communication(strLog);
							theGlobal.m_Upload.m_bConnectClient[i] = FALSE;
						}

						//����5����û�л���ɹ����Ͳ��ٲ������
						if (s_hvCount[i] > 300)
						{
							s_hvCount[i] = 0;
							theGlobal.m_Upload.m_bSendBatchOK[i] = TRUE;
							CErrorInfo::tagErrorInfo tInfo;
							tInfo.strDescribe.Format("��λ%d����1����û������Զ�����(%s),����ʧ��", i + 2, theGlobal.GetCurrentBatch());
							tInfo.nID = 0;
							tInfo.strSolution = "1��鹤λ�����������Ƿ�����\n2��λ�Ƿ�ѡ�Զ�����\n";
							theGlobal.m_Log.Alarm(tInfo);
						}
					}

					//���û���䷽У�鷴��������
					static HTuple s_hvCountProduct = HTuple().TupleGenConst(MAX_CONNECT, 0);
					s_hvCountProduct[i] += 1;
					if (theGlobal.m_Upload.m_bSendCheckProductInfo[i] == FALSE)
					{
						try
						{
							if (s_hvCountProduct[i].I() % 5 == 0 && theGlobal.m_Upload.m_hvProductInfo.Length() > 0)
							{
								SendTuple(theGlobal.m_Upload.m_ClientSocket[i], theGlobal.m_Upload.m_hvProductInfo);

								CString strLog;
								strLog.Format("��λ%d�����䷽��Ϣ:%s�ɹ�", i + 2, theGlobal.m_Upload.m_hvProductInfo[1].S().Text());
								//theGlobal.m_Log.Communication(strLog);
							}
						}
						catch (HException&)
						{
							CString strLog;
							strLog.Format("�빤λ%d���ӶϿ��������䷽��Ϣ:%sʧ��", i + 2, theGlobal.m_Upload.m_hvProductInfo[1].S().Text());
							theGlobal.m_Log.Communication(strLog);
							theGlobal.m_Upload.m_bConnectClient[i] = FALSE;
						}

						//����5����û�л���ɹ����Ͳ��ٲ������
						if (s_hvCountProduct[i] > 300)
						{
							s_hvCountProduct[i] = 0;
							theGlobal.m_Upload.m_bSendCheckProductInfo[i] = TRUE;
							CErrorInfo::tagErrorInfo tInfo;
							tInfo.strDescribe.Format("����1����û���յ���λ%d���䷽У����,У��ʧ��", i + 2);
							tInfo.nID = 0;
							tInfo.strSolution = "����λ���������Ƿ�����\n";
							theGlobal.m_Log.Alarm(tInfo);
						}
					}
				}
			}
			catch (HException e)
			{
				theGlobal.m_Upload.m_bConnectClient[i] = FALSE;
			}
		}
		WaitSeconds(0.2);
	}

	//�˳�ǰ������
	for (int i = 0; i < MAX_CONNECT; i++)
	{
		HALCON_TRY(CloseSocket(theGlobal.m_Upload.m_ClientSocket[i]));
		HALCON_TRY(CloseSocket(theGlobal.m_Upload.m_ServerSocket[i]));
	}

}
void T_ClientSocket()//�ǻ�ͷ��ʱ�����
{
	if (!theGlobal.m_Upload.m_tParam.bUseDataUpload) return;

	HTuple hvError, hvOpenStatus, ParamValue;
	while (FALSE == theGlobal.m_bExitThread)
	{
		try
		{
			if (FALSE == theGlobal.m_Upload.m_bConnect)
			{
				try
				{
					HALCON_TRY(CloseSocket(theGlobal.m_Upload.m_Socket));
					OpenSocketConnect(HTuple(theGlobal.m_Upload.m_tParam.strAddresConnect),
						theGlobal.m_Upload.m_tParam.nPortConnect
						, HTuple("protocol").Append("timeout"), HTuple("HALCON4").Append(1), &theGlobal.m_Upload.m_Socket);
					SetSocketParam(theGlobal.m_Upload.m_Socket, "timeout", 0.05);
					CString strLog;
					strLog.Format("�͹�λ1���罨���ɹ�:%s:%d���ӳɹ�", theGlobal.m_Upload.m_tParam.strAddresConnect, theGlobal.m_Upload.m_tParam.nPortConnect);
					theGlobal.m_Log.Communication(strLog);
					theGlobal.m_Upload.m_bConnect = TRUE;
				}
				catch (HException e)
				{
					CString strLog;
					strLog.Format("%s(%s)", e.ErrorMessage().Text(), e.ProcName().Text());
					//theGlobal.m_Log.Std(strLog);
					theGlobal.m_Upload.m_bConnect = FALSE;
				}
			}
		}
		catch (HException e)
		{
			theGlobal.m_Upload.m_bConnect = FALSE;
		}
		WaitSeconds(0.2);
	}
	HALCON_TRY(CloseSocket(theGlobal.m_Upload.m_Socket));
}
void CUpload::Upload(C_MergeWork C)
{
	return;
	try
	{
		HTuple hvMessageSend;
		CreateMessage(&hvMessageSend);
		C.WriteMessage(hvMessageSend);
		EnqueueMessage(m_hvQueue, hvMessageSend, HTuple(), HTuple());
	}
	catch (HException& e)
	{
	}
}
BOOL CUpload::ReadParam()
{
	for (int i = 0; i < MAX_CONNECT; i++)
	{
		CString strKeyName;
		strKeyName.Format("strAddressListen[%d]", i);
		m_tParam.strAddressListen[i] = GetIniString("Upload", strKeyName, "127.0.0.1");
		strKeyName.Format("nPortListen[%d]", i);
		m_tParam.nPortListen[i] = GetIniInt("Upload", strKeyName, 5001 + i);
		strKeyName.Format("bListenClient[%d]", i);
		m_tParam.bListenClient[i] = GetIniInt("Upload", strKeyName, TRUE);
	}
	m_tParam.strAddresConnect = GetIniString("Upload", "strAddresConnect", "127.0.0.1");
	m_tParam.nPortConnect = GetIniInt("Upload", "nPortConnect", 5000);

	m_tParam.bUseDataUpload = GetIniInt("Upload", "bUseDataUpload", FALSE);
	m_tParam.bUseCompensationAuto = GetIniInt("Upload", "bUseCompensationAuto", FALSE);
	m_tParam.dToNextStationMeter = GetIniDouble("Upload", "dToNextStationMeter", 0);
	m_tParam.dStartIgnoreMeter = GetIniDouble("Upload", "dStartIgnoreMeter", 100);
	m_tParam.dCompensationInvertal = GetIniDouble("Upload", "dCompensationInvertal", 5);
	m_tParam.dScale = GetIniDouble("Upload", "dScale", 1);
	m_tParam.bCompensationDataInverse = GetIniInt("Upload", "bCompensationDataInverse", FALSE);
	return TRUE;
}
void CUpload::JWSendBiasTo2(HTuple hvValue, int nImageSN)//��β���Ͳ���ֵ��Ϳ2
{
	if (!m_tParam.bUseDataUpload/* || !m_tParam.bUseCompensationAuto*/) return;

	//��β���Ͳ���ֵ��Ϳ2 ��Ҫ����������תһ�£�����ǰ���һ��ת���ֶ�
	tagPDInfo tPdInfo = theGlobal.m_ProductTool.GetCurrentProductInfo();
	//0��λ��
	//1������Ϣ����
	//2��Ʒ����
	//3�Ƿ����AT11
	//4�Ƿ���ڰ���
	//5ͼ���
	//6ʱ��
	TupleInsert(hvValue, 0, "to2", &hvValue);
	TupleInsert(hvValue, 1, "Bias", &hvValue);
	TupleInsert(hvValue, 2, tPdInfo.nPDType, &hvValue);//��Ʒ����
	TupleInsert(hvValue, 3, tPdInfo.bExistAT11, &hvValue);//�Ƿ����AT11
	TupleInsert(hvValue, 4, tPdInfo.bExistBaseCoat, &hvValue);//�Ƿ���ڰ���
	TupleInsert(hvValue, 5, nImageSN, &hvValue);//ͼ���
	CString strTime, strLog;
	SYSTEMTIME tTime;
	::GetLocalTime(&tTime);
	strTime.Format("%02d:%02d:%02d.%03d", tTime.wHour, tTime.wMinute, tTime.wSecond, tTime.wMilliseconds);
	TupleInsert(hvValue, 6, HTuple(strTime), &hvValue);//ʱ��
	try
	{
		SendTuple(theGlobal.m_Upload.m_Socket, hvValue);
		strLog.Format("���Ͳ���ֵ����λ2�ɹ�:,ͼ���%d,", nImageSN);
		for (int i = 7; i < hvValue.Length() - 1; i++)
		{
			CString strTmp;
			strTmp.Format("%0.4f,", hvValue[i].D());
			strLog += strTmp;
		}
		theGlobal.m_Log.Communication(strLog);
	}
	catch (HException e)
	{
		strLog.Format("��λ3���Ͳ���ֵʧ��");
		theGlobal.m_Log.Communication(strLog);
		m_bConnect = FALSE;
	}
}
void CUpload::To2SendBiasJT(HTuple hvValue, int nImageSN)//Ϳ2���Ͳ���ֵ����ͷ
{
	if (!m_tParam.bUseDataUpload/* || !m_tParam.bUseCompensationAuto*/) return;

	tagPDInfo tPdInfo = theGlobal.m_ProductTool.GetCurrentProductInfo();
	//0��λ��
	//1������Ϣ����
	//2��Ʒ����
	//3�Ƿ����AT11
	//4�Ƿ���ڰ���
	//5ͼ���
	//6ʱ��
	TupleInsert(hvValue, 0, "Jitou", &hvValue);
	TupleInsert(hvValue, 1, "Bias", &hvValue);
	TupleInsert(hvValue, 2, tPdInfo.nPDType, &hvValue);//��Ʒ����
	TupleInsert(hvValue, 3, tPdInfo.bExistAT11, &hvValue);//�Ƿ����AT11
	TupleInsert(hvValue, 4, tPdInfo.bExistBaseCoat, &hvValue);//�Ƿ���ڰ���
	TupleInsert(hvValue, 5, nImageSN, &hvValue);//ͼ���
	CString strTime;
	SYSTEMTIME tTime;
	::GetLocalTime(&tTime);
	strTime.Format("%02d:%02d:%02d.%03d", tTime.wHour, tTime.wMinute, tTime.wSecond, tTime.wMilliseconds);
	TupleInsert(hvValue, 6, HTuple(strTime), &hvValue);//ʱ��
	try
	{
		SendTuple(theGlobal.m_Upload.m_Socket, hvValue);
		CString strLog;
		strLog.Format("���Ͳ���ֵ����λ1�ɹ�:,ͼ���%d,", nImageSN);
		for (int i = 7; i < hvValue.Length() - 1; i++)
		{
			CString strTmp;
			strTmp.Format("%0.4f,", hvValue[i].D());
			strLog += strTmp;
		}
		theGlobal.m_Log.Communication(strLog);
	}
	catch (HException e)
	{
		CString strLog;
		strLog.Format("��λ2���Ͳ���ֵʧ��");
		theGlobal.m_Log.Communication(strLog);
		m_bConnect = FALSE;
	}
}
void CUpload::RecvBias(HTuple hvValue)
{
	CString strLog;
	strLog.Format("�յ�ǰ��λ����ֵ(Bias):,ͼ���%d,", hvValue[0].I());
	for (int i = 2; i < hvValue.Length(); i++)
	{
		strLog.Format("%s,%.4f", strLog, hvValue[i].D());
	}
	theGlobal.m_Log.Communication(strLog);

	//ʵ���ǲ���ֵ����ҪһͿ��Ϳ��������,0Ԫ����ͼ���,1Ԫ����ʱ��
	TupleSelectRange(hvValue, 2, hvValue.Length() - 1, &m_hvBiasRecv);
}
BOOL CUpload::IsNetConnect()
{
	BOOL bConnect = TRUE;
	if (theGlobal.m_tSystemSet.eSN == CGlobal::SN_Head)
	{
		if (!m_tParam.bUseDataUpload || (!m_bListenSuccess[0]&&m_tParam.bListenClient[0]) || (!m_bListenSuccess[1]&&m_tParam.bListenClient[1]))bConnect = FALSE;
	}
	else
	{
		if (!m_tParam.bUseDataUpload || !m_bConnect) bConnect = FALSE;
	}

	return bConnect;
}

void CUpload::SendBatchToClient(CString strBatch)
{
	if (!m_tParam.bUseDataUpload) return;

	for (int i = 0; i < MAX_CONNECT; i++)
	{
		try
		{
			CString strLog;
			if (theGlobal.m_Upload.m_bListenSuccess[i] == FALSE)
			{
				strLog.Format("�빤λ%d���ӶϿ������;��:%sʧ��", i + 2, strBatch);
				theGlobal.m_Log.Communication(strLog);
				theGlobal.m_Upload.m_bSendBatchOK[i] = FALSE;
				continue;
			}

			HTuple hvSendData;
			hvSendData.Append("ChangeBatch");
			hvSendData.Append(HTuple(strBatch));
			SendTuple(theGlobal.m_Upload.m_ClientSocket[i], hvSendData);

			strLog.Format("��λ%d���;��:%s�ɹ�", i + 2, strBatch);
			theGlobal.m_Log.Communication(strLog);
		}
		catch (HException&)
		{
			CString strLog;
			strLog.Format("�빤λ%d���ӶϿ������;��:%sʧ��", i + 2, strBatch);
			theGlobal.m_Log.Communication(strLog);
			//theGlobal.m_Upload.m_bListenSuccess[i] = FALSE;
			theGlobal.m_Upload.m_bConnectClient[i] = FALSE;
		}
		theGlobal.m_Upload.m_bSendBatchOK[i] = FALSE;
	}
}
void CUpload::RecvBatchFromServer(CString strBatch)
{
	m_strBatchRecv = strBatch;
	CString strLog;
	strLog.Format("���չ�λ1���͵ľ��:%s", strBatch);
	theGlobal.m_Log.Communication(strLog);
}
BOOL CUpload::WriteParam()
{
	for (int i = 0; i < MAX_CONNECT; i++)
	{
		CString strKeyName;
		strKeyName.Format("strAddressListen[%d]", i);
		WriteIniString("Upload", strKeyName, m_tParam.strAddressListen[i]);
		strKeyName.Format("nPortListen[%d]", i);
		WriteIniInt("Upload", strKeyName, m_tParam.nPortListen[i]);
		strKeyName.Format("bListenClient[%d]", i);
		WriteIniInt("Upload", strKeyName, m_tParam.bListenClient[i]);
	}
	WriteIniString("Upload", "strAddresConnect", m_tParam.strAddresConnect);
	WriteIniInt("Upload", "nPortConnect", m_tParam.nPortConnect);

	WriteIniInt("Upload", "bUseDataUpload", m_tParam.bUseDataUpload);
	WriteIniInt("Upload", "bUseCompensationAuto", m_tParam.bUseCompensationAuto);
	WriteIniDouble("Upload", "dToNextStationMeter", m_tParam.dToNextStationMeter);
	WriteIniDouble("Upload", "dStartIgnoreMeter", m_tParam.dStartIgnoreMeter);
	WriteIniDouble("Upload", "dCompensationInvertal", m_tParam.dCompensationInvertal);
	WriteIniDouble("Upload", "dScale", m_tParam.dScale);
	WriteIniInt("Upload", "bCompensationDataInverse", m_tParam.bCompensationDataInverse);
	return TRUE;
}
BOOL CUpload::Init()
{
	ReadParam();

	CreateMessageQueue(&m_hvQueue);//����һ��IO����
	SetMessageQueueParam(m_hvQueue, "max_message_num", 1);
	HDevThreadContext Context;
	HDevThread* pThread = new HDevThread(Context,
		(void*)HThreadUpload, 1, 0);//����һ������1��������߳�;
	pThread->SetInputCtrlParamTuple(0, m_hvQueue);
	pThread->Start();

	if (CGlobal::SN_Head == theGlobal.m_tSystemSet.eSN)
	{
		AfxBeginThread((AFX_THREADPROC)T_ServerSocket, NULL);//��ͷ��Ϊ������
		AfxBeginThread((AFX_THREADPROC)T_ServerRecvData, NULL);//��ͷ��Ϊ������
	}
	else
	{
		AfxBeginThread((AFX_THREADPROC)T_ClientSocket, NULL);//��������Ϊ�ͻ���
		AfxBeginThread((AFX_THREADPROC)T_ClientRecvData, NULL);//��������Ϊ�ͻ���
	}
	return TRUE;
}
void* CUpload::HThreadUpload(void *hcthread)//io�߳�
{
	HDevThread*  hcppthread = (HDevThread*)hcthread;
	try
	{
		const HTuple        &cbhv_Queue = hcppthread->GetInputCtrlParamTuple(0);
		//T_Upload(cbhv_Queue);
	}
	catch (HException& e)
	{
	}
	return NULL;
}
void T_ServerRecvData()
{
	if (!theGlobal.m_Upload.m_tParam.bUseDataUpload) return;

	HTuple hvTimeStampe[MAX_CONNECT];
	HTuple hvTimeStampeListen[MAX_CONNECT];
	CountSeconds(&hvTimeStampe[0]);
	CountSeconds(&hvTimeStampe[1]);
	CountSeconds(&hvTimeStampeListen[0]);
	CountSeconds(&hvTimeStampeListen[1]);

	BOOL bConnectAlarm[MAX_CONNECT] = { FALSE,FALSE };
	BOOL bConnectLongAlarm[MAX_CONNECT] = { FALSE,FALSE };

	while (FALSE == theGlobal.m_bExitThread)
	{
		HTuple hvDataType;
		for (int i = 0; i < MAX_CONNECT; i++)
		{
			if (!theGlobal.m_Upload.m_tParam.bListenClient[i]) continue;
			try
			{
				//�ж�60sû�н��յ��ͻ��˵�������Ϣ���Ͽ�����
				if (theGlobal.m_Upload.m_bConnectClient[i] == TRUE)
				{
					bConnectLongAlarm[i] = FALSE;
					CountSeconds(&hvTimeStampeListen[i]);
					HTuple hvTimeTmp;
					CountSeconds(&hvTimeTmp);
					if ((hvTimeTmp.D() - hvTimeStampe[i].D() > 60) && bConnectAlarm[i])
					{
						bConnectAlarm[i] = FALSE;
						theGlobal.m_Upload.m_bConnectClient[i] = FALSE;
						CountSeconds(&hvTimeStampe[i]);
						CString strMessage;
						strMessage.Format("1����δ�յ���λ%d���������ѶϿ�����", i + 2);
						theGlobal.m_Log.Communication(strMessage);
						theGlobal.m_Log.Alarm(strMessage);
					}
				}
				else if (theGlobal.m_Upload.m_bConnectClient[i] == FALSE && !bConnectLongAlarm[i])
				{
					bConnectAlarm[i] = TRUE;
					HTuple hvTimeTmp;
					CountSeconds(&hvTimeTmp);
					if ((hvTimeTmp.D() - hvTimeStampeListen[i].D() > 60*3))
					{
						bConnectLongAlarm[i] = TRUE;
						//theGlobal.m_Upload.m_bListenSuccess[i] = FALSE;
						CountSeconds(&hvTimeStampeListen[i]);
						CString strMessage;
						strMessage.Format("3����δ�����Ϲ�λ%d������", i + 2);
						theGlobal.m_Log.Communication(strMessage);
						theGlobal.m_Log.Alarm(strMessage);
					}
				}
				//�ͻ��˽��յ���Ϣ
				GetNextSocketDataType(theGlobal.m_Upload.m_ClientSocket[i], &hvDataType);
			}
			catch (HException e)
			{
				continue;
			}
			catch (...)
			{
			}

			CString strRecvData;
			strRecvData.Format("%s", hvDataType.S().Text());
			if ("tuple" == strRecvData)//�����յ�����Offsetֵ  ��0Ϊ�������� ��1λ����ƫ��ֵ
			{
				try
				{
					HTuple hvData;
					ReceiveTuple(theGlobal.m_Upload.m_ClientSocket[i], &hvData);
					if ("to2" == hvData[0])//����Ҫת����Ϳ2������
					{
						try
						{
							SendTuple(theGlobal.m_Upload.m_ClientSocket[0], hvData);
							CString strlog;
							strlog.Format("ת���������ݸ���λ2�ɹ�");
							theGlobal.m_Log.Communication(strlog);
						}
						catch (HException&e)
						{
							CString strlog;
							strlog.Format("ת���������ݸ���λ2ʧ��");
							theGlobal.m_Log.Exception(strlog);
						}

					}
					else if (hvData.Length() > 1 && "Bias" == hvData[1])
					{
						tagPDInfo tPdInfo = theGlobal.m_ProductTool.GetCurrentProductInfo();
						//0��λ��
						//1������Ϣ����
						//2��Ʒ����
						//3�Ƿ����AT11
						//4�Ƿ���ڰ���
						//5ͼ���
						//6ʱ��
						if (tPdInfo.nPDType == hvData[2].I() && tPdInfo.bExistAT11 == hvData[3].I() && tPdInfo.bExistBaseCoat == hvData[4].I())
						{
							HTuple hvRev;
							TupleSelectRange(hvData, 5, hvData.Length() - 1, &hvRev);
							theGlobal.m_Upload.RecvBias(hvRev);

							CString strMessage;
							strMessage.Format("���ղ������ݳɹ�");
							//theGlobal.m_Log.Communication(strMessage);
							theGlobal.m_Upload.WriteBiasFile(hvRev, hvData[5].I());
						}
						else
						{
							CString strMessage;
							strMessage.Format("���յĲ��������뵱ǰ��λ��Ʒ���Ͳ���: ��λ�Ĳ�Ʒ����:һ��%d, �Ƿ����AT11:%s, �Ƿ���ڰ���:%s",
								hvData[2].I() * 2, hvData[3].I() == TRUE ? "��" : "��", hvData[4].I() == TRUE ? "��" : "��");
							theGlobal.m_Log.Exception(strMessage);
						}
					}
					else if ("ShakeHandOK" == hvData[0])
					{
						CountSeconds(&hvTimeStampe[i]);

						CString strlog;
						strlog.Format("���չ�λ%d����������Ϣ�����ɹ�", i + 2);
						//theGlobal.m_Log.Communication(strlog);
					}
					else if ("ChangeBatchOK" == hvData[0] && theGlobal.GetCurrentBatch() == hvData[1].S().Text())
					{
						theGlobal.m_Upload.m_bSendBatchOK[i] = TRUE;

						CString strlog;
						strlog.Format("���չ�λ%d������ɷ����ɹ�", i + 2);
						theGlobal.m_Log.Communication(strlog);
					}
					else if (hvData.Length() > 1 && "CheckProductInfo" == hvData[0])
					{
						CString strInfo;
						if (hvData[1] == "OK")
						{
							//У��ɹ�
							strInfo.Format("��λ%d���䷽У�����", i + 2);
						}
						else
						{
							//У��ʧ��
							strInfo.Format("��λ%d���䷽У�����,�䷽��Ϣ��һ��,��λ%d�䷽��Ϣ�Ѹ���", i + 2, i + 2);
							theGlobal.SetAlarmInfo(strInfo);
						}
						theGlobal.m_Log.Communication(strInfo);
						theGlobal.m_Upload.m_bSendCheckProductInfo[i] = TRUE;
					}
					else if ("AlarmInfo" == hvData[0])
					{
						//����������λ�ı�����Ϣ
						theGlobal.m_Upload.RecvAlarmInfoFromClient(hvData, i);
					}

					static HTuple s_hvCount = HTuple().TupleGenConst(MAX_CONNECT, 0);
					if (theGlobal.m_Upload.m_bSendCheckProductInfo[i] == FALSE)
					{
						s_hvCount[i] += 1;
						if (s_hvCount[i] > 60000)//2����û���յ��䷽У�鷴�������ٽ���
						{
							s_hvCount = 0;
							theGlobal.m_Upload.m_bSendCheckProductInfo[i] = TRUE;
							CString strInfo;
							strInfo.Format("��λ%d���䷽У��ʧ�ܣ�δ�յ���λ%d���䷽У�鷴��", i + 2, i + 2);
						}
					}
				}
				catch (HException)
				{
					//theGlobal.m_Upload.m_bConnect = FALSE;
				}
				catch (...)
				{

				}
			}
		}
		Sleep(20);
	}
}
void T_ClientRecvData()
{
	if (!theGlobal.m_Upload.m_tParam.bUseDataUpload) return;

	HTuple hvTimeStampe(0);
	CountSeconds(&hvTimeStampe);
	BOOL bConnectAlarm = TRUE;
	while (FALSE == theGlobal.m_bExitThread)
	{
		HTuple hvDataType;
		try
		{
			//�ж�30sû�н��յ���������������Ϣ���Ͽ�����
			HTuple hvTimeTmp;
			CountSeconds(&hvTimeTmp);
			if (theGlobal.m_Upload.m_bConnect == TRUE)
			{
				if ((hvTimeTmp.D() - hvTimeStampe.D() > 60) && bConnectAlarm)
				{
					CountSeconds(&hvTimeStampe);
					theGlobal.m_Upload.m_bConnect = FALSE;
					CString strMessage;
					strMessage.Format("1����δ���յ���λ1�������ѶϿ�����");
					theGlobal.m_Log.Communication(strMessage);
					theGlobal.m_Log.Alarm(strMessage);

					bConnectAlarm = FALSE;
				}
			}
			else if (theGlobal.m_Upload.m_bConnect == FALSE)
			{
				bConnectAlarm = TRUE;
				CountSeconds(&hvTimeStampe);
			}
			//�ͻ��˽��յ���Ϣ
			GetNextSocketDataType(theGlobal.m_Upload.m_Socket, &hvDataType);
		}
		catch (HException e)
		{
			continue;
		}
		catch (...)
		{
			continue;
		}
		CString strRecvData;
		strRecvData.Format("%s", hvDataType.S().Text());
		if ("tuple" == hvDataType)//�����յ�����Offsetֵ  ��0Ϊ�������� ��1λ����ƫ��ֵ
		{
			try
			{
				HTuple hvData;
				ReceiveTuple(theGlobal.m_Upload.m_Socket, &hvData);
				if ("ChangeBatch" == hvData[0])
				{
					CString strBatch;
					strBatch.Format("%s", hvData[1].S().Text());
					theGlobal.m_Upload.RecvBatchFromServer(strBatch);

					//������Ϣ
					HTuple hvMsg = "ChangeBatchOK";
					hvMsg.Append((HTuple)strBatch);
					SendTuple(theGlobal.m_Upload.m_Socket, hvMsg);
				}
				else if (hvData.Length() > 1 && "Bias" == hvData[1])
				{
					tagPDInfo tPdInfo = theGlobal.m_ProductTool.GetCurrentProductInfo();
					//0��λ��
					//1������Ϣ����
					//2��Ʒ����
					//3�Ƿ����AT11
					//4�Ƿ���ڰ���
					//5ͼ���
					//6ʱ��
					if (tPdInfo.nPDType == hvData[2].I() && tPdInfo.bExistAT11 == hvData[3].I() && tPdInfo.bExistBaseCoat == hvData[4].I())
					{
						HTuple hvRev;
						TupleSelectRange(hvData, 5, hvData.Length() - 1, &hvRev);
						theGlobal.m_Upload.RecvBias(hvRev);

						CString strMessage;
						strMessage.Format("���ղ������ݳɹ�");
						//theGlobal.m_Log.Communication(strMessage);
						theGlobal.m_Upload.WriteBiasFile(hvRev, hvData[5].I());
					}
					else
					{
						CString strMessage;
						strMessage.Format("���յĲ��������뵱ǰ��λ��Ʒ���Ͳ���: ��λ�Ĳ�Ʒ����:һ��%d, �Ƿ����AT11:%s, �Ƿ���ڰ���:%s",
							hvData[2].I() * 2, hvData[3].I() == TRUE ? "��" : "��", hvData[4].I() == TRUE ? "��" : "��");
						theGlobal.m_Log.Exception(strMessage);
					}
				}
				else if ("ShakeHand" == hvData[0])
				{
					HTuple hvMsg = "ShakeHandOK";
					SendTuple(theGlobal.m_Upload.m_Socket, hvMsg);
					CountSeconds(&hvTimeStampe);
				}
				else if ("CheckProductInfo" == hvData[0])
				{
					theGlobal.m_Upload.m_hvProductInfo = hvData;

					CString strInfo;
					strInfo.Format("");
					//������Ϣ
					HTuple hvMsg = "CheckProductInfo";
					if (!theGlobal.m_Upload.CheckProductInfoFromServer())
					{
						//У��ʧ��
						strInfo = "�յ���ͷ�䷽��Ϣ���䷽��Ϣ��һ�£��Ѿ�����";
						hvMsg.Append((HTuple)"NG");
					}
					else
					{
						//У��ɹ�
						strInfo = "�յ���ͷ�䷽��Ϣ���䷽��Ϣһ�£��䷽У�����";
						hvMsg.Append((HTuple)"OK");
					}
					//������Ϣ
					SendTuple(theGlobal.m_Upload.m_Socket, hvMsg);
					theGlobal.m_Log.Communication(strInfo);
				}
			}
			catch (HException)
			{

			}
			catch (...)
			{

			}
		}
		Sleep(20);
	}
}

void CUpload::Action(const C_MergeWork& C)
{
	try
	{
		HTuple hvHeight, hvHeightM;
		GetImageSize(C.vecB[0].vecA[0].hoGrabImage, &HTuple(), &hvHeight);
		hvHeightM = hvHeight.D()*theGlobal.m_vecResolution[0].dRow * 0.001;

		//��Ϳ�ͻ�ͷ��Ҫ�ռ�����
		if (theGlobal.m_tSystemSet.eSN == CGlobal::SN_Head || theGlobal.m_tSystemSet.eSN == CGlobal::SN_2nd)
		{
			m_vecRawData[m_vecRawData.Length()].T() = C.vecB[0].tMeasure.tSize.hv_WidthOrigin;//ԭʼ����

			double dMeterCount = m_vecRawData.Length()*hvHeightM.D();
			if (dMeterCount > m_tParam.dToNextStationMeter)
				m_vecRawData = m_vecRawData.Remove(0);
		}

		if (!m_tParam.bUseDataUpload/* || !m_tParam.bUseCompensationAuto*/) return;
		const double dCurrentMeterCount = theGlobal.m_Counter.GetCurrentMeterCount();
		static double s_dMeterCount = dCurrentMeterCount;
		//��ֹ�������㣬�޷�����ֵ
		if (s_dMeterCount > dCurrentMeterCount)
			s_dMeterCount = dCurrentMeterCount;

		if (theGlobal.m_tSystemSet.eSN == CGlobal::SN_2nd)
		{
			//��Ϳ�Ƚ��ջ�β���ݣ����㲹��ֵ
			if (m_tParam.bUseCompensationAuto && m_hvBiasRecv.Length() == C.vecB[0].tMeasure.tSize.hv_WidthOrigin.Length())
			{
				//ȡ�ӳٵ���β�����������ͷ���ף�����һ����ֵ���ͽ��յ�����������õ����߲���ֵ
				try
				{
					int nNum = m_tParam.dToNextStationMeter / hvHeightM.D();
					int nNum5m = m_tParam.dCompensationInvertal / hvHeightM.D();
					if (m_vecRawData.Length() > (nNum + nNum5m - 1))
					{
						HTuple hvData;
						for (int i = 0; i < m_vecRawData[0].T().Length(); i++)
						{
							HTuple hvTmp, hvTmp2;
							for (int j = 0; j < nNum5m; j++)
							{
								hvTmp2.Append(m_vecRawData[j].T()[i]);
								HTuple hv_FindResult0 = m_vecRawData[j].T().TupleFind(0);
								HTuple hv_FindResultNoData = m_vecRawData[j].T().TupleFind(NO_DATA);
								if (hv_FindResult0 != -1 || hv_FindResultNoData != -1) continue;

								hvTmp.Append(m_vecRawData[j].T()[i]);
							}
							TupleMedian(hvTmp, &hvTmp);
							hvData.Append(hvTmp);
							if (hvTmp2.TupleFind(0).Length() > hvTmp2.Length() / 4
								|| hvTmp2.TupleFind(NO_DATA).Length() > hvTmp2.Length() / 4)
							{
								//��Чֵ�ж�
								CString strLog;
								strLog.Format("CUpload_Action: ��Чֵ���࣬�����㲹��");
								theGlobal.m_Log.Std(strLog);
								return;
							}
							else if (hvTmp2.TupleMedian() > 0.8)
							{
								//�����׼��
								CString strLog;
								strLog.Format("CUpload_Action: �ռ���׼��%0.2f������0.8�������㲹��", hvTmp2.TupleMedian().D());
								theGlobal.m_Log.Std(strLog);
								return;
							}
						}
						if (hvData.Length() == m_hvBiasRecv.Length())
						{
							HTuple hv_FindResult0 = m_hvBiasRecv.TupleFind(0);
							HTuple hv_FindResultNoData = m_hvBiasRecv.TupleFind(NO_DATA);
							if (hv_FindResult0 != -1 || hv_FindResultNoData != -1) return;

							TupleSub(m_hvBiasRecv, hvData, &m_hvCompensation);
							TupleMult(m_hvCompensation, m_tParam.dScale, &m_hvCompensation);
							//��Ҫ������ֵ���¸����߲�������
							theGlobal.m_ProductTool.UpdateOnlineOffset(m_hvCompensation);
							CString strLog = "Upload_Action: ����Ĳ���ֵΪ:,";
							for (int i = 0; i < m_hvCompensation.Length(); i++)
							{
								CString strVal;
								strVal.Format("%0.4f,", m_hvCompensation[i].D());
								strLog += strVal;
							}
							strLog += "�յ��¹�λ�Ĳ�������Ϊ:,";
							for (int i = 0; i < m_hvBiasRecv.Length(); i++)
							{
								CString strVal;
								strVal.Format("%0.4f,", m_hvBiasRecv[i].D());
								strLog += strVal;
							}
							strLog += "��ǰ��Ӧ����ֵΪ:,";
							for (int i = 0; i < hvData.Length(); i++)
							{
								CString strVal;
								strVal.Format("%0.4f,", hvData[i].D());
								strLog += strVal;
							}
							theGlobal.m_Log.Std(strLog);
						}
					}
				}
				catch (HException&)
				{
					CString strLog;
					strLog.Format("Upload_Action: ���㲹��ֵʧ��");
					theGlobal.m_Log.Exception(strLog);
				}
			}

			//��Ϳ�����ݸ���ͷ��������������Ĳ�������,�������ҵ�����������ͷ

			if (m_tParam.bCompensationDataInverse)
			{
				if (C.vecB.size() != 2) return;
				//���ж���Чֵ
				/*HTuple hv_FindResult0 = C.vecB[1].tMeasure.tSize.hv_Width.TupleFind(0);
				HTuple hv_FindResultNoData = C.vecB[1].tMeasure.tSize.hv_Width.TupleFind(NO_DATA);
				if (hv_FindResult0 != -1 || hv_FindResultNoData != -1) return;*/

				m_vecDataCollect[m_vecDataCollect.Length()].T() = C.vecB[1].tMeasure.tSize.hv_Width.TupleInverse();
			}
			else
			{
				if (C.vecB.size() != 2) return;
				//���ж���Чֵ
				/*HTuple hv_FindResult0 = C.vecB[0].tMeasure.tSize.hv_Width.TupleFind(0);
				HTuple hv_FindResultNoData = C.vecB[0].tMeasure.tSize.hv_Width.TupleFind(NO_DATA);
				if (hv_FindResult0 != -1 || hv_FindResultNoData != -1) return;*/

				m_vecDataCollect[m_vecDataCollect.Length()].T() = C.vecB[0].tMeasure.tSize.hv_Width;
			}

			const double dCurrentMeterCount = theGlobal.m_Counter.GetCurrentMeterCount();

			//5�׷�һ������
			if (dCurrentMeterCount - s_dMeterCount > m_tParam.dCompensationInvertal && m_vecDataCollect.Length() > 0)
			{
				try
				{
					s_dMeterCount = dCurrentMeterCount;

					//ÿ5����������ֵ������
					HTuple hvData;
					for (int i = 0; i < m_vecDataCollect[0].T().Length(); i++)
					{
						HTuple hvTmp;
						for (int j = 0; j < m_vecDataCollect.Length(); j++)
						{
							hvTmp.Append(m_vecDataCollect[j].T()[i]);
						}
						TupleMedian(hvTmp, &hvTmp);
						hvData.Append(hvTmp);
					}
					if (hvData.Length() == m_vecDataCollect[m_vecDataCollect.Length() - 1].T().Length())
						To2SendBiasJT(hvData, C.vecB[0].vecA[0].hvImageSN.I());
					m_vecDataCollect.Clear();
				}
				catch (HException&)
				{
					CString strLog;
					strLog.Format("Upload_Action: ���Ͳ�������ʧ��");
					theGlobal.m_Log.Exception(strLog);
				}
			}
		}
		else if (theGlobal.m_tSystemSet.eSN == CGlobal::SN_Tail)
		{
			//��β�����ݸ���Ϳ����������������ݼ��ɣ��������ҵߵ�����
			if (m_tParam.bCompensationDataInverse)
			{
				if (C.vecB.size() != 2) return;
				/*HTuple hv_FindResult0 = C.vecB[1].tMeasure.tSize.hv_Width.TupleFind(0);
				HTuple hv_FindResultNoData = C.vecB[1].tMeasure.tSize.hv_Width.TupleFind(NO_DATA);
				if (hv_FindResult0 != -1 || hv_FindResultNoData != -1) return;*/

				m_vecDataCollect[m_vecDataCollect.Length()].T() = C.vecB[1].tMeasure.tSize.hv_Width.TupleInverse();
			}
			else
			{
				if (C.vecB.size() != 2) return;
				/*HTuple hv_FindResult0 = C.vecB[0].tMeasure.tSize.hv_Width.TupleFind(0);
				HTuple hv_FindResultNoData = C.vecB[0].tMeasure.tSize.hv_Width.TupleFind(NO_DATA);
				if (hv_FindResult0 != -1 || hv_FindResultNoData != -1) return;*/

				m_vecDataCollect[m_vecDataCollect.Length()].T() = C.vecB[0].tMeasure.tSize.hv_Width;
			}

			const double dCurrentMeterCount = theGlobal.m_Counter.GetCurrentMeterCount();

			//5�׷�һ������
			if (dCurrentMeterCount - s_dMeterCount > m_tParam.dCompensationInvertal
				&& m_vecDataCollect.Length() > 0)
			{
				s_dMeterCount = dCurrentMeterCount;

				try
				{
					//ÿ5����������ֵ������
					HTuple hvData;
					for (int i = 0; i < m_vecDataCollect[0].T().Length(); i++)
					{
						HTuple hvTmp;
						for (int j = 0; j < m_vecDataCollect.Length(); j++)
						{
							hvTmp.Append(m_vecDataCollect[j].T()[i]);
						}
						TupleMedian(hvTmp, &hvTmp);
						hvData.Append(hvTmp);
					}
					//2021.7.14��ʱ���ԣ������Ͳ�������
					if (hvData.Length() == m_vecDataCollect[m_vecDataCollect.Length() - 1].T().Length())
						JWSendBiasTo2(hvData, C.vecB[0].vecA[0].hvImageSN.I());
					m_vecDataCollect.Clear();
				}
				catch (HException&)
				{
					CString strLog;
					strLog.Format("Upload_Action: ���Ͳ�������ʧ��");
					theGlobal.m_Log.Exception(strLog);
				}
			}
		}
		else
		{
			//��ͷֻ�������ݴ���
			if (m_tParam.bUseCompensationAuto && m_hvBiasRecv.Length() == C.vecB[0].tMeasure.tSize.hv_WidthOrigin.Length())
			{
				//ȡ�ӳٵ���Ϳ�����������ͷ���ף�����һ����ֵ���ͽ��յ�����������õ����߲���ֵ
				int nNum = m_tParam.dToNextStationMeter / hvHeightM.D();
				int nNum5m = m_tParam.dCompensationInvertal / hvHeightM.D();
				if (m_vecRawData.Length() > (nNum + nNum5m - 1))
				{
					try
					{
						HTuple hvData;
						HTuple hvStdevData;
						for (int i = 0; i < m_vecRawData[0].T().Length(); i++)
						{
							HTuple hvTmp,hvTmp2;
							for (int j = 0; j < nNum5m; j++)
							{
								hvTmp2.Append(m_vecRawData[j].T()[i]);
								HTuple hv_FindResult0 = m_vecRawData[j].T().TupleFind(0);
								HTuple hv_FindResultNoData = m_vecRawData[j].T().TupleFind(NO_DATA);
								if (hv_FindResult0 != -1 || hv_FindResultNoData != -1) continue;

								hvTmp.Append(m_vecRawData[j].T()[i]);
							}
							TupleMedian(hvTmp, &hvTmp);
							hvData.Append(hvTmp);
							if (hvTmp2.TupleFind(0).Length() > hvTmp2.Length()/4
								|| hvTmp2.TupleFind(NO_DATA).Length() > hvTmp2.Length() / 4)
							{
								//��Чֵ�ж�
								CString strLog;
								strLog.Format("CUpload_Action: ��Чֵ���࣬�����㲹��");
								theGlobal.m_Log.Std(strLog);
								return;
							}
							else if (hvTmp2.TupleMedian() > 0.8)
							{
								//�����׼��
								CString strLog;
								strLog.Format("CUpload_Action: �ռ���׼��%0.2f������0.8�������㲹��", hvTmp2.TupleMedian().D());
								theGlobal.m_Log.Std(strLog);
								return;
							}
						}
						if (hvData.Length() == m_hvBiasRecv.Length())
						{
							HTuple hv_FindResult0 = m_hvBiasRecv.TupleFind(0);
							HTuple hv_FindResultNoData = m_hvBiasRecv.TupleFind(NO_DATA);
							if (hv_FindResult0 != -1 || hv_FindResultNoData != -1) return;

							TupleSub(m_hvBiasRecv, hvData, &m_hvCompensation);
							TupleMult(m_hvCompensation, m_tParam.dScale, &m_hvCompensation);
							//��Ҫ������ֵ���¸����߲�������
							theGlobal.m_ProductTool.UpdateOnlineOffset(m_hvCompensation);
							CString strLog = "Upload_Action: ����Ĳ���ֵΪ:,";
							for (int i = 0; i < m_hvCompensation.Length(); i++)
							{
								CString strVal;
								strVal.Format("%0.4f,", m_hvCompensation[i].D());
								strLog += strVal;
							}
							strLog += "�յ��¹�λ�Ĳ�������Ϊ:,";
							for (int i = 0; i < m_hvBiasRecv.Length(); i++)
							{
								CString strVal;
								strVal.Format("%0.4f,", m_hvBiasRecv[i].D());
								strLog += strVal;
							}
							strLog += "��ǰ��Ӧ����ֵΪ:,";
							for (int i = 0; i < hvData.Length(); i++)
							{
								CString strVal;
								strVal.Format("%0.4f,", hvData[i].D());
								strLog += strVal;
							}
							theGlobal.m_Log.Std(strLog);
						}
					}
					catch (HException&)
					{
						CString strLog;
						strLog.Format("Upload_Action: ���㲹������ʧ��");
						theGlobal.m_Log.Exception(strLog);
					}
				}
			}
		}
	}
	catch (HException&e)
	{
		CString strException;
		strException.Format("Upload_Action: �쳣%s(%s)", e.ErrorMessage().Text(), e.ProcName().Text());
		theGlobal.m_Log.Exception(strException);
	}
	catch (...)
	{
		CString strException;
		strException.Format("Upload_Action: �����쳣������");
		theGlobal.m_Log.Exception(strException);
	}
}

void CUpload::WriteBiasFile(HTuple hvBiasValue, int nImageSN)
{
	if (!m_tParam.bUseCompensationAuto) return;

	try
	{
		tagPDInfo tPdInfo = theGlobal.m_ProductTool.GetCurrentProductInfo();
		SYSTEMTIME tTm;
		GetLocalTime(&tTm);
		CString strCsvFile, strFliePath;
		strFliePath.Format("%s//��ʪĤ�Զ���������//", theGlobal.m_Storage.GetMapFolderPath());
		if (!PathFileExists(strFliePath)) CreateMultilevelPath(strFliePath);
		strCsvFile.Format("%s//��λ%d���չ�λ%d��������_%04d%02d%02d.csv", strFliePath, theGlobal.m_tSystemSet.eSN, theGlobal.m_tSystemSet.eSN + 1, tTm.wYear, tTm.wMonth, tTm.wDay);
		if (!PathFileExists(strCsvFile))
		{
			CString strName, strTmp;
			strName.Format("ʱ��,��λʱ��,��λͼ���,");
			for (int i = 0; i < tPdInfo.vecNameList.size(); i++)
			{
				strTmp.Format("%s,", tPdInfo.vecNameList[i]);
				strName += strTmp;
			}
			strName += "\n";
			CStdioFile fl;
			if (fl.Open(strCsvFile, CFile::modeCreate | CFile::modeWrite | CFile::modeNoTruncate))
			{
				fl.WriteString(strName);
				fl.Close();
			}
		}

		//д����
		CString strValue, strTmp;
		strValue.Format("%04d-%02d-%02d %02d:%02d:%02d.%03d,", tTm.wYear, tTm.wMonth, tTm.wDay,
			tTm.wHour, tTm.wMinute, tTm.wSecond, tTm.wMilliseconds);
		strTmp.Format("%d,", nImageSN);
		strValue += strTmp;
		strTmp.Format("%s,", hvBiasValue[0].S().Text());
		strValue += strTmp;
		for (int i = 1; i < hvBiasValue.Length(); i++)
		{
			strTmp.Format("%0.4f,", hvBiasValue[i].D());
			strValue += strTmp;
		}
		strValue += "\n";

		CStdioFile file;
		if (!file.Open(strCsvFile, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite))
		{
			CString strException;
			strException.Format("CUpload_WriteBiasFile: ���ղ��������ļ�����,�޷�д������");
			theGlobal.m_Log.Exception(strException);
			return;
		}
		file.SeekToEnd();
		file.WriteString(strValue);
		file.Close();
	}
	catch (...) {}
}

void CUpload::ListenStartRunIgnoreCompen()
{
	//�ж��Ƿ�ͣ��
	const double dCurrentMeterCount = theGlobal.m_Counter.GetCurrentMeterCount();
	static double s_dMeterCurrent = 0;
	if (s_dMeterCurrent > dCurrentMeterCount)
		s_dMeterCurrent = dCurrentMeterCount;
	if (theGlobal.m_Speed.GetSpeed() < 0.1)
	{
		s_dMeterCurrent = dCurrentMeterCount;
		m_bAutoCompensationDisable = TRUE;
		/*CString strLog;
		strLog.Format("CUpload_ListenStartRunIgnoreCompen: ��⵽�豸ͣ�������ÿ�����������%0.2fm", m_tParam.dStartIgnoreMeter);
		theGlobal.m_Log.Std(strLog);*/
	}
	else
	{
		if (dCurrentMeterCount < (s_dMeterCurrent + abs(m_tParam.dStartIgnoreMeter)))
		{
			m_bAutoCompensationDisable = TRUE;
			/*CString strLog;
			strLog.Format("CUpload_RunIgnoreCompen: ��ʪĤ��������״̬:%s,����:%0.2fm,Ŀ������:%0.2fm", m_bAutoCompensationDisable ? "����" : "�ر�",
				theGlobal.m_Counter.GetCurrentCount().dTotalMeter, (s_dMeterCurrent + abs(m_tParam.dStartIgnoreMeter)));
			theGlobal.m_Log.Std(strLog);*/
		}
		else
		{
			m_bAutoCompensationDisable = FALSE;
			/*CString strLog;
			strLog.Format("CUpload_ListenStartRunIgnoreCompen: ��ʪĤ��������״̬:%s, ����:%0.2fm", m_bAutoCompensationDisable ? "����" : "�ر�", 
				theGlobal.m_Counter.GetCurrentCount().dTotalMeter);
			theGlobal.m_Log.Std(strLog);*/
		}
	}
}

BOOL CUpload::SendProductInfoToClient()
{
	if (!m_tParam.bUseDataUpload) return TRUE;
	tagPDInfo tPdInfo = theGlobal.m_ProductTool.GetCurrentProductInfo();
	if (tPdInfo.nPDType == ThreeCavity) return TRUE;

	//�����䷽��Ϣ
	m_hvProductInfo.Clear();
	//0У��ָ��
	//1��Ʒ��
	//2��Ʒ����
	//3�Ƿ����AT11
	//4�Ƿ���ڰ���
	//5Ϳ�����ͣ���˫��
	//Ϳ��������Ϳ������
	//������������������
	//����/at9��񣬰���/at9����
	//��λ��񣬴�λ����
	m_hvProductInfo.Append("CheckProductInfo");
	m_hvProductInfo.Append((HTuple)tPdInfo.strPDName);
	m_hvProductInfo.Append(tPdInfo.nPDType);
	m_hvProductInfo.Append(tPdInfo.bExistAT11);
	m_hvProductInfo.Append(tPdInfo.bExistBaseCoat);
	m_hvProductInfo.Append(tPdInfo.nCoatingType);
	//Ϳ��
	m_hvProductInfo.Append(tPdInfo.tParam.vecCoatWidth[0].dStandard);
	m_hvProductInfo.Append(tPdInfo.tParam.vecCoatWidth[0].dWarnTolerance);
	m_hvProductInfo.Append(tPdInfo.tParam.vecCoatWidth[0].dTolerance);
	//����
	for (int i = 0; i < tPdInfo.nPDType + 1; i++)
	{
		m_hvProductInfo.Append(tPdInfo.tParam.vecFoilWidth[i].dStandard);
		m_hvProductInfo.Append(tPdInfo.tParam.vecFoilWidth[i].dWarnTolerance);
		m_hvProductInfo.Append(tPdInfo.tParam.vecFoilWidth[i].dTolerance);
	}
	//AT11�Ͱ���
	if (tPdInfo.bExistAT11)
	{
		m_hvProductInfo.Append(tPdInfo.tParam.vecAT11Width[0].dStandard);
		m_hvProductInfo.Append(tPdInfo.tParam.vecAT11Width[0].dWarnTolerance);
		m_hvProductInfo.Append(tPdInfo.tParam.vecAT11Width[0].dTolerance);
	}
	else if (tPdInfo.bExistBaseCoat)
	{
		m_hvProductInfo.Append(tPdInfo.tParam.vecBaseCoatWidth[0].dStandard);
		m_hvProductInfo.Append(tPdInfo.tParam.vecBaseCoatWidth[0].dWarnTolerance);
		m_hvProductInfo.Append(tPdInfo.tParam.vecBaseCoatWidth[0].dTolerance);
	}
	if (tPdInfo.nCoatingType == TwoSide)
	{
		m_hvProductInfo.Append(tPdInfo.tParam.vecMisalignment[0].dStandard);
		m_hvProductInfo.Append(tPdInfo.tParam.vecMisalignment[0].dWarnTolerance);
		m_hvProductInfo.Append(tPdInfo.tParam.vecMisalignment[0].dTolerance);
	}

	for (int i = 0; i < MAX_CONNECT; i++)
	{
		try
		{
			CString strLog;
			if (theGlobal.m_Upload.m_bListenSuccess[i] == FALSE)
			{
				strLog.Format("�빤λ%d���ӶϿ�������У���䷽:%sʧ��", i + 2, tPdInfo.strPDName);
				theGlobal.m_Log.Communication(strLog);
				theGlobal.m_Upload.m_bSendCheckProductInfo[i] = FALSE;
				continue;
			}

			SendTuple(theGlobal.m_Upload.m_ClientSocket[i], m_hvProductInfo);

			strLog.Format("��λ%d����У���䷽:%s�ɹ�", i + 2, tPdInfo.strPDName);
			theGlobal.m_Log.Communication(strLog);
		}
		catch (HException&)
		{
			CString strLog;
			strLog.Format("�빤λ%d���ӶϿ�������У���䷽:%sʧ��", i + 2, tPdInfo.strPDName);
			theGlobal.m_Log.Communication(strLog);
			//theGlobal.m_Upload.m_bListenSuccess[i] = FALSE;
			theGlobal.m_Upload.m_bConnectClient[i] = FALSE;
			return FALSE;
		}
		theGlobal.m_Upload.m_bSendCheckProductInfo[i] = FALSE;
	}

	return TRUE;
}

BOOL CUpload::CheckProductInfoFromServer()
{
	tagPDInfo tPdInfo = theGlobal.m_ProductTool.GetCurrentProductInfo();
	tagPDInfo tPdInfoOld = theGlobal.m_ProductTool.GetCurrentProductInfo();
	if (tPdInfo.nPDType == ThreeCavity) return FALSE;
	if (m_hvProductInfo.Length() < 6) return FALSE;

	//0У��ָ��
	//1��Ʒ��
	//2��Ʒ����
	//3�Ƿ����AT11
	//4�Ƿ���ڰ���
	//5Ϳ�����ͣ���˫��
	//Ϳ�����Ϳ������
	//������������������
	//����/at9��񣬰���/at9����
	//��λ��񣬴�λ����
	BOOL bRet = TRUE;
	try
	{
		int nCount = 2;
		if (m_hvProductInfo[nCount] != tPdInfo.nPDType)
		{
			tPdInfo.nPDType = m_hvProductInfo[nCount];
			bRet = FALSE;
		}
		nCount++;
		if (m_hvProductInfo[nCount] != tPdInfo.bExistAT11)
		{
			tPdInfo.bExistAT11 = m_hvProductInfo[nCount];
			bRet = FALSE;
		}
		nCount++;
		if (m_hvProductInfo[nCount] != tPdInfo.bExistBaseCoat)
		{
			tPdInfo.bExistBaseCoat = m_hvProductInfo[nCount];
			bRet = FALSE;
		}
		nCount++;
		if (theGlobal.m_Project.GetProjectConfig().nSideCount == 2)
		{
			//˫���������ã���ҪУ��ͻ�ͷ�ĵ�˫�������Ƿ�һ��
			if (m_hvProductInfo[nCount] != tPdInfo.nCoatingType) bRet = FALSE;
		}
		nCount++;
		//Ϳ�����Ԥ�����NG���
		if (tPdInfo.tParam.vecCoatWidth.size() < tPdInfo.nPDType)
		{
			tPdInfo.tParam.vecCoatWidth.resize(tPdInfo.nPDType, tPdInfo.tParam.vecCoatWidth[0]);
		}
		if (fabs(m_hvProductInfo[nCount].D() - tPdInfo.tParam.vecCoatWidth[0].dStandard) > EPS)
		{
			tPdInfo.tParam.vecCoatWidth[0].dStandard = m_hvProductInfo[nCount].D();
			bRet = FALSE;
		}
		nCount++;
		if (fabs(m_hvProductInfo[nCount].D() - tPdInfo.tParam.vecCoatWidth[0].dWarnTolerance) > EPS)
		{
			tPdInfo.tParam.vecCoatWidth[0].dWarnTolerance = m_hvProductInfo[nCount];
			bRet = FALSE;
		}
		nCount++;
		if (fabs(m_hvProductInfo[nCount].D() - tPdInfo.tParam.vecCoatWidth[0].dTolerance) > EPS)
		{
			tPdInfo.tParam.vecCoatWidth[0].dTolerance = m_hvProductInfo[nCount];
			bRet = FALSE;
		}
		nCount++;
		//�������Ԥ�����NG���
		if (tPdInfo.tParam.vecFoilWidth.size() < tPdInfo.nPDType + 1)
		{
			tPdInfo.tParam.vecFoilWidth.resize(tPdInfo.nPDType + 1, tPdInfo.tParam.vecFoilWidth[0]);
		}
		for (int i = 0; i < tPdInfo.tParam.vecFoilWidth.size(); i++)
		{
			if (fabs(m_hvProductInfo[nCount].D() - tPdInfo.tParam.vecFoilWidth[i].dStandard) > EPS)
			{
				tPdInfo.tParam.vecFoilWidth[i].dStandard = m_hvProductInfo[nCount];
				bRet = FALSE;
			}
			nCount++;
			if (fabs(m_hvProductInfo[nCount].D() - tPdInfo.tParam.vecFoilWidth[i].dWarnTolerance) > EPS)
			{
				tPdInfo.tParam.vecFoilWidth[i].dWarnTolerance = m_hvProductInfo[nCount];
				bRet = FALSE;
			}
			nCount++;
			if (fabs(m_hvProductInfo[nCount].D() - tPdInfo.tParam.vecFoilWidth[i].dTolerance) > EPS)
			{
				tPdInfo.tParam.vecFoilWidth[i].dTolerance = m_hvProductInfo[nCount];
				bRet = FALSE;
			}
			nCount++;
		}
		if (tPdInfo.bExistAT11)
		{
			//�޸Ĺ������
			if (tPdInfo.tParam.vecAT11Width.size() < tPdInfo.nPDType*2)
			{
				tagTolerance tTol;
				if (tPdInfo.tParam.vecAT11Width.size() == 0)
				{
					tTol.dOffsetOL = 0;
					tTol.dStandard = 1.5;
					tTol.dStandard2 = 1.5;
					tTol.dTolerance = 0.5;
					tTol.dWarnTolerance = 0.3;
					tTol.OffsetList.push_back(0);
					if (tPdInfo.nCoatingType == TwoSide)
						tTol.OffsetList.push_back(0);
				}
				else
				{
					tTol = tPdInfo.tParam.vecAT11Width[0];
				}
				tPdInfo.tParam.vecAT11Width.resize(tPdInfo.nPDType*2, tTol);
			}
			if (fabs(m_hvProductInfo[nCount].D() - tPdInfo.tParam.vecAT11Width[0].dStandard) > EPS)
			{
				tPdInfo.tParam.vecAT11Width[0].dStandard = m_hvProductInfo[nCount];
				bRet = FALSE;
			}
			nCount++;
			if (fabs(m_hvProductInfo[nCount].D() - tPdInfo.tParam.vecAT11Width[0].dWarnTolerance) > EPS)
			{
				tPdInfo.tParam.vecAT11Width[0].dWarnTolerance = m_hvProductInfo[nCount];
				bRet = FALSE;
			}
			nCount++;
			if (fabs(m_hvProductInfo[nCount].D() - tPdInfo.tParam.vecAT11Width[0].dTolerance) > EPS)
			{
				tPdInfo.tParam.vecAT11Width[0].dTolerance = m_hvProductInfo[nCount];
				bRet = FALSE;
			}
			nCount++;
		}
		else if (tPdInfo.bExistBaseCoat)
		{
			//�޸Ĺ������
			if (tPdInfo.tParam.vecBaseCoatWidth.size() < tPdInfo.nPDType * 2)
			{
				tagTolerance tTol;
				if (tPdInfo.tParam.vecBaseCoatWidth.size() == 0)
				{
					tTol.dOffsetOL = 0;
					tTol.dStandard = 1.5;
					tTol.dStandard2 = 1.5;
					tTol.dTolerance = 0.5;
					tTol.dWarnTolerance = 0.3;
					tTol.OffsetList.push_back(0);
					if(tPdInfo.nCoatingType == TwoSide)
						tTol.OffsetList.push_back(0);
				}
				else
				{
					tTol = tPdInfo.tParam.vecBaseCoatWidth[0];
				}

				tPdInfo.tParam.vecBaseCoatWidth.resize(tPdInfo.nPDType * 2, tTol);
			}
			if (fabs(m_hvProductInfo[nCount].D() - tPdInfo.tParam.vecBaseCoatWidth[0].dStandard) > EPS)
			{
				tPdInfo.tParam.vecBaseCoatWidth[0].dStandard = m_hvProductInfo[nCount];
				bRet = FALSE;
			}
			nCount++;
			if (fabs(m_hvProductInfo[nCount].D() - tPdInfo.tParam.vecBaseCoatWidth[0].dWarnTolerance) > EPS)
			{
				tPdInfo.tParam.vecBaseCoatWidth[0].dWarnTolerance = m_hvProductInfo[nCount];
				bRet = FALSE;
			}
			nCount++;
			if (fabs(m_hvProductInfo[nCount].D() - tPdInfo.tParam.vecBaseCoatWidth[0].dTolerance) > EPS)
			{
				tPdInfo.tParam.vecBaseCoatWidth[0].dTolerance = m_hvProductInfo[nCount];
				bRet = FALSE;
			}
			nCount++;
		}
		if (theGlobal.m_Project.GetProjectConfig().nSideCount == 2 && tPdInfo.nCoatingType == TwoSide)
		{
			if (tPdInfo.tParam.vecMisalignment.size() < tPdInfo.nPDType * 2)
			{
				tagTolerance tTol;
				if (tPdInfo.tParam.vecMisalignment.size() == 0)
				{
					tTol.dOffsetOL = 0;
					tTol.dStandard = 0;
					tTol.dStandard2 = 0;
					tTol.dTolerance = 0.8;
					tTol.dWarnTolerance = 0.5;
					tTol.OffsetList.push_back(0);
					if (tPdInfo.nCoatingType == TwoSide)
						tTol.OffsetList.push_back(0);
				}
				else
				{
					tTol = tPdInfo.tParam.vecMisalignment[0];
				}

				tPdInfo.tParam.vecMisalignment.resize(tPdInfo.nPDType * 2, tTol);
			}
			if (fabs(m_hvProductInfo[nCount].D() - tPdInfo.tParam.vecMisalignment[0].dStandard) > EPS)
			{
				tPdInfo.tParam.vecMisalignment[0].dStandard = m_hvProductInfo[nCount];
				bRet = FALSE;
			}
			nCount++;
			if (fabs(m_hvProductInfo[nCount].D() - tPdInfo.tParam.vecMisalignment[0].dWarnTolerance) > EPS)
			{
				tPdInfo.tParam.vecMisalignment[0].dWarnTolerance = m_hvProductInfo[nCount];
				bRet = FALSE;
			}
			nCount++;
			if (fabs(m_hvProductInfo[nCount].D() - tPdInfo.tParam.vecMisalignment[0].dTolerance) > EPS)
			{
				tPdInfo.tParam.vecMisalignment[0].dTolerance = m_hvProductInfo[nCount];
				bRet = FALSE;
			}
			nCount++;
		}

		//�޸ĵ�ǰ�䷽
		if (!bRet)
		{
			theGlobal.m_ProductTool.SetProductInfo(theGlobal.m_ProductTool.GetCurrentProductSN(), tPdInfo);
			theGlobal.m_ProductTool.WriteProductParam(theGlobal.m_ProductTool.GetCurrentProductSN());

			theGlobal.m_ProductTool.ProductParamCorrected(tPdInfoOld, tPdInfo, TRUE);

			if (tPdInfoOld.nCoatingType != tPdInfo.nCoatingType
				|| tPdInfoOld.bExistAT11 != tPdInfo.bExistAT11
				|| tPdInfoOld.bExistBaseCoat != tPdInfo.bExistBaseCoat
				|| tPdInfoOld.nPDType != tPdInfo.nPDType)
			{
				theGlobal.PostMessageToMainDlg(WM_RESTARTPROCESS);
			}
			//���������Χ�ı䣬Ҳ������ʾ������ͼ
			theGlobal.m_pSubDispScope->PostMessageA(WM_REFRESH_DISP);
		}
	}
	catch (HException&e)
	{
		CString strLog;
		strLog.Format("У���䷽��Ϣ:%sʧ��", m_hvProductInfo[1].S().Text());
		theGlobal.m_Log.Communication(strLog);
		return FALSE;
	}
	return bRet;
}

BOOL CUpload::SendAlarmInfoToServer(const CErrorInfo::tagErrorInfo& tInfo, const BOOL& bStopMachine)
{
	if (theGlobal.m_tSystemSet.eSN == CGlobal::SN_Head) return TRUE;

	//������Ϣ
	HTuple hvAlarm;
	//0 ����ͷ
	//1 ��λ���
	//2 ������
	//3 ������Ϣ����
	//4 �������
	//5 0�ޣ�1�豸���ⱨ����2�豸ͣ��

	hvAlarm.Append("AlarmInfo");
	hvAlarm.Append(theGlobal.m_tSystemSet.eSN);
	hvAlarm.Append(tInfo.nID);
	hvAlarm.Append((HTuple)tInfo.strDescribe);
	hvAlarm.Append((HTuple)tInfo.strSolution);
	if (bStopMachine)
		hvAlarm.Append(1);
	else
		hvAlarm.Append(2);

	try
	{
		SendTuple(theGlobal.m_Upload.m_Socket, hvAlarm);
		CString strLog;
		strLog.Format("���ͱ�����Ϣ����λ1�ɹ�");
		//theGlobal.m_Log.Communication(strLog);
	}
	catch (HException e)
	{
		CString strLog;
		strLog.Format("��λ%d���Ͳ���ֵʧ��", theGlobal.m_tSystemSet.eSN);
		theGlobal.m_Log.Communication(strLog);
		m_bConnect = FALSE;
		return FALSE;
	}

	return TRUE;
}

void CUpload::RecvAlarmInfoFromClient(const HTuple& hvAlarmInfo, int nSN)
{
	if (hvAlarmInfo.Length() < 5)
	{
		CString strLog;
		strLog.Format("���չ�λ%d������Ϣʧ�ܣ���Ϣ��ȫ", nSN + 2);
		theGlobal.m_Log.Communication(strLog);
		return;
	}
	//������Ϣ
	HTuple hvAlarm = hvAlarmInfo;
	//0 ����ͷ
	//1 ��λ���
	//2 ������
	//3 ������Ϣ����
	//4 �������
	//5 0�ޣ�1������2ͣ��

	CErrorInfo::tagErrorInfo tInfo;
	int nStationSN = hvAlarmInfo[1];
	tInfo.nID = hvAlarmInfo[2];
	tInfo.strDescribe.Format("[��λ%d] %s", nStationSN, hvAlarmInfo[3].S().Text());
	tInfo.strSolution.Format("%s", hvAlarmInfo[4].S().Text());
	int nAlarmOrStop = hvAlarmInfo[5];

	theGlobal.SetAlarmInfo(tInfo);
	if (nAlarmOrStop == 1)
		theGlobal.m_CommunicationTool.CCDAlarm();
	else if (nAlarmOrStop == 2)
		theGlobal.m_CommunicationTool.CCDStop();

	CString strLog;
	strLog.Format("���չ�λ%d������Ϣ�ɹ�������ʾ", nSN + 2);
	theGlobal.m_Log.Communication(strLog);
}