#include "StdAfx.h"
#include "Upload.h"

#define EPS 1e-12	//���弫�ޣ������ж�����double�Ƿ����

HTupleVector CUpload::m_hvecInfoSendToServer(1);
HTupleVector CUpload::m_hvecInfoSendToClient(1);

CUpload::CUpload() :
	m_vecDataCollect(1)
	, m_vecRawData(1)
	, m_hvecCompen(1)
{
	m_bConnect = FALSE;
	m_hvSeconds = 0;
	m_hvFirstSend = TRUE;
	m_vecDataCollect.Clear();
	m_hvecCompen.Clear();

	CString strCounterPath;
	strCounterPath.Format("%s\\Config\\Public\\Count\\Count0.ini", GetCurrentAppPath());
	m_strBatchRecv = GetPrivateProfileCString("Batch", "Name", "None", strCounterPath);
}
CUpload::~CUpload()
{
	//WriteParam();
}
void T_ServerSocket()//��ͷ�Ͷ�Ϳ��ʱ�����
{
	if (!theGlobal.m_Upload.m_tParam.bUseDataUpload) return;
	try
	{
		for (int i = 0; i < 1; i++)
		{
			if (!theGlobal.m_Upload.m_tParam.bListenClient) continue;

			theGlobal.m_Upload.m_bListenSuccess = FALSE;
			theGlobal.m_Upload.m_bConnectClient = FALSE;
			theGlobal.m_Upload.m_bSendBatchOK = TRUE;
			theGlobal.m_Upload.m_bSendCheckProductInfo = TRUE;

			CString strLog;
			strLog.Format("������λ%d�ļ���:%s:%d", theGlobal.m_tSystemSet.eSN + 1, theGlobal.m_Upload.m_tParam.strAddressListen, theGlobal.m_Upload.m_tParam.nPortListen);
			theGlobal.m_Log.Communication(strLog);
		}
	}
	catch (HException e)
	{
	}
	HTuple hvTimeStamp = 0;
	while (!theGlobal.m_bExitThread)
	{
		for (int i = 0; i < 1; i++)
		{
			try
			{
				if (!theGlobal.m_Upload.m_tParam.bListenClient) continue;

				if (theGlobal.m_Upload.m_bListenSuccess == FALSE)
				{
					//�����Ͽ������´򿪼���
					HALCON_TRY(CloseSocket(theGlobal.m_Upload.m_ClientSocket));
					HALCON_TRY(CloseSocket(theGlobal.m_Upload.m_ServerSocket));
					theGlobal.m_Upload.m_bConnectClient = FALSE;
					try
					{
						HTuple hvName, hvValue;
						hvName.Append("protocol");
						hvName.Append("timeout");
						hvValue.Append("HALCON4");
						hvValue.Append(5);
						OpenSocketAccept(theGlobal.m_Upload.m_tParam.nPortListen, hvName, hvValue, &theGlobal.m_Upload.m_ServerSocket);

						//CString strLog;
						//strLog.Format("�ѿ�����λ%d�ļ���:%s:%d",i+2,theGlobal.m_Upload.m_tParam.strAddressListen,theGlobal.m_Upload.m_tParam.nPortListen);
						////theGlobal.m_Log.Communication(strLog);
						theGlobal.m_Upload.m_bListenSuccess = TRUE;
					}
					catch (HException e)
					{

					}
				}
				if (theGlobal.m_Upload.m_bConnectClient == FALSE)
				{
					HTuple hvClient;
					try
					{
						SocketAcceptConnect(theGlobal.m_Upload.m_ServerSocket, "false", &hvClient);

						HTuple hvValue1;
						GetSocketParam(hvClient, "address_info", &hvValue1);
						//SetSocketParam(hvClient, "timeout", 0.05);

						theGlobal.m_Upload.m_ClientSocket = hvClient;
						theGlobal.m_Upload.m_bConnectClient = TRUE;

						CString strlog;
						strlog.Format("�빤λ%d���罨���ɹ�.(IP:%s:%d)---(IP:%s:%d)", theGlobal.m_tSystemSet.eSN + 1, hvValue1[0].S().Text(), hvValue1[1].I(), hvValue1[2].S().Text(), hvValue1[3].I());
						theGlobal.m_Log.Communication(strlog);
					}
					catch (HException&e)
					{
						CString strMessage;
						strMessage.Format("%s(%s)", e.ErrorMessage().Text(), e.ProcName().Text());
						//theGlobal.m_Log.Std(strMessage);
						theGlobal.m_Upload.m_bConnectClient = FALSE;
						//������η���ʧ�ܣ��Ͽ��������������������ӿͻ���
						/*if (++nDisconnectCount > 60)
						{
							theGlobal.m_Upload.m_bListenSuccess = FALSE;
							nDisconnectCount = 0;
						}*/
					}
				}
				else
				{
					//�������5��û���κ���Ϣ���򲹳������ź�
					HTuple hvTimeStampTmp;
					CountSeconds(&hvTimeStampTmp);
					if ((hvTimeStampTmp.D() - hvTimeStamp.D()) > 5)
					{
						HTuple hvMsg = "HeartBeat";
						theGlobal.m_Upload.AddInfoToClient(hvMsg);
					}
					//������Ϣ
					while (theGlobal.m_Upload.m_hvecInfoSendToClient.Length() > 0)
					{
						if (theGlobal.m_Upload.m_bConnectClient == FALSE) break;

						HTuple hvInfoSend = theGlobal.m_Upload.m_hvecInfoSendToClient[0].T();
						{
							//ɾ��ͷ��Ԫ��
							CLockHelper lk(&theGlobal.m_Upload.m_csLock);
							theGlobal.m_Upload.m_hvecInfoSendToClient = theGlobal.m_Upload.m_hvecInfoSendToClient.Remove(0);
						}
						try
						{
							SendTuple(theGlobal.m_Upload.m_ClientSocket, hvInfoSend);

							CountSeconds(&hvTimeStamp);

							CString strLog;
							//strLog.Format("��λ%d������Ϣ:%s�ɹ�", theGlobal.m_tSystemSet.eSN + 1, hvInfoSend[0].S().Text());

							if ("ChangeBatch" == hvInfoSend[0])
							{
								strLog.Format("���;��(%s)����λ%d�ɹ�", hvInfoSend[1].S().Text(), theGlobal.m_tSystemSet.eSN + 1);
								theGlobal.m_Log.Communication(strLog);
							}
							else if ("CheckProductInfo" == hvInfoSend[0])
							{
								strLog.Format("�����䷽��Ϣ(%s)����λ%d�ɹ�", hvInfoSend[1].S().Text(), theGlobal.m_tSystemSet.eSN + 1);
								theGlobal.m_Log.Communication(strLog);
							}
							else if ("HeartBeat" == hvInfoSend[0])
							{
								if (hvInfoSend.Length()>1)
								{
									strLog.Format("���������ź�%d����λ%d�ɹ�", hvInfoSend[1].I(), theGlobal.m_tSystemSet.eSN + 1);
									theGlobal.m_Log.Communication(strLog);
								}
								else
								{
									strLog.Format("���������źŵ���λ%d�ɹ�", theGlobal.m_tSystemSet.eSN + 1);
									theGlobal.m_Log.Communication(strLog);
								}
							}
						}
						catch (HException&)
						{
							CString strLog;
							strLog.Format("�빤λ%d���ӶϿ���������Ϣ:%sʧ��", theGlobal.m_tSystemSet.eSN + 1, hvInfoSend[0].S().Text());
							theGlobal.m_Log.Communication(strLog);
							theGlobal.m_Upload.m_bConnectClient = FALSE;
							theGlobal.ShowUploadDlg(strLog);

							//������Ϣ����ʧ��,��Ҫ����
							if ("ChangeBatch" == hvInfoSend[0] ||
								"CheckProductInfo" == hvInfoSend[0] ||
								"AlarmInfo" == hvInfoSend[0] ||
								"HeartBeat" == hvInfoSend[0])
							{
								theGlobal.m_Upload.AddInfoToClient(hvInfoSend);
								break;
							}
						}
					}
				}
			}
			catch (HException& e)
			{
				CString strLog;
				strLog.Format("T_ServerSocket: �쳣,�������ӶϿ�%s (%s)", e.ErrorMessage().Text(), e.ProcName().Text());
				theGlobal.m_Log.Exception(strLog);
				theGlobal.m_Upload.m_bConnectClient = FALSE;
			}
		}
		Sleep(20);
	}

	//�˳�ǰ������
	//for (int i = 0; i < 1; i++)
	{
		HALCON_TRY(CloseSocket(theGlobal.m_Upload.m_ClientSocket));
		HALCON_TRY(CloseSocket(theGlobal.m_Upload.m_ServerSocket));
	}

}
void T_ClientSocket()//�ǻ�ͷ��ʱ�����
{
	if (!theGlobal.m_Upload.m_tParam.bUseDataUpload) return;

	HTuple hvTimeStamp;
	CountSeconds(&hvTimeStamp);
	while (!theGlobal.m_bExitThread)
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
					strLog.Format("�͹�λ%d���罨���ɹ�:%s:%d���ӳɹ�", theGlobal.m_tSystemSet.eSN-1,theGlobal.m_Upload.m_tParam.strAddresConnect, theGlobal.m_Upload.m_tParam.nPortConnect);
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
			else
			{
				//�������5��û���κ���Ϣ���򲹳������ź�
				HTuple hvTimeStampTmp;
				CountSeconds(&hvTimeStampTmp);

				//����
				static int nHeartCount = 0;
				if ((hvTimeStampTmp.D() - hvTimeStamp.D()) > 5)
				{
					HTuple hvMsg = "HeartBeat";
					hvMsg.Append(nHeartCount);
					theGlobal.m_Upload.AddInfoToServer(hvMsg);
					nHeartCount++;
				}
				//������Ϣ
				while (theGlobal.m_Upload.m_hvecInfoSendToServer.Length() > 0)
				{
					if(FALSE == theGlobal.m_Upload.m_bConnect) break;

					HTuple hvInfoSend = theGlobal.m_Upload.m_hvecInfoSendToServer[0].T();
					{
						//ɾ��ͷ��Ԫ��
						CLockHelper lk(&theGlobal.m_Upload.m_csLock);
						theGlobal.m_Upload.m_hvecInfoSendToServer = theGlobal.m_Upload.m_hvecInfoSendToServer.Remove(0);
					}
					try
					{
						SendTuple(theGlobal.m_Upload.m_Socket, hvInfoSend);

						CountSeconds(&hvTimeStamp);

						CString strLog;
						//strLog.Format("��λ%d������Ϣ:%s�ɹ�", theGlobal.m_tSystemSet.eSN - 1, hvInfoSend[0].S().Text());
						
						if ("Bias" == hvInfoSend[0])
						{
							strLog.Format("���Ͳ���ֵ����λ%d�ɹ�:,ͼ���%d,", theGlobal.m_tSystemSet.eSN - 1, hvInfoSend[5].I());
							for (int i = 7; i < hvInfoSend.Length(); i++)
							{
								CString strTmp;
								strTmp.Format("%0.4f,", hvInfoSend[i].D());
								strLog += strTmp;
							}
							theGlobal.m_Log.Communication(strLog);
						}
						else if ("ChangeBatch" == hvInfoSend[0])
						{
							strLog.Format("���������ź�(%s)����λ%d�ɹ�", hvInfoSend[1].S().Text(), theGlobal.m_tSystemSet.eSN - 1);
							theGlobal.m_Log.Communication(strLog);
						}
						else if ("CheckProductInfo" == hvInfoSend[0])
						{
							strLog.Format("�����䷽ͬ����Ϣ(%s)����λ%d�ɹ�", hvInfoSend[1].S().Text(), theGlobal.m_tSystemSet.eSN - 1);
							theGlobal.m_Log.Communication(strLog);
						}
						else if ("HeartBeat" == hvInfoSend[0])
						{
							strLog.Format("���������ź�(%d)����λ%d�ɹ�", hvInfoSend[1].I(), theGlobal.m_tSystemSet.eSN - 1);
							theGlobal.m_Log.Communication(strLog);
						}
					}
					catch (HException&)
					{
						CString strLog;
						strLog.Format("�빤λ%d���ӶϿ���������Ϣ:%sʧ��", theGlobal.m_tSystemSet.eSN - 1, hvInfoSend[0].S().Text());
						theGlobal.m_Log.Communication(strLog);
						theGlobal.ShowUploadDlg(strLog);
						theGlobal.m_Upload.m_bConnect = FALSE;

						//������Ϣ����ʧ��,��Ҫ����
						if ("ChangeBatch" == hvInfoSend[0] ||
							"CheckProductInfo" == hvInfoSend[0] || 
							"AlarmInfo" == hvInfoSend[0] ||
							"HeartBeat" == hvInfoSend[0])
						{
							theGlobal.m_Upload.AddInfoToServer(hvInfoSend);
							break;
						}
					}
				}
			}
		}
		catch (HException e)
		{
			CString strLog;
			strLog.Format("T_ClientSocket: �쳣,�������ӶϿ�%s (%s)", e.ErrorMessage().Text(), e.ProcName().Text());
			theGlobal.m_Log.Exception(strLog);
			theGlobal.m_Upload.m_bConnect = FALSE;
		}
		Sleep(20);
	}
	HALCON_TRY(CloseSocket(theGlobal.m_Upload.m_Socket));
}

BOOL CUpload::ReadParam()
{
	for (int i = 0; i < 1; i++)
	{
		CString strKeyName;
		strKeyName.Format("strAddressListen[%d]", i);
		m_tParam.strAddressListen = GetIniString("Upload", strKeyName, "127.0.0.1");
		strKeyName.Format("nPortListen[%d]", i);
		m_tParam.nPortListen = GetIniInt("Upload", strKeyName, 5001 + i);
		strKeyName.Format("bListenClient[%d]", i);
		m_tParam.bListenClient = GetIniInt("Upload", strKeyName, TRUE);
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
void CUpload::SendBias(HTuple hvValue, int nImageSN)//��β���Ͳ���ֵ��Ϳ2
{
	if (!m_tParam.bUseDataUpload/* || !m_tParam.bUseCompensationAuto*/) return;

	tagPDInfo tPdInfo = theGlobal.m_ProductTool.GetCurrentProductInfo();
	//0������Ϣ����
	//1���չ�λ��
	//2��Ʒ����
	//3�Ƿ����AT11
	//4�Ƿ���ڰ���
	//5ͼ���
	//6ʱ��
	CString strStation;
	if (theGlobal.m_tSystemSet.eSN-1 == CGlobal::SN_Head)
	{
		strStation = "Head";
	}
	else if (theGlobal.m_tSystemSet.eSN-1 == CGlobal::SN_2nd)
	{
		strStation = "2nd";
	}
	else if (theGlobal.m_tSystemSet.eSN-1 == CGlobal::SN_3rd)
	{
		strStation = "3rd";
	}
	else
	{
		strStation = "Tail";
	}
	CString strTime, strLog;
	try
	{
		/*SendTuple(theGlobal.m_Upload.m_Socket, hvValue);
		strLog.Format("���Ͳ���ֵ����λ2�ɹ�:,ͼ���%d,", nImageSN);
		for (int i = 7; i < hvValue.Length() - 1; i++)
		{
			CString strTmp;
			strTmp.Format("%0.4f,", hvValue.D());
			strLog += strTmp;
		}
		theGlobal.m_Log.Communication(strLog);*/

		TupleInsert(hvValue, 0, "Bias", &hvValue);
		TupleInsert(hvValue, 1, HTuple(strStation), &hvValue);
		TupleInsert(hvValue, 2, tPdInfo.nPDType, &hvValue);//��Ʒ����
		TupleInsert(hvValue, 3, tPdInfo.bExistAT11, &hvValue);//�Ƿ����AT11
		TupleInsert(hvValue, 4, tPdInfo.bExistBaseCoat, &hvValue);//�Ƿ���ڰ���
		TupleInsert(hvValue, 5, nImageSN, &hvValue);//ͼ���
		
		SYSTEMTIME tTime;
		::GetLocalTime(&tTime);
		strTime.Format("%02d:%02d:%02d.%03d", tTime.wHour, tTime.wMinute, tTime.wSecond, tTime.wMilliseconds);
		TupleInsert(hvValue, 6, HTuple(strTime), &hvValue);//ʱ��

		AddInfoToServer(hvValue);
	}
	catch (HException e)
	{
		strLog.Format("��λ%d���Ͳ���ֵʧ��", theGlobal.m_tSystemSet.eSN);
		theGlobal.m_Log.Communication(strLog);
		m_bConnect = FALSE;
	}
}

void CUpload::RecvBias(HTuple hvValue)
{
	CString strLog;
	strLog.Format("�յ���λ%d����ֵ(Bias):,ͼ���%d,", theGlobal.m_tSystemSet.eSN + 1, hvValue[0].I());
	for (int i = 2; i < hvValue.Length(); i++)
	{
		strLog.Format("%s,%.4f", strLog, hvValue[i].D());
	}
	theGlobal.m_Log.Communication(strLog);

	//ʵ���ǲ���ֵ����ҪһͿ��Ϳ��������,0Ԫ����ͼ���,1Ԫ����ʱ��
	TupleSelectRange(hvValue, 2, hvValue.Length() - 1, &m_hvBiasRecv);

	//���ں����жϽ��������Ƿ����ϴ���ͬ
	static HTuple hv_OldBiasRecv = m_hvBiasRecv;
	static int nBiasEqualCount = 0;
	HTuple hvSub, hvGreater, hvInc;
	TupleSub(m_hvBiasRecv, hv_OldBiasRecv, &hvSub);
	TupleGreaterElem(hvSub.TupleAbs(), 0.001, &hvGreater);
	TupleFind(hvGreater, 1, &hvInc);
	if (hvInc == -1 || hvInc.Length() == 0)
	{
		nBiasEqualCount++;
		if (nBiasEqualCount > 5)
		{
			//�жϽ��յ��������Ƿ�δ����
			
			m_bBiasRecv = TRUE;
			CString strLog;
			strLog.Format("CUpload_RecvBias: ����%d���յ��Ϲ�λ�����Ĳ���ֵ��ȫ��ͬ�������㲹��",nBiasEqualCount);
			theGlobal.m_Log.Std(strLog);
			theGlobal.ShowUploadDlg(strLog);
			nBiasEqualCount = 0;
		}
	}
	else
	{
		hv_OldBiasRecv = m_hvBiasRecv;
		nBiasEqualCount = 0;
		m_bBiasRecv = FALSE;
	}

}
BOOL CUpload::IsNetConnect()
{
	BOOL bConnect = TRUE;
	if (theGlobal.m_tSystemSet.eSN == CGlobal::SN_Head)
	{
		if (!m_tParam.bUseDataUpload || (!m_bListenSuccess&&m_tParam.bListenClient) || (!m_bListenSuccess&&m_tParam.bListenClient))bConnect = FALSE;
	}
	else if (theGlobal.m_tSystemSet.eSN == CGlobal::SN_2nd)
	{
		if (theGlobal.m_tSystemSet.eDevType == CGlobal::DT_DoubleLayer)
		{
			if (!m_tParam.bUseDataUpload || (!m_bListenSuccess&&m_tParam.bListenClient) || !m_bConnect)bConnect = FALSE;
		}
		else
		{
			if (!m_tParam.bUseDataUpload || !m_bConnect) bConnect = FALSE;
		}
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

	//for (int i = 0; i < 1; i++)
	{
		try
		{
			HTuple hvBatch;
			hvBatch.Append("ChangeBatch");
			hvBatch.Append(HTuple(strBatch));

			AddInfoToClient(hvBatch);
			theGlobal.m_Upload.m_bSendBatchOK = FALSE;
		}
		catch (HException&)
		{
			//CString strLog;
			//strLog.Format("�빤λ%d���ӶϿ������;��:%sʧ��", i + 2, strBatch);
			//theGlobal.m_Log.Communication(strLog);
			////theGlobal.m_Upload.m_bListenSuccess = FALSE;
			//theGlobal.m_Upload.m_bConnectClient = FALSE;
		}
	}
}

void CUpload::RecvBatchFromServer(CString strBatch)
{
	m_strBatchRecv = strBatch;
	CString strLog;
	strLog.Format("���չ�λ1���͵ľ��:%s", strBatch);
	theGlobal.m_Log.Communication(strLog);
}

void CUpload::AddInfoToClient(HTuple hvInfo)
{
	if (theGlobal.m_tSystemSet.eSN != CGlobal::SN_Head && theGlobal.m_tSystemSet.eSN != CGlobal::SN_2nd) return;
	try
	{
		{
			CLockHelper lk(&m_csLock);
			int nSameCount = 0;
			for (int i=0;i<m_hvecInfoSendToClient.Length();i++)
			{
				if (m_hvecInfoSendToClient[i].T()[0] == hvInfo[0])
				{
					m_hvecInfoSendToClient[i].T() = hvInfo;
					nSameCount++;
				}
			}
			if(nSameCount == 0)
				m_hvecInfoSendToClient[m_hvecInfoSendToClient.Length()].T() = hvInfo;
		}
		
		if (m_hvecInfoSendToClient.Length() > 10)
		{
			CString strLog;
			strLog.Format("CUpload_SendInfoToClient: Ҫ���͸���λ%d����Ϣ����10��", theGlobal.m_tSystemSet.eSN + 1);
			theGlobal.m_Log.Exception(strLog);
		}
	}
	catch (HException&e)
	{
		CString strLog;
		strLog.Format("CUpload_SendInfoToClient: %s (%s)", e.ErrorMessage().Text(), e.ProcName().Text());
		theGlobal.m_Log.Exception(strLog);
	}
}

void CUpload::AddInfoToServer(HTuple hvInfo)
{
	//if (theGlobal.m_tSystemSet.eSN == CGlobal::SN_Head) return;
	try
	{
		{
			CLockHelper lk(&m_csLock);
			int nSameCount = 0;
			for (int i = 0; i < m_hvecInfoSendToServer.Length(); i++)
			{
				if (m_hvecInfoSendToServer[i].T()[0] == hvInfo[0])
				{
					m_hvecInfoSendToServer[i].T() = hvInfo;
					nSameCount++;
				}
			}
			if (nSameCount == 0)
			m_hvecInfoSendToServer[m_hvecInfoSendToServer.Length()].T() = hvInfo;
		}
		
		if (m_hvecInfoSendToServer.Length() > 10)
		{
			CString strLog;
			strLog.Format("CUpload_SendInfoToServer: Ҫ���͸���λ%d����Ϣ����10��", theGlobal.m_tSystemSet.eSN - 1);
			theGlobal.m_Log.Exception(strLog);
		}
	}
	catch (HException&e)
	{
		CString strLog;
		strLog.Format("CUpload_SendInfoToServer: %s (%s)", e.ErrorMessage().Text(), e.ProcName().Text());
		theGlobal.m_Log.Exception(strLog);
	}
}

BOOL CUpload::WriteParam()
{
	for (int i = 0; i < 1; i++)
	{
		CString strKeyName;
		strKeyName.Format("strAddressListen[%d]", i);
		WriteIniString("Upload", strKeyName, m_tParam.strAddressListen);
		strKeyName.Format("nPortListen[%d]", i);
		WriteIniInt("Upload", strKeyName, m_tParam.nPortListen);
		strKeyName.Format("bListenClient[%d]", i);
		WriteIniInt("Upload", strKeyName, m_tParam.bListenClient);
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
	//HDevThreadContext Context;
	//HDevThread* pThread = new HDevThread(Context,
	//	(void*)HThreadUpload, 1, 0);//����һ������1��������߳�;
	//pThread->SetInputCtrlParamTuple(0, m_hvQueue);
	//pThread->Start();

	if (CGlobal::SN_Head == theGlobal.m_tSystemSet.eSN)
	{
		//��ͷ��Ϊ���������͹�λ2ͨ��
		AfxBeginThread((AFX_THREADPROC)T_ServerSocket, NULL);
		AfxBeginThread((AFX_THREADPROC)T_ServerRecvData, NULL);
	}
	else if (CGlobal::SN_2nd == theGlobal.m_tSystemSet.eSN)
	{
		if (CGlobal::DT_DoubleLayer == theGlobal.m_tSystemSet.eDevType)
		{
			//˫�㣬��λ2��Ϊ���������͹�λ3ͨ��
			AfxBeginThread((AFX_THREADPROC)T_ServerSocket, NULL);
			AfxBeginThread((AFX_THREADPROC)T_ServerRecvData, NULL);
		}
		//˫��͵��㣬��λ2����Ϊ�ͻ���
		AfxBeginThread((AFX_THREADPROC)T_ClientSocket, NULL);
		AfxBeginThread((AFX_THREADPROC)T_ClientRecvData, NULL);
	}
	else
	{
		//��β��������λ����Ϊ�ͻ���,������һ����λ
		AfxBeginThread((AFX_THREADPROC)T_ClientSocket, NULL);
		AfxBeginThread((AFX_THREADPROC)T_ClientRecvData, NULL);
	}

	//�����򿪾��
	if (theGlobal.m_tSystemSet.eSN == CGlobal::SN_Head)
	{
		theGlobal.m_Upload.SendBatchToClient(theGlobal.GetCurrentBatch());
	}

	m_bBiasRecv = FALSE;
	return TRUE;
}

void T_ServerRecvData()
{
	if (!theGlobal.m_Upload.m_tParam.bUseDataUpload) return;

	HTuple hvTimeStampe;
	HTuple hvTimeStampeListen;
	CountSeconds(&hvTimeStampe);
	CountSeconds(&hvTimeStampeListen);

	BOOL bConnectAlarm = FALSE;
	BOOL bConnectLongAlarm = FALSE;

	while (!theGlobal.m_bExitThread)
	{
		HTuple hvDataType;
		//for (int i = 0; i < 1; i++)
		{
			if (!theGlobal.m_Upload.m_tParam.bListenClient) continue;
			try
			{
				try
				{
					//�ж���λ
					static HTuple s_hvCount = HTuple().TupleGenConst(2, 0);
					if (theGlobal.m_Upload.m_bSendCheckProductInfo == FALSE)
					{
						s_hvCount[0] += 1;
						if (s_hvCount[0] > 6000)//2����û���յ��䷽У�鷴�������ٽ���
						{
							s_hvCount[0] = 0;
							theGlobal.m_Upload.m_bSendCheckProductInfo = TRUE;
							CString strInfo;
							strInfo.Format("��λ%d���䷽У��ʧ�ܣ�δ�յ���λ%d���䷽У�鷴��",
								theGlobal.m_tSystemSet.eSN + 1, theGlobal.m_tSystemSet.eSN + 1);
							theGlobal.m_Log.Alarm(strInfo);
						}
					}
					else
					{
						s_hvCount[0] = 0;
					}
					//���;�ŷ���
					if (theGlobal.m_Upload.m_bSendBatchOK == FALSE)
					{
						s_hvCount[1] += 1;
						if (s_hvCount[1] > 3000)//2����û���յ��䷽У�鷴�������ٽ���
						{
							s_hvCount[1] = 0;
							theGlobal.m_Upload.m_bSendBatchOK = TRUE;
							CString strInfo;
							strInfo.Format("��λ%d����ʧ��,1����δ�յ���λ%d�Ļ�����,���в���",
								theGlobal.m_tSystemSet.eSN + 1, theGlobal.m_tSystemSet.eSN + 1);
							theGlobal.m_Log.Alarm(strInfo);
							theGlobal.ShowUploadDlg(strInfo);
							theGlobal.m_Upload.SendBatchToClient(theGlobal.GetCurrentBatch());
						}
					}
					else
					{
						s_hvCount[1] = 0;
					}
				}
				catch (HException&)
				{

				}

				//�ж�60sû�н��յ��ͻ��˵��κ���Ϣ���Ͽ�����
				if (theGlobal.m_Upload.m_bConnectClient == TRUE)
				{
					CountSeconds(&hvTimeStampeListen);
					/*bConnectLongAlarm = FALSE;
					CountSeconds(&hvTimeStampeListen);
					HTuple hvTimeTmp;
					CountSeconds(&hvTimeTmp);
					if ((hvTimeTmp.D() - hvTimeStampe.D() > 120) && bConnectAlarm)
					{
						bConnectAlarm = FALSE;
						theGlobal.m_Upload.m_bConnectClient = FALSE;
						CountSeconds(&hvTimeStampe);
						CString strMessage;
						strMessage.Format("2����δ�յ���λ%d����Ϣ���ѶϿ�����", theGlobal.m_tSystemSet.eSN + 1);
						theGlobal.m_Log.Communication(strMessage);
						theGlobal.m_Log.Alarm(strMessage);
					}*/

					//�ͻ��˽��յ���Ϣ
					GetNextSocketDataType(theGlobal.m_Upload.m_ClientSocket, &hvDataType);
				}
				else if (theGlobal.m_Upload.m_bConnectClient == FALSE /*&& !bConnectLongAlarm*/)
				{
					bConnectAlarm = TRUE;
					HTuple hvTimeTmp;
					CountSeconds(&hvTimeTmp);
					if ((hvTimeTmp.D() - hvTimeStampeListen.D() > 60 * 3))
					{
						bConnectLongAlarm = TRUE;
						theGlobal.m_Upload.m_bListenSuccess = FALSE;
						CountSeconds(&hvTimeStampeListen);
						CountSeconds(&hvTimeStampe);
						CString strMessage;
						strMessage.Format("3����δ�����Ϲ�λ%d������", theGlobal.m_tSystemSet.eSN + 1);
						theGlobal.m_Log.Communication(strMessage);
						theGlobal.ShowUploadDlg(strMessage);
					}

					Sleep(20);
					continue;
				}
				else
				{
					Sleep(20);
					continue;
				}
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
			if ("tuple" == strRecvData)
			{
				CountSeconds(&hvTimeStampe);//���յ���Ϣ�͸���ʱ��
				try
				{
					HTuple hvData;
					ReceiveTuple(theGlobal.m_Upload.m_ClientSocket, &hvData);
					if (hvData.Length() > 1 && "Bias" == hvData[0])
					{
						tagPDInfo tPdInfo = theGlobal.m_ProductTool.GetCurrentProductInfo();
						//0������Ϣ����
						//1��λ��
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
					else if ("HeartBeat" == hvData[0])
					{
						CString strlog;
						strlog.Format("���չ�λ%d�������������ź�%d�ɹ�", theGlobal.m_tSystemSet.eSN + 1,hvData[1].I());
						theGlobal.m_Log.Communication(strlog);
						//��������
						theGlobal.m_Upload.AddInfoToClient(hvData);
					}
					else if (hvData.Length() > 1 && "ChangeBatch" == hvData[0] && 
						theGlobal.GetCurrentBatch() == hvData[1].S().Text())
					{
						theGlobal.m_Upload.m_bSendBatchOK = TRUE;

						CString strlog;
						strlog.Format("���չ�λ%d����(%s)��ɷ����ɹ�", theGlobal.m_tSystemSet.eSN + 1, hvData[1].S().Text());
						theGlobal.m_Log.Communication(strlog);
					}
					else if (hvData.Length() > 1 && "CheckProductInfo" == hvData[0])
					{
						CString strInfo;
						if (hvData[1] == "OK")
						{
							//У��ɹ�
							strInfo.Format("��λ%d���䷽У�����", theGlobal.m_tSystemSet.eSN + 1);
						}
						else
						{
							//У��ʧ��
							strInfo.Format("��λ%d���䷽У�����,�䷽��Ϣ��һ��,��λ%d�䷽��Ϣ�Ѹ���", 
								theGlobal.m_tSystemSet.eSN + 1, theGlobal.m_tSystemSet.eSN + 1);
							theGlobal.SetAlarmInfo(strInfo);
						}
						theGlobal.m_Log.Communication(strInfo);
						theGlobal.m_Upload.m_bSendCheckProductInfo = TRUE;
					}
					else if ("AlarmInfo" == hvData[0])
					{
						//����������λ�ı�����Ϣ
						theGlobal.m_Upload.RecvAlarmInfoFromClient(hvData, theGlobal.m_tSystemSet.eSN-1);
						//������Ϣ
						if (theGlobal.m_tSystemSet.eSN != CGlobal::SN_Head)
						{
							theGlobal.m_Upload.AddInfoToServer(hvData);
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
	while (!theGlobal.m_bExitThread)
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
					strMessage.Format("1����δ���յ���λ%d�������ѶϿ�����", theGlobal.m_tSystemSet.eSN - 1);
					theGlobal.m_Log.Communication(strMessage);
					//theGlobal.m_Log.Alarm(strMessage);
					theGlobal.ShowUploadDlg(strMessage);

					bConnectAlarm = FALSE;
				}

				//�ͻ��˽��յ���Ϣ
				GetNextSocketDataType(theGlobal.m_Upload.m_Socket, &hvDataType);
			}
			else if (theGlobal.m_Upload.m_bConnect == FALSE)
			{
				bConnectAlarm = TRUE;
				CountSeconds(&hvTimeStampe);

				Sleep(20);
				continue;
			}
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
		if ("tuple" == hvDataType)
		{
			CountSeconds(&hvTimeStampe);
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
					HTuple hvMsg;
					hvMsg.Append("ChangeBatch");
					hvMsg.Append((HTuple)strBatch);
					theGlobal.m_Upload.AddInfoToServer(hvMsg);

					if (theGlobal.m_tSystemSet.eSN == CGlobal::SN_2nd &&
						theGlobal.m_tSystemSet.eDevType == CGlobal::DT_DoubleLayer)
					{
						//˫��Ϳ����λ2Ҫת����Ϣ����β
						theGlobal.m_Upload.AddInfoToClient(hvData);
					}
				}
				else if ("HeartBeat" == hvData[0])
				{
					//HTuple hvMsg = "HeartBeat";
					//theGlobal.m_Upload.AddInfoToServer(hvMsg);
					//��������������
					CString strLog;
					strLog.Format("�յ���λ%d���������������ź�(%d)�ɹ�", theGlobal.m_tSystemSet.eSN - 1, hvData[1].I());
					theGlobal.m_Log.Communication(strLog);
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
					theGlobal.m_Log.Communication(strInfo);
					//������Ϣ
					theGlobal.m_Upload.AddInfoToServer(hvMsg);
					
					if (theGlobal.m_tSystemSet.eSN == CGlobal::SN_2nd &&
						theGlobal.m_tSystemSet.eDevType == CGlobal::DT_DoubleLayer)
					{
						//˫��Ϳ����λ2Ҫת����Ϣ����β
						theGlobal.m_Upload.AddInfoToClient(hvData);
					}
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
			if (dMeterCount > (m_tParam.dToNextStationMeter + m_tParam.dCompensationInvertal))
				m_vecRawData = m_vecRawData.Remove(0);
		}

		//if (!m_tParam.bUseDataUpload/* || !m_tParam.bUseCompensationAuto*/) return;
		if (!IsNetConnect())
		{
			//ͨѶ״̬�ж�
			CString strLog;
			strLog.Format("CUpload_Action: ͨѶ���ӶϿ��������㲹��");
			theGlobal.m_Log.Std(strLog);
			return;
		}

		const double dCurrentMeterCount = theGlobal.m_Counter.GetCurrentMeterCount();
		static double s_dMeterCount = dCurrentMeterCount;

		//��ֹ�������㣬�޷�����ֵ
		if (s_dMeterCount > dCurrentMeterCount)
			s_dMeterCount = dCurrentMeterCount;

		if (theGlobal.m_tSystemSet.eSN == CGlobal::SN_2nd)
		{
			//��λ2�Ƚ��ջ�β���ݣ����㲹��ֵ
			if (m_tParam.bUseCompensationAuto && m_hvBiasRecv.Length() == C.vecB[0].tMeasure.tSize.hv_WidthOrigin.Length())
			{
				//ȡ�ӳٵ���β�����������ͷ���ף�����һ����ֵ���ͽ��յ�����������õ����߲���ֵ
				try
				{
					//if (theGlobal.m_Upload.m_bAutoCompensationDisable)
					//{
					//	if (m_hvecCompen.Length() == 0)return;
					//	//��֮ǰ�Ĳ���ֵ���м���
					//	HTuple hvSize;
					//	for (int i = 0; i < m_hvecCompen[0].T().Length(); i++)
					//	{
					//		HTuple hvTmp;
					//		for (int j = 0; j < m_hvecCompen.Length(); j++)
					//		{
					//			hvTmp.Append(m_hvecCompen[j].T()[i]);
					//		}
					//		hvSize.Append(hvTmp.TupleMedian().D());
					//	}
					//	m_hvCompensation = hvSize;
					//	theGlobal.m_ProductTool.UpdateOnlineOffset(m_hvCompensation);
					//	CString strLog = "Upload_Action: �������ÿ���,ѡȡ�Ĳ���ֵΪ:,";
					//	for (int i = 0; i < m_hvCompensation.Length(); i++)
					//	{
					//		CString strVal;
					//		strVal.Format("%0.4f,", m_hvCompensation.D());
					//		strLog += strVal;
					//	}
					//	/*strLog += "�յ��¹�λ�Ĳ�������Ϊ:,";
					//	for (int i = 0; i < m_hvBiasRecv.Length(); i++)
					//	{
					//		CString strVal;
					//		strVal.Format("%0.4f,", m_hvBiasRecv.D());
					//		strLog += strVal;
					//	}
					//	strLog += "��ǰ��Ӧ����ֵΪ:,";
					//	for (int i = 0; i < hvData.Length(); i++)
					//	{
					//		CString strVal;
					//		strVal.Format("%0.4f,", hvData.D());
					//		strLog += strVal;
					//	}*/
					//	theGlobal.m_Log.Std(strLog);

					//	return;
					//}

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
								hvTmp2.Append(m_vecRawData[j].T()[i].D());
								HTuple hv_FindResult0 = m_vecRawData[j].T().TupleFind(0);
								HTuple hv_FindResultNoData = m_vecRawData[j].T().TupleFind(NO_DATA);
								if (hv_FindResult0 != -1 || hv_FindResultNoData != -1) continue;

								hvTmp.Append(m_vecRawData[j].T()[i].D());
							}
							TupleMedian(hvTmp, &hvTmp);
							hvData.Append(hvTmp);
							if (hvTmp2.TupleFind(0).Length() > 1
								|| hvTmp2.TupleFind(NO_DATA).Length() > 1)
							{
								//��Чֵ�ж�
								CString strLog;
								strLog.Format("CUpload_Action: ��Чֵ���࣬�����㲹��");
								theGlobal.m_Log.Std(strLog);
								return;
							}
							else if (hvTmp2.Length() > 5 && hvTmp2.TupleDeviation().D() > 0.8)
							{
								//�����׼��
								CString strLog;
								strLog.Format("CUpload_Action: �ռ���׼��%0.2f������0.8�������㲹��", hvTmp2.TupleDeviation().D());
								theGlobal.m_Log.Std(strLog);
								return;
							}
						}
						if (hvData.Length() == m_hvBiasRecv.Length())
						{
							HTuple hv_FindResult0 = m_hvBiasRecv.TupleFind(0);
							HTuple hv_FindResultNoData = m_hvBiasRecv.TupleFind(NO_DATA);
							if (hv_FindResult0 != -1 || hv_FindResultNoData != -1) return;
							//�жϽ��յ��������Ƿ�ÿ�ζ���ͬ��������
							if (m_bBiasRecv)
							{
								CString strLog, strAllData, strBiasRecvData = "";
								for (int i = 0; i < m_hvBiasRecv.Length(); i++)
								{
									strBiasRecvData.Format("%0.2f,", m_hvBiasRecv[i].D());
									strAllData += strBiasRecvData;
								}
								strLog.Format("CUpload_Action: ����������ȫ��ͬ%s�������㲹��", strAllData);
								theGlobal.m_Log.Std(strLog);
								return;
							}
							TupleSub(m_hvBiasRecv, hvData, &m_hvCompensation);
							TupleMult(m_hvCompensation, m_tParam.dScale, &m_hvCompensation);
							//��Ҫ������ֵ���¸����߲�������
							theGlobal.m_ProductTool.UpdateOnlineOffset(m_hvCompensation);
							//������ʱ���¼����Ĳ���ֵ
							/*m_hvecCompen[m_hvecCompen.Length()].T() = m_hvCompensation;
							if (m_hvecCompen.Length() > 3) m_hvecCompen = m_hvecCompen.Remove(0);*/
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

			//˫�㣬��Ϳ�����ݸ���ͷ��������������Ĳ�������,�������ҵ�����������ͷ
			//�ռ�����
			if (theGlobal.m_tSystemSet.eDevType == CGlobal::DT_DoubleLayer)
			{
				if (C.vecB.size() != 2) return;
				m_vecDataCollect[m_vecDataCollect.Length()].T() = C.vecB[1].tMeasure.tSize.hv_Width.TupleInverse();
			}
			else
			{
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
							hvTmp.Append(m_vecDataCollect[j].T()[i].D());
						}
						TupleMedian(hvTmp, &hvTmp);
						hvData.Append(hvTmp);
					}
					if (hvData.Length() == m_vecDataCollect[m_vecDataCollect.Length() - 1].T().Length())
						SendBias(hvData, C.vecB[0].vecA[0].hvImageSN.I());
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
		else if (theGlobal.m_tSystemSet.eSN == CGlobal::SN_3rd)
		{
			//��β�����ݸ���Ϳ����������������ݼ��ɣ��������ҵߵ�����
			m_vecDataCollect[m_vecDataCollect.Length()].T() = C.vecB[0].tMeasure.tSize.hv_Width;

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
							hvTmp.Append(m_vecDataCollect[j].T()[i].D());
						}
						TupleMedian(hvTmp, &hvTmp);
						hvData.Append(hvTmp);
					}
					//2021.7.14��ʱ���ԣ������Ͳ�������
					if (hvData.Length() == m_vecDataCollect[m_vecDataCollect.Length() - 1].T().Length())
						SendBias(hvData, C.vecB[0].vecA[0].hvImageSN.I());
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
		else if(theGlobal.m_tSystemSet.eSN == CGlobal::SN_Head)
		{
			//��ͷֻ�������ݴ���
			if (m_tParam.bUseCompensationAuto && m_hvBiasRecv.Length() == C.vecB[0].tMeasure.tSize.hv_WidthOrigin.Length())
			{
				//ȡ�ӳٵ���Ϳ�����������ͷ���ף�����һ����ֵ���ͽ��յ�����������õ����߲���ֵ
 			//	if (theGlobal.m_Upload.m_bAutoCompensationDisable)
				//{
				//	if (m_hvecCompen.Length() == 0)return;
				//	//��֮ǰ�Ĳ���ֵ���м���
				//	HTuple hvSize;
				//	for (int i = 0; i < m_hvecCompen[0].T().Length(); i++)
				//	{
				//		HTuple hvTmp;
				//		for (int j = 0; j < m_hvecCompen.Length(); j++)
				//		{
				//			hvTmp.Append(m_hvecCompen[j].T()[i]);
				//		}
				//		hvSize.Append(hvTmp.TupleMedian().D());
				//	}
				//	m_hvCompensation = hvSize;
				//	theGlobal.m_ProductTool.UpdateOnlineOffset(m_hvCompensation);
				//	CString strLog = "Upload_Action: �������ÿ���,ѡȡ�Ĳ���ֵΪ:,";
				//	for (int i = 0; i < m_hvCompensation.Length(); i++)
				//	{
				//		CString strVal;
				//		strVal.Format("%0.4f,", m_hvCompensation.D());
				//		strLog += strVal;
				//	}
				//	/*strLog += "�յ��¹�λ�Ĳ�������Ϊ:,";
				//	for (int i = 0; i < m_hvBiasRecv.Length(); i++)
				//	{
				//		CString strVal;
				//		strVal.Format("%0.4f,", m_hvBiasRecv.D());
				//		strLog += strVal;
				//	}
				//	strLog += "��ǰ��Ӧ����ֵΪ:,";
				//	for (int i = 0; i < hvData.Length(); i++)
				//	{
				//		CString strVal;
				//		strVal.Format("%0.4f,", hvData.D());
				//		strLog += strVal;
				//	}*/
				//	theGlobal.m_Log.Std(strLog);

				//	return;
				//}
				{
					CString strLog;
					strLog.Format("CUpload_Action: ��ʼ�����ʪĤ����");
					theGlobal.m_Log.Debug(strLog);
				}
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
							HTuple hvTmp, hvTmp2;
							for (int j = 0; j < nNum5m; j++)
							{
								hvTmp2.Append(m_vecRawData[j].T()[i].D());
								HTuple hv_FindResult0 = m_vecRawData[j].T().TupleFind(0);
								HTuple hv_FindResultNoData = m_vecRawData[j].T().TupleFind(NO_DATA);
								if (hv_FindResult0 != -1 || hv_FindResultNoData != -1) continue;

								hvTmp.Append(m_vecRawData[j].T()[i].D());
							}
							TupleMedian(hvTmp, &hvTmp);
							hvData.Append(hvTmp);
							if (hvTmp2.TupleFind(0).Length() > 2
								|| hvTmp2.TupleFind(NO_DATA).Length() > 2)
							{
								//��Чֵ�ж�
								CString strLog;
								strLog.Format("CUpload_Action: ��Чֵ���࣬�����㲹��");
								theGlobal.m_Log.Std(strLog);
								return;
							}
							else if (hvTmp2.Length() > 5 && hvTmp2.TupleDeviation().D() > 0.8)
							{
								//�����׼��
								CString strLog;
								strLog.Format("CUpload_Action: �ռ���׼��%0.2f������0.8�������㲹��", hvTmp2.TupleDeviation().D());
								theGlobal.m_Log.Std(strLog);
								return;
							}
						}
						if (hvData.Length() == m_hvBiasRecv.Length())
						{
							HTuple hv_FindResult0 = m_hvBiasRecv.TupleFind(0);
							HTuple hv_FindResultNoData = m_hvBiasRecv.TupleFind(NO_DATA);
							if (hv_FindResult0 != -1 || hv_FindResultNoData != -1)
							{
								CString strLog;
								strLog.Format("CUpload_Action: m_hvBiasRecv����Чֵ");
								theGlobal.m_Log.Debug(strLog);
								return;
							}
							//�жϽ��յ��������Ƿ�ÿ�ζ���ͬ��������
							//�жϽ��յ��������Ƿ�ÿ�ζ���ͬ��������
							if (m_bBiasRecv)
							{
								CString strLog, strAllData, strBiasRecvData = "";
								for (int i = 0; i < m_hvBiasRecv.Length(); i++)
								{
									strBiasRecvData.Format("%0.2f,", m_hvBiasRecv[i].D());
									strAllData += strBiasRecvData;
								}
								strLog.Format("CUpload_Action: ����������ȫ��ͬ%s�������㲹��",  strAllData);
								theGlobal.m_Log.Std(strLog);
								return;
							}

							TupleSub(m_hvBiasRecv, hvData, &m_hvCompensation);
							TupleMult(m_hvCompensation, m_tParam.dScale, &m_hvCompensation);
							//��Ҫ������ֵ���¸����߲�������
							theGlobal.m_ProductTool.UpdateOnlineOffset(m_hvCompensation);
							//������ʱ���¼����Ĳ���ֵ
							/*m_hvecCompen[m_hvecCompen.Length()].T() = m_hvCompensation;
							if (m_hvecCompen.Length() > 3) m_hvecCompen = m_hvecCompen.Remove(0);*/
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
						else
						{
							CString strLog;
							strLog.Format("CUpload_Action: hvData.Length() != m_hvBiasRecv.Length()");
							theGlobal.m_Log.Debug(strLog);
						}
					}
					catch (HException&)
					{
						CString strLog;
						strLog.Format("Upload_Action: ���㲹������ʧ��");
						theGlobal.m_Log.Exception(strLog);
					}
				}
				else
				{
					CString strLog;
					strLog.Format("CUpload_Action: ���ݳ���(%d)�����������㲹����Ŀ�곤��(%d)", m_vecRawData.Length(), nNum + nNum5m);
					theGlobal.m_Log.Debug(strLog);
				}
			}
			else
			{
				CString strLog;
				strLog.Format("CUpload_Action: ��ͷû������㲹�����̣��Ƿ�ʹ�ò���:%d���������ݳ���%d��ԭʼ���ݳ���%d",
					m_tParam.bUseCompensationAuto, m_hvBiasRecv.Length(), C.vecB[0].tMeasure.tSize.hv_WidthOrigin.Length());
				theGlobal.m_Log.Debug(strLog);
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
		//strTmp.Format("%d,", nImageSN);
		//strValue += strTmp;
		strTmp.Format("%d,", hvBiasValue[0].I());
		strValue += strTmp;
		strTmp.Format("%s,", hvBiasValue[1].S().Text());
		strValue += strTmp;
		for (int i = 2; i < hvBiasValue.Length(); i++)
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
	if (theGlobal.m_GrabberTool.GetGrabberType(0) == CGrabberTool::Simulate)
	{
		m_bAutoCompensationDisable = FALSE;
		return;
	}

	const double dCurrentMeterCount = theGlobal.m_Counter.GetCurrentMeterCount();
	static double s_dMeterCurrent = 0;
	if (s_dMeterCurrent > dCurrentMeterCount)
		s_dMeterCurrent = dCurrentMeterCount;
	if (theGlobal.m_Speed.GetSpeed() < 0.1 || theGlobal.m_Judge.m_tFirstArticleInspCount.bStart)
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
				theGlobal.m_Counter.GetCurrentMeterCount(), (s_dMeterCurrent + abs(m_tParam.dStartIgnoreMeter)));
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

	AddInfoToClient(m_hvProductInfo);

	theGlobal.m_Upload.m_bSendCheckProductInfo = FALSE;

	//for (int i = 0; i < 1; i++)
	//{
	//	try
	//	{
	//		CString strLog;
	//		if (theGlobal.m_Upload.m_bListenSuccess == FALSE)
	//		{
	//			strLog.Format("�빤λ%d���ӶϿ�������У���䷽:%sʧ��", i + 2, tPdInfo.strPDName);
	//			theGlobal.m_Log.Communication(strLog);
	//			theGlobal.m_Upload.m_bSendCheckProductInfo = FALSE;
	//			continue;
	//		}

	//		SendTuple(theGlobal.m_Upload.m_ClientSocket, m_hvProductInfo);

	//		strLog.Format("��λ%d����У���䷽:%s�ɹ�", i + 2, tPdInfo.strPDName);
	//		theGlobal.m_Log.Communication(strLog);
	//	}
	//	catch (HException&)
	//	{
	//		CString strLog;
	//		strLog.Format("�빤λ%d���ӶϿ�������У���䷽:%sʧ��", i + 2, tPdInfo.strPDName);
	//		theGlobal.m_Log.Communication(strLog);
	//		//theGlobal.m_Upload.m_bListenSuccess = FALSE;
	//		theGlobal.m_Upload.m_bConnectClient = FALSE;
	//		return FALSE;
	//	}
	//	theGlobal.m_Upload.m_bSendCheckProductInfo = FALSE;
	//}

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
			if (tPdInfo.tParam.vecAT11Width.size() < tPdInfo.nPDType * 2)
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
				tPdInfo.tParam.vecAT11Width.resize(tPdInfo.nPDType * 2, tTol);
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
					if (tPdInfo.nCoatingType == TwoSide)
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
		hvAlarm.Append(2);
	else
		hvAlarm.Append(1);

	AddInfoToServer(hvAlarm);

	//try
	//{
	//	SendTuple(theGlobal.m_Upload.m_Socket, hvAlarm);
	//	CString strLog;
	//	strLog.Format("���ͱ�����Ϣ����λ1�ɹ�");
	//	//theGlobal.m_Log.Communication(strLog);
	//}
	//catch (HException e)
	//{
	//	CString strLog;
	//	strLog.Format("��λ%d���Ͳ���ֵʧ��", theGlobal.m_tSystemSet.eSN);
	//	theGlobal.m_Log.Communication(strLog);
	//	m_bConnect = FALSE;
	//	return FALSE;
	//}

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
	strLog.Format("���չ�λ%d����(%s)��Ϣ�ɹ�������ʾ", nSN + 2, nAlarmOrStop == 2 ? "ͣ��" : "���ⱨ��");
	theGlobal.m_Log.Communication(strLog);
}