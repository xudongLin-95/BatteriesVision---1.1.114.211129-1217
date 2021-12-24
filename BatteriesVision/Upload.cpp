#include "StdAfx.h"
#include "Upload.h"

#define EPS 1e-12	//定义极限，用于判断两个double是否相等

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
void T_ServerSocket()//机头和二涂的时候调用
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
			strLog.Format("开启工位%d的监听:%s:%d", theGlobal.m_tSystemSet.eSN + 1, theGlobal.m_Upload.m_tParam.strAddressListen, theGlobal.m_Upload.m_tParam.nPortListen);
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
					//监听断开，重新打开监听
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
						//strLog.Format("已开启工位%d的监听:%s:%d",i+2,theGlobal.m_Upload.m_tParam.strAddressListen,theGlobal.m_Upload.m_tParam.nPortListen);
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
						strlog.Format("与工位%d网络建立成功.(IP:%s:%d)---(IP:%s:%d)", theGlobal.m_tSystemSet.eSN + 1, hvValue1[0].S().Text(), hvValue1[1].I(), hvValue1[2].S().Text(), hvValue1[3].I());
						theGlobal.m_Log.Communication(strlog);
					}
					catch (HException&e)
					{
						CString strMessage;
						strMessage.Format("%s(%s)", e.ErrorMessage().Text(), e.ProcName().Text());
						//theGlobal.m_Log.Std(strMessage);
						theGlobal.m_Upload.m_bConnectClient = FALSE;
						//连续多次发送失败，断开服务器监听，重新连接客户端
						/*if (++nDisconnectCount > 60)
						{
							theGlobal.m_Upload.m_bListenSuccess = FALSE;
							nDisconnectCount = 0;
						}*/
					}
				}
				else
				{
					//如果超过5秒没发任何消息，则补充心跳信号
					HTuple hvTimeStampTmp;
					CountSeconds(&hvTimeStampTmp);
					if ((hvTimeStampTmp.D() - hvTimeStamp.D()) > 5)
					{
						HTuple hvMsg = "HeartBeat";
						theGlobal.m_Upload.AddInfoToClient(hvMsg);
					}
					//发送信息
					while (theGlobal.m_Upload.m_hvecInfoSendToClient.Length() > 0)
					{
						if (theGlobal.m_Upload.m_bConnectClient == FALSE) break;

						HTuple hvInfoSend = theGlobal.m_Upload.m_hvecInfoSendToClient[0].T();
						{
							//删除头部元素
							CLockHelper lk(&theGlobal.m_Upload.m_csLock);
							theGlobal.m_Upload.m_hvecInfoSendToClient = theGlobal.m_Upload.m_hvecInfoSendToClient.Remove(0);
						}
						try
						{
							SendTuple(theGlobal.m_Upload.m_ClientSocket, hvInfoSend);

							CountSeconds(&hvTimeStamp);

							CString strLog;
							//strLog.Format("向工位%d发送信息:%s成功", theGlobal.m_tSystemSet.eSN + 1, hvInfoSend[0].S().Text());

							if ("ChangeBatch" == hvInfoSend[0])
							{
								strLog.Format("发送卷号(%s)到工位%d成功", hvInfoSend[1].S().Text(), theGlobal.m_tSystemSet.eSN + 1);
								theGlobal.m_Log.Communication(strLog);
							}
							else if ("CheckProductInfo" == hvInfoSend[0])
							{
								strLog.Format("发送配方信息(%s)到工位%d成功", hvInfoSend[1].S().Text(), theGlobal.m_tSystemSet.eSN + 1);
								theGlobal.m_Log.Communication(strLog);
							}
							else if ("HeartBeat" == hvInfoSend[0])
							{
								if (hvInfoSend.Length()>1)
								{
									strLog.Format("反馈心跳信号%d到工位%d成功", hvInfoSend[1].I(), theGlobal.m_tSystemSet.eSN + 1);
									theGlobal.m_Log.Communication(strLog);
								}
								else
								{
									strLog.Format("反馈心跳信号到工位%d成功", theGlobal.m_tSystemSet.eSN + 1);
									theGlobal.m_Log.Communication(strLog);
								}
							}
						}
						catch (HException&)
						{
							CString strLog;
							strLog.Format("与工位%d连接断开，发送信息:%s失败", theGlobal.m_tSystemSet.eSN + 1, hvInfoSend[0].S().Text());
							theGlobal.m_Log.Communication(strLog);
							theGlobal.m_Upload.m_bConnectClient = FALSE;
							theGlobal.ShowUploadDlg(strLog);

							//部分信息发送失败,需要补发
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
				strLog.Format("T_ServerSocket: 异常,网络连接断开%s (%s)", e.ErrorMessage().Text(), e.ProcName().Text());
				theGlobal.m_Log.Exception(strLog);
				theGlobal.m_Upload.m_bConnectClient = FALSE;
			}
		}
		Sleep(20);
	}

	//退出前清理句柄
	//for (int i = 0; i < 1; i++)
	{
		HALCON_TRY(CloseSocket(theGlobal.m_Upload.m_ClientSocket));
		HALCON_TRY(CloseSocket(theGlobal.m_Upload.m_ServerSocket));
	}

}
void T_ClientSocket()//非机头的时候调用
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
					strLog.Format("和工位%d网络建立成功:%s:%d连接成功", theGlobal.m_tSystemSet.eSN-1,theGlobal.m_Upload.m_tParam.strAddresConnect, theGlobal.m_Upload.m_tParam.nPortConnect);
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
				//如果超过5秒没发任何消息，则补充心跳信号
				HTuple hvTimeStampTmp;
				CountSeconds(&hvTimeStampTmp);

				//测试
				static int nHeartCount = 0;
				if ((hvTimeStampTmp.D() - hvTimeStamp.D()) > 5)
				{
					HTuple hvMsg = "HeartBeat";
					hvMsg.Append(nHeartCount);
					theGlobal.m_Upload.AddInfoToServer(hvMsg);
					nHeartCount++;
				}
				//发送信息
				while (theGlobal.m_Upload.m_hvecInfoSendToServer.Length() > 0)
				{
					if(FALSE == theGlobal.m_Upload.m_bConnect) break;

					HTuple hvInfoSend = theGlobal.m_Upload.m_hvecInfoSendToServer[0].T();
					{
						//删除头部元素
						CLockHelper lk(&theGlobal.m_Upload.m_csLock);
						theGlobal.m_Upload.m_hvecInfoSendToServer = theGlobal.m_Upload.m_hvecInfoSendToServer.Remove(0);
					}
					try
					{
						SendTuple(theGlobal.m_Upload.m_Socket, hvInfoSend);

						CountSeconds(&hvTimeStamp);

						CString strLog;
						//strLog.Format("向工位%d发送信息:%s成功", theGlobal.m_tSystemSet.eSN - 1, hvInfoSend[0].S().Text());
						
						if ("Bias" == hvInfoSend[0])
						{
							strLog.Format("发送补偿值到工位%d成功:,图像号%d,", theGlobal.m_tSystemSet.eSN - 1, hvInfoSend[5].I());
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
							strLog.Format("反馈换卷信号(%s)到工位%d成功", hvInfoSend[1].S().Text(), theGlobal.m_tSystemSet.eSN - 1);
							theGlobal.m_Log.Communication(strLog);
						}
						else if ("CheckProductInfo" == hvInfoSend[0])
						{
							strLog.Format("反馈配方同步信息(%s)到工位%d成功", hvInfoSend[1].S().Text(), theGlobal.m_tSystemSet.eSN - 1);
							theGlobal.m_Log.Communication(strLog);
						}
						else if ("HeartBeat" == hvInfoSend[0])
						{
							strLog.Format("发送心跳信号(%d)到工位%d成功", hvInfoSend[1].I(), theGlobal.m_tSystemSet.eSN - 1);
							theGlobal.m_Log.Communication(strLog);
						}
					}
					catch (HException&)
					{
						CString strLog;
						strLog.Format("与工位%d连接断开，发送信息:%s失败", theGlobal.m_tSystemSet.eSN - 1, hvInfoSend[0].S().Text());
						theGlobal.m_Log.Communication(strLog);
						theGlobal.ShowUploadDlg(strLog);
						theGlobal.m_Upload.m_bConnect = FALSE;

						//部分信息发送失败,需要补发
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
			strLog.Format("T_ClientSocket: 异常,网络连接断开%s (%s)", e.ErrorMessage().Text(), e.ProcName().Text());
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
void CUpload::SendBias(HTuple hvValue, int nImageSN)//机尾发送补偿值给涂2
{
	if (!m_tParam.bUseDataUpload/* || !m_tParam.bUseCompensationAuto*/) return;

	tagPDInfo tPdInfo = theGlobal.m_ProductTool.GetCurrentProductInfo();
	//0发送消息类型
	//1接收工位名
	//2产品类型
	//3是否存在AT11
	//4是否存在凹版
	//5图像号
	//6时间
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
		strLog.Format("发送补偿值到工位2成功:,图像号%d,", nImageSN);
		for (int i = 7; i < hvValue.Length() - 1; i++)
		{
			CString strTmp;
			strTmp.Format("%0.4f,", hvValue.D());
			strLog += strTmp;
		}
		theGlobal.m_Log.Communication(strLog);*/

		TupleInsert(hvValue, 0, "Bias", &hvValue);
		TupleInsert(hvValue, 1, HTuple(strStation), &hvValue);
		TupleInsert(hvValue, 2, tPdInfo.nPDType, &hvValue);//产品类型
		TupleInsert(hvValue, 3, tPdInfo.bExistAT11, &hvValue);//是否存在AT11
		TupleInsert(hvValue, 4, tPdInfo.bExistBaseCoat, &hvValue);//是否存在凹版
		TupleInsert(hvValue, 5, nImageSN, &hvValue);//图像号
		
		SYSTEMTIME tTime;
		::GetLocalTime(&tTime);
		strTime.Format("%02d:%02d:%02d.%03d", tTime.wHour, tTime.wMinute, tTime.wSecond, tTime.wMilliseconds);
		TupleInsert(hvValue, 6, HTuple(strTime), &hvValue);//时间

		AddInfoToServer(hvValue);
	}
	catch (HException e)
	{
		strLog.Format("工位%d发送补偿值失败", theGlobal.m_tSystemSet.eSN);
		theGlobal.m_Log.Communication(strLog);
		m_bConnect = FALSE;
	}
}

void CUpload::RecvBias(HTuple hvValue)
{
	CString strLog;
	strLog.Format("收到工位%d测量值(Bias):,图像号%d,", theGlobal.m_tSystemSet.eSN + 1, hvValue[0].I());
	for (int i = 2; i < hvValue.Length(); i++)
	{
		strLog.Format("%s,%.4f", strLog, hvValue[i].D());
	}
	theGlobal.m_Log.Communication(strLog);

	//实际是测量值，需要一涂二涂接收数据,0元素是图像号,1元素是时间
	TupleSelectRange(hvValue, 2, hvValue.Length() - 1, &m_hvBiasRecv);

	//用于后面判断接收数据是否与上次相同
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
			//判断接收到的数据是否未更新
			
			m_bBiasRecv = TRUE;
			CString strLog;
			strLog.Format("CUpload_RecvBias: 连续%d次收到上工位发来的测量值完全相同，不计算补偿",nBiasEqualCount);
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
			//strLog.Format("与工位%d连接断开，发送卷号:%s失败", i + 2, strBatch);
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
	strLog.Format("接收工位1发送的卷号:%s", strBatch);
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
			strLog.Format("CUpload_SendInfoToClient: 要发送给工位%d的消息超过10条", theGlobal.m_tSystemSet.eSN + 1);
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
			strLog.Format("CUpload_SendInfoToServer: 要发送给工位%d的消息超过10条", theGlobal.m_tSystemSet.eSN - 1);
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

	CreateMessageQueue(&m_hvQueue);//创建一个IO队列
	SetMessageQueueParam(m_hvQueue, "max_message_num", 1);
	//HDevThreadContext Context;
	//HDevThread* pThread = new HDevThread(Context,
	//	(void*)HThreadUpload, 1, 0);//开启一个具有1个输入的线程;
	//pThread->SetInputCtrlParamTuple(0, m_hvQueue);
	//pThread->Start();

	if (CGlobal::SN_Head == theGlobal.m_tSystemSet.eSN)
	{
		//机头作为服务器，和工位2通信
		AfxBeginThread((AFX_THREADPROC)T_ServerSocket, NULL);
		AfxBeginThread((AFX_THREADPROC)T_ServerRecvData, NULL);
	}
	else if (CGlobal::SN_2nd == theGlobal.m_tSystemSet.eSN)
	{
		if (CGlobal::DT_DoubleLayer == theGlobal.m_tSystemSet.eDevType)
		{
			//双层，工位2作为服务器，和工位3通信
			AfxBeginThread((AFX_THREADPROC)T_ServerSocket, NULL);
			AfxBeginThread((AFX_THREADPROC)T_ServerRecvData, NULL);
		}
		//双层和单层，工位2都作为客户端
		AfxBeginThread((AFX_THREADPROC)T_ClientSocket, NULL);
		AfxBeginThread((AFX_THREADPROC)T_ClientRecvData, NULL);
	}
	else
	{
		//机尾和其他工位都作为客户端,连接上一个工位
		AfxBeginThread((AFX_THREADPROC)T_ClientSocket, NULL);
		AfxBeginThread((AFX_THREADPROC)T_ClientRecvData, NULL);
	}

	//开机打开卷号
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
					//判定工位
					static HTuple s_hvCount = HTuple().TupleGenConst(2, 0);
					if (theGlobal.m_Upload.m_bSendCheckProductInfo == FALSE)
					{
						s_hvCount[0] += 1;
						if (s_hvCount[0] > 6000)//2分钟没有收到配方校验反馈，不再接收
						{
							s_hvCount[0] = 0;
							theGlobal.m_Upload.m_bSendCheckProductInfo = TRUE;
							CString strInfo;
							strInfo.Format("工位%d的配方校验失败，未收到工位%d的配方校验反馈",
								theGlobal.m_tSystemSet.eSN + 1, theGlobal.m_tSystemSet.eSN + 1);
							theGlobal.m_Log.Alarm(strInfo);
						}
					}
					else
					{
						s_hvCount[0] = 0;
					}
					//发送卷号反馈
					if (theGlobal.m_Upload.m_bSendBatchOK == FALSE)
					{
						s_hvCount[1] += 1;
						if (s_hvCount[1] > 3000)//2分钟没有收到配方校验反馈，不再接收
						{
							s_hvCount[1] = 0;
							theGlobal.m_Upload.m_bSendBatchOK = TRUE;
							CString strInfo;
							strInfo.Format("工位%d换卷失败,1分钟未收到工位%d的换卷反馈,进行补发",
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

				//判断60s没有接收到客户端的任何消息，断开重连
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
						strMessage.Format("2分钟未收到工位%d的信息，已断开网络", theGlobal.m_tSystemSet.eSN + 1);
						theGlobal.m_Log.Communication(strMessage);
						theGlobal.m_Log.Alarm(strMessage);
					}*/

					//客户端接收到信息
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
						strMessage.Format("3分钟未连接上工位%d，请检查", theGlobal.m_tSystemSet.eSN + 1);
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
				CountSeconds(&hvTimeStampe);//接收到消息就更新时间
				try
				{
					HTuple hvData;
					ReceiveTuple(theGlobal.m_Upload.m_ClientSocket, &hvData);
					if (hvData.Length() > 1 && "Bias" == hvData[0])
					{
						tagPDInfo tPdInfo = theGlobal.m_ProductTool.GetCurrentProductInfo();
						//0发送消息类型
						//1工位名
						//2产品类型
						//3是否存在AT11
						//4是否存在凹版
						//5图像号
						//6时间
						if (tPdInfo.nPDType == hvData[2].I() && tPdInfo.bExistAT11 == hvData[3].I() && tPdInfo.bExistBaseCoat == hvData[4].I())
						{
							HTuple hvRev;
							TupleSelectRange(hvData, 5, hvData.Length() - 1, &hvRev);
							theGlobal.m_Upload.RecvBias(hvRev);

							CString strMessage;
							strMessage.Format("接收补偿数据成功");
							//theGlobal.m_Log.Communication(strMessage);
							theGlobal.m_Upload.WriteBiasFile(hvRev, hvData[5].I());
						}
						else
						{
							CString strMessage;
							strMessage.Format("接收的补偿数据与当前工位产品类型不符: 后工位的产品类型:一出%d, 是否存在AT11:%s, 是否存在凹版:%s",
								hvData[2].I() * 2, hvData[3].I() == TRUE ? "是" : "否", hvData[4].I() == TRUE ? "是" : "否");
							theGlobal.m_Log.Exception(strMessage);
						}
					}
					else if ("HeartBeat" == hvData[0])
					{
						CString strlog;
						strlog.Format("接收工位%d发送来的心跳信号%d成功", theGlobal.m_tSystemSet.eSN + 1,hvData[1].I());
						theGlobal.m_Log.Communication(strlog);
						//反馈心跳
						theGlobal.m_Upload.AddInfoToClient(hvData);
					}
					else if (hvData.Length() > 1 && "ChangeBatch" == hvData[0] && 
						theGlobal.GetCurrentBatch() == hvData[1].S().Text())
					{
						theGlobal.m_Upload.m_bSendBatchOK = TRUE;

						CString strlog;
						strlog.Format("接收工位%d换卷(%s)完成反馈成功", theGlobal.m_tSystemSet.eSN + 1, hvData[1].S().Text());
						theGlobal.m_Log.Communication(strlog);
					}
					else if (hvData.Length() > 1 && "CheckProductInfo" == hvData[0])
					{
						CString strInfo;
						if (hvData[1] == "OK")
						{
							//校验成功
							strInfo.Format("工位%d的配方校验完成", theGlobal.m_tSystemSet.eSN + 1);
						}
						else
						{
							//校验失败
							strInfo.Format("工位%d的配方校验完成,配方信息不一致,工位%d配方信息已更正", 
								theGlobal.m_tSystemSet.eSN + 1, theGlobal.m_tSystemSet.eSN + 1);
							theGlobal.SetAlarmInfo(strInfo);
						}
						theGlobal.m_Log.Communication(strInfo);
						theGlobal.m_Upload.m_bSendCheckProductInfo = TRUE;
					}
					else if ("AlarmInfo" == hvData[0])
					{
						//处理其他工位的报警信息
						theGlobal.m_Upload.RecvAlarmInfoFromClient(hvData, theGlobal.m_tSystemSet.eSN-1);
						//反馈消息
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
			//判断30s没有接收到服务器的握手消息，断开重连
			HTuple hvTimeTmp;
			CountSeconds(&hvTimeTmp);
			if (theGlobal.m_Upload.m_bConnect == TRUE)
			{
				if ((hvTimeTmp.D() - hvTimeStampe.D() > 60) && bConnectAlarm)
				{
					CountSeconds(&hvTimeStampe);
					theGlobal.m_Upload.m_bConnect = FALSE;
					CString strMessage;
					strMessage.Format("1分钟未接收到工位%d心跳，已断开网络", theGlobal.m_tSystemSet.eSN - 1);
					theGlobal.m_Log.Communication(strMessage);
					//theGlobal.m_Log.Alarm(strMessage);
					theGlobal.ShowUploadDlg(strMessage);

					bConnectAlarm = FALSE;
				}

				//客户端接收到信息
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

					//反馈消息
					HTuple hvMsg;
					hvMsg.Append("ChangeBatch");
					hvMsg.Append((HTuple)strBatch);
					theGlobal.m_Upload.AddInfoToServer(hvMsg);

					if (theGlobal.m_tSystemSet.eSN == CGlobal::SN_2nd &&
						theGlobal.m_tSystemSet.eDevType == CGlobal::DT_DoubleLayer)
					{
						//双层涂布工位2要转发消息给机尾
						theGlobal.m_Upload.AddInfoToClient(hvData);
					}
				}
				else if ("HeartBeat" == hvData[0])
				{
					//HTuple hvMsg = "HeartBeat";
					//theGlobal.m_Upload.AddInfoToServer(hvMsg);
					//心跳反馈不处理
					CString strLog;
					strLog.Format("收到工位%d反馈回来的心跳信号(%d)成功", theGlobal.m_tSystemSet.eSN - 1, hvData[1].I());
					theGlobal.m_Log.Communication(strLog);
				}
				else if ("CheckProductInfo" == hvData[0])
				{
					theGlobal.m_Upload.m_hvProductInfo = hvData;

					CString strInfo;
					strInfo.Format("");
					//反馈消息
					HTuple hvMsg = "CheckProductInfo";
					if (!theGlobal.m_Upload.CheckProductInfoFromServer())
					{
						//校验失败
						strInfo = "收到机头配方信息，配方信息不一致，已经更正";
						hvMsg.Append((HTuple)"NG");
					}
					else
					{
						//校验成功
						strInfo = "收到机头配方信息，配方信息一致，配方校验完成";
						hvMsg.Append((HTuple)"OK");
					}
					theGlobal.m_Log.Communication(strInfo);
					//反馈消息
					theGlobal.m_Upload.AddInfoToServer(hvMsg);
					
					if (theGlobal.m_tSystemSet.eSN == CGlobal::SN_2nd &&
						theGlobal.m_tSystemSet.eDevType == CGlobal::DT_DoubleLayer)
					{
						//双层涂布工位2要转发消息给机尾
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

		//二涂和机头需要收集数据
		if (theGlobal.m_tSystemSet.eSN == CGlobal::SN_Head || theGlobal.m_tSystemSet.eSN == CGlobal::SN_2nd)
		{
			m_vecRawData[m_vecRawData.Length()].T() = C.vecB[0].tMeasure.tSize.hv_WidthOrigin;//原始数据

			double dMeterCount = m_vecRawData.Length()*hvHeightM.D();
			if (dMeterCount > (m_tParam.dToNextStationMeter + m_tParam.dCompensationInvertal))
				m_vecRawData = m_vecRawData.Remove(0);
		}

		//if (!m_tParam.bUseDataUpload/* || !m_tParam.bUseCompensationAuto*/) return;
		if (!IsNetConnect())
		{
			//通讯状态判断
			CString strLog;
			strLog.Format("CUpload_Action: 通讯连接断开，不计算补偿");
			theGlobal.m_Log.Std(strLog);
			return;
		}

		const double dCurrentMeterCount = theGlobal.m_Counter.GetCurrentMeterCount();
		static double s_dMeterCount = dCurrentMeterCount;

		//防止记米清零，无法发送值
		if (s_dMeterCount > dCurrentMeterCount)
			s_dMeterCount = dCurrentMeterCount;

		if (theGlobal.m_tSystemSet.eSN == CGlobal::SN_2nd)
		{
			//工位2先接收机尾数据，计算补偿值
			if (m_tParam.bUseCompensationAuto && m_hvBiasRecv.Length() == C.vecB[0].tMeasure.tSize.hv_WidthOrigin.Length())
			{
				//取延迟到机尾的数据里面的头五米，计算一个中值，和接收的数据相减，得到在线补偿值
				try
				{
					//if (theGlobal.m_Upload.m_bAutoCompensationDisable)
					//{
					//	if (m_hvecCompen.Length() == 0)return;
					//	//用之前的补偿值进行计算
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
					//	CString strLog = "Upload_Action: 补偿禁用开启,选取的补偿值为:,";
					//	for (int i = 0; i < m_hvCompensation.Length(); i++)
					//	{
					//		CString strVal;
					//		strVal.Format("%0.4f,", m_hvCompensation.D());
					//		strLog += strVal;
					//	}
					//	/*strLog += "收到下工位的测量数据为:,";
					//	for (int i = 0; i < m_hvBiasRecv.Length(); i++)
					//	{
					//		CString strVal;
					//		strVal.Format("%0.4f,", m_hvBiasRecv.D());
					//		strLog += strVal;
					//	}
					//	strLog += "当前对应测量值为:,";
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
								//无效值判断
								CString strLog;
								strLog.Format("CUpload_Action: 无效值过多，不计算补偿");
								theGlobal.m_Log.Std(strLog);
								return;
							}
							else if (hvTmp2.Length() > 5 && hvTmp2.TupleDeviation().D() > 0.8)
							{
								//计算标准差
								CString strLog;
								strLog.Format("CUpload_Action: 收集标准差%0.2f，大于0.8，不计算补偿", hvTmp2.TupleDeviation().D());
								theGlobal.m_Log.Std(strLog);
								return;
							}
						}
						if (hvData.Length() == m_hvBiasRecv.Length())
						{
							HTuple hv_FindResult0 = m_hvBiasRecv.TupleFind(0);
							HTuple hv_FindResultNoData = m_hvBiasRecv.TupleFind(NO_DATA);
							if (hv_FindResult0 != -1 || hv_FindResultNoData != -1) return;
							//判断接收到的数据是否每次都不同（防呆）
							if (m_bBiasRecv)
							{
								CString strLog, strAllData, strBiasRecvData = "";
								for (int i = 0; i < m_hvBiasRecv.Length(); i++)
								{
									strBiasRecvData.Format("%0.2f,", m_hvBiasRecv[i].D());
									strAllData += strBiasRecvData;
								}
								strLog.Format("CUpload_Action: 接收数据完全相同%s，不计算补偿", strAllData);
								theGlobal.m_Log.Std(strLog);
								return;
							}
							TupleSub(m_hvBiasRecv, hvData, &m_hvCompensation);
							TupleMult(m_hvCompensation, m_tParam.dScale, &m_hvCompensation);
							//需要将补偿值更新给在线补偿参数
							theGlobal.m_ProductTool.UpdateOnlineOffset(m_hvCompensation);
							//正常的时候记录最近的补偿值
							/*m_hvecCompen[m_hvecCompen.Length()].T() = m_hvCompensation;
							if (m_hvecCompen.Length() > 3) m_hvecCompen = m_hvecCompen.Remove(0);*/
							CString strLog = "Upload_Action: 计算的补偿值为:,";
							for (int i = 0; i < m_hvCompensation.Length(); i++)
							{
								CString strVal;
								strVal.Format("%0.4f,", m_hvCompensation[i].D());
								strLog += strVal;
							}
							strLog += "收到下工位的测量数据为:,";
							for (int i = 0; i < m_hvBiasRecv.Length(); i++)
							{
								CString strVal;
								strVal.Format("%0.4f,", m_hvBiasRecv[i].D());
								strLog += strVal;
							}
							strLog += "当前对应测量值为:,";
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
					strLog.Format("Upload_Action: 计算补偿值失败");
					theGlobal.m_Log.Exception(strLog);
				}
			}

			//双层，二涂发数据给机头，发送下面相机的测量数据,数据左右倒过来发给机头
			//收集数据
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

			//5米发一次数据
			if (dCurrentMeterCount - s_dMeterCount > m_tParam.dCompensationInvertal && m_vecDataCollect.Length() > 0)
			{
				try
				{
					s_dMeterCount = dCurrentMeterCount;

					//每5米数据求中值，发送
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
					strLog.Format("Upload_Action: 发送补偿数据失败");
					theGlobal.m_Log.Exception(strLog);
				}
			}
		}
		else if (theGlobal.m_tSystemSet.eSN == CGlobal::SN_3rd)
		{
			//机尾发数据给二涂，发送上面相机数据即可，不用左右颠倒数据
			m_vecDataCollect[m_vecDataCollect.Length()].T() = C.vecB[0].tMeasure.tSize.hv_Width;

			const double dCurrentMeterCount = theGlobal.m_Counter.GetCurrentMeterCount();

			//5米发一次数据
			if (dCurrentMeterCount - s_dMeterCount > m_tParam.dCompensationInvertal
				&& m_vecDataCollect.Length() > 0)
			{
				s_dMeterCount = dCurrentMeterCount;

				try
				{
					//每5米数据求中值，发送
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
					//2021.7.14临时测试，不发送补偿数据
					if (hvData.Length() == m_vecDataCollect[m_vecDataCollect.Length() - 1].T().Length())
						SendBias(hvData, C.vecB[0].vecA[0].hvImageSN.I());
					m_vecDataCollect.Clear();
				}
				catch (HException&)
				{
					CString strLog;
					strLog.Format("Upload_Action: 发送补偿数据失败");
					theGlobal.m_Log.Exception(strLog);
				}
			}
		}
		else if(theGlobal.m_tSystemSet.eSN == CGlobal::SN_Head)
		{
			//机头只接收数据处理
			if (m_tParam.bUseCompensationAuto && m_hvBiasRecv.Length() == C.vecB[0].tMeasure.tSize.hv_WidthOrigin.Length())
			{
				//取延迟到二涂的数据里面的头五米，计算一个中值，和接收的数据相减，得到在线补偿值
 			//	if (theGlobal.m_Upload.m_bAutoCompensationDisable)
				//{
				//	if (m_hvecCompen.Length() == 0)return;
				//	//用之前的补偿值进行计算
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
				//	CString strLog = "Upload_Action: 补偿禁用开启,选取的补偿值为:,";
				//	for (int i = 0; i < m_hvCompensation.Length(); i++)
				//	{
				//		CString strVal;
				//		strVal.Format("%0.4f,", m_hvCompensation.D());
				//		strLog += strVal;
				//	}
				//	/*strLog += "收到下工位的测量数据为:,";
				//	for (int i = 0; i < m_hvBiasRecv.Length(); i++)
				//	{
				//		CString strVal;
				//		strVal.Format("%0.4f,", m_hvBiasRecv.D());
				//		strLog += strVal;
				//	}
				//	strLog += "当前对应测量值为:,";
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
					strLog.Format("CUpload_Action: 开始计算干湿膜补偿");
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
								//无效值判断
								CString strLog;
								strLog.Format("CUpload_Action: 无效值过多，不计算补偿");
								theGlobal.m_Log.Std(strLog);
								return;
							}
							else if (hvTmp2.Length() > 5 && hvTmp2.TupleDeviation().D() > 0.8)
							{
								//计算标准差
								CString strLog;
								strLog.Format("CUpload_Action: 收集标准差%0.2f，大于0.8，不计算补偿", hvTmp2.TupleDeviation().D());
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
								strLog.Format("CUpload_Action: m_hvBiasRecv含无效值");
								theGlobal.m_Log.Debug(strLog);
								return;
							}
							//判断接收到的数据是否每次都不同（防呆）
							//判断接收到的数据是否每次都不同（防呆）
							if (m_bBiasRecv)
							{
								CString strLog, strAllData, strBiasRecvData = "";
								for (int i = 0; i < m_hvBiasRecv.Length(); i++)
								{
									strBiasRecvData.Format("%0.2f,", m_hvBiasRecv[i].D());
									strAllData += strBiasRecvData;
								}
								strLog.Format("CUpload_Action: 接收数据完全相同%s，不计算补偿",  strAllData);
								theGlobal.m_Log.Std(strLog);
								return;
							}

							TupleSub(m_hvBiasRecv, hvData, &m_hvCompensation);
							TupleMult(m_hvCompensation, m_tParam.dScale, &m_hvCompensation);
							//需要将补偿值更新给在线补偿参数
							theGlobal.m_ProductTool.UpdateOnlineOffset(m_hvCompensation);
							//正常的时候记录最近的补偿值
							/*m_hvecCompen[m_hvecCompen.Length()].T() = m_hvCompensation;
							if (m_hvecCompen.Length() > 3) m_hvecCompen = m_hvecCompen.Remove(0);*/
							CString strLog = "Upload_Action: 计算的补偿值为:,";
							for (int i = 0; i < m_hvCompensation.Length(); i++)
							{
								CString strVal;
								strVal.Format("%0.4f,", m_hvCompensation[i].D());
								strLog += strVal;
							}
							strLog += "收到下工位的测量数据为:,";
							for (int i = 0; i < m_hvBiasRecv.Length(); i++)
							{
								CString strVal;
								strVal.Format("%0.4f,", m_hvBiasRecv[i].D());
								strLog += strVal;
							}
							strLog += "当前对应测量值为:,";
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
						strLog.Format("Upload_Action: 计算补偿数据失败");
						theGlobal.m_Log.Exception(strLog);
					}
				}
				else
				{
					CString strLog;
					strLog.Format("CUpload_Action: 数据长度(%d)不够，不计算补偿，目标长度(%d)", m_vecRawData.Length(), nNum + nNum5m);
					theGlobal.m_Log.Debug(strLog);
				}
			}
			else
			{
				CString strLog;
				strLog.Format("CUpload_Action: 机头没进入计算补偿流程，是否使用补偿:%d，接收数据长度%d，原始数据长度%d",
					m_tParam.bUseCompensationAuto, m_hvBiasRecv.Length(), C.vecB[0].tMeasure.tSize.hv_WidthOrigin.Length());
				theGlobal.m_Log.Debug(strLog);
			}
		}
		
	}
	catch (HException&e)
	{
		CString strException;
		strException.Format("Upload_Action: 异常%s(%s)", e.ErrorMessage().Text(), e.ProcName().Text());
		theGlobal.m_Log.Exception(strException);
	}
	catch (...)
	{
		CString strException;
		strException.Format("Upload_Action: 出现异常，请检查");
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
		strFliePath.Format("%s//干湿膜自动补偿数据//", theGlobal.m_Storage.GetMapFolderPath());
		if (!PathFileExists(strFliePath)) CreateMultilevelPath(strFliePath);
		strCsvFile.Format("%s//工位%d接收工位%d补偿数据_%04d%02d%02d.csv", strFliePath, theGlobal.m_tSystemSet.eSN, theGlobal.m_tSystemSet.eSN + 1, tTm.wYear, tTm.wMonth, tTm.wDay);
		if (!PathFileExists(strCsvFile))
		{
			CString strName, strTmp;
			strName.Format("时间,后工位时间,后工位图像号,");
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

		//写数据
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
			strException.Format("CUpload_WriteBiasFile: 接收补偿数据文件被打开,无法写入数据");
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
	//判断是否停机
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
		strLog.Format("CUpload_ListenStartRunIgnoreCompen: 检测到设备停机，设置开机忽略米数%0.2fm", m_tParam.dStartIgnoreMeter);
		theGlobal.m_Log.Std(strLog);*/
	}
	else
	{
		if (dCurrentMeterCount < (s_dMeterCurrent + abs(m_tParam.dStartIgnoreMeter)))
		{
			m_bAutoCompensationDisable = TRUE;
			/*CString strLog;
			strLog.Format("CUpload_RunIgnoreCompen: 干湿膜补偿禁用状态:%s,记米:%0.2fm,目标米数:%0.2fm", m_bAutoCompensationDisable ? "开启" : "关闭",
				theGlobal.m_Counter.GetCurrentMeterCount(), (s_dMeterCurrent + abs(m_tParam.dStartIgnoreMeter)));
			theGlobal.m_Log.Std(strLog);*/
		}
		else
		{
			m_bAutoCompensationDisable = FALSE;
			/*CString strLog;
			strLog.Format("CUpload_ListenStartRunIgnoreCompen: 干湿膜补偿禁用状态:%s, 记米:%0.2fm", m_bAutoCompensationDisable ? "开启" : "关闭",
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

	//建立配方信息
	m_hvProductInfo.Clear();
	//0校验指令
	//1产品名
	//2产品类型
	//3是否存在AT11
	//4是否存在凹版
	//5涂布类型，单双面
	//涂布数量，涂布公差
	//极耳数量，极耳公差
	//凹版/at9规格，凹版/at9公差
	//错位规格，错位公差
	m_hvProductInfo.Append("CheckProductInfo");
	m_hvProductInfo.Append((HTuple)tPdInfo.strPDName);
	m_hvProductInfo.Append(tPdInfo.nPDType);
	m_hvProductInfo.Append(tPdInfo.bExistAT11);
	m_hvProductInfo.Append(tPdInfo.bExistBaseCoat);
	m_hvProductInfo.Append(tPdInfo.nCoatingType);
	//涂布
	m_hvProductInfo.Append(tPdInfo.tParam.vecCoatWidth[0].dStandard);
	m_hvProductInfo.Append(tPdInfo.tParam.vecCoatWidth[0].dWarnTolerance);
	m_hvProductInfo.Append(tPdInfo.tParam.vecCoatWidth[0].dTolerance);
	//极耳
	for (int i = 0; i < tPdInfo.nPDType + 1; i++)
	{
		m_hvProductInfo.Append(tPdInfo.tParam.vecFoilWidth[i].dStandard);
		m_hvProductInfo.Append(tPdInfo.tParam.vecFoilWidth[i].dWarnTolerance);
		m_hvProductInfo.Append(tPdInfo.tParam.vecFoilWidth[i].dTolerance);
	}
	//AT11和凹版
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
	//			strLog.Format("与工位%d连接断开，发送校验配方:%s失败", i + 2, tPdInfo.strPDName);
	//			theGlobal.m_Log.Communication(strLog);
	//			theGlobal.m_Upload.m_bSendCheckProductInfo = FALSE;
	//			continue;
	//		}

	//		SendTuple(theGlobal.m_Upload.m_ClientSocket, m_hvProductInfo);

	//		strLog.Format("向工位%d发送校验配方:%s成功", i + 2, tPdInfo.strPDName);
	//		theGlobal.m_Log.Communication(strLog);
	//	}
	//	catch (HException&)
	//	{
	//		CString strLog;
	//		strLog.Format("与工位%d连接断开，发送校验配方:%s失败", i + 2, tPdInfo.strPDName);
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

	//0校验指令
	//1产品名
	//2产品类型
	//3是否存在AT11
	//4是否存在凹版
	//5涂布类型，单双面
	//涂布规格，涂布公差
	//极耳数量，极耳公差
	//凹版/at9规格，凹版/at9公差
	//错位规格，错位公差
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
			//双面的相机配置，需要校验和机头的单双面设置是否一样
			if (m_hvProductInfo[nCount] != tPdInfo.nCoatingType) bRet = FALSE;
		}
		nCount++;
		//涂布规格，预警公差，NG公差，
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
		//极耳规格，预警公差，NG公差，
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
			//修改规格数量
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
			//修改规格数量
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

		//修改当前配方
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
			//如果参数范围改变，也重新显示下折线图
			theGlobal.m_pSubDispScope->PostMessageA(WM_REFRESH_DISP);
		}
	}
	catch (HException&e)
	{
		CString strLog;
		strLog.Format("校验配方信息:%s失败", m_hvProductInfo[1].S().Text());
		theGlobal.m_Log.Communication(strLog);
		return FALSE;
	}
	return bRet;
}

BOOL CUpload::SendAlarmInfoToServer(const CErrorInfo::tagErrorInfo& tInfo, const BOOL& bStopMachine)
{
	if (theGlobal.m_tSystemSet.eSN == CGlobal::SN_Head) return TRUE;

	//报警信息
	HTuple hvAlarm;
	//0 命令头
	//1 工位编号
	//2 错误码
	//3 错误信息描述
	//4 解决方案
	//5 0无，1设备声光报警，2设备停机

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
	//	strLog.Format("发送报警信息到工位1成功");
	//	//theGlobal.m_Log.Communication(strLog);
	//}
	//catch (HException e)
	//{
	//	CString strLog;
	//	strLog.Format("工位%d发送补偿值失败", theGlobal.m_tSystemSet.eSN);
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
		strLog.Format("接收工位%d报警信息失败，信息不全", nSN + 2);
		theGlobal.m_Log.Communication(strLog);
		return;
	}
	//报警信息
	HTuple hvAlarm = hvAlarmInfo;
	//0 命令头
	//1 工位编号
	//2 错误码
	//3 错误信息描述
	//4 解决方案
	//5 0无，1报警，2停机

	CErrorInfo::tagErrorInfo tInfo;
	int nStationSN = hvAlarmInfo[1];
	tInfo.nID = hvAlarmInfo[2];
	tInfo.strDescribe.Format("[工位%d] %s", nStationSN, hvAlarmInfo[3].S().Text());
	tInfo.strSolution.Format("%s", hvAlarmInfo[4].S().Text());
	int nAlarmOrStop = hvAlarmInfo[5];

	theGlobal.SetAlarmInfo(tInfo);
	if (nAlarmOrStop == 1)
		theGlobal.m_CommunicationTool.CCDAlarm();
	else if (nAlarmOrStop == 2)
		theGlobal.m_CommunicationTool.CCDStop();

	CString strLog;
	strLog.Format("接收工位%d报警(%s)信息成功，已显示", nSN + 2, nAlarmOrStop == 2 ? "停机" : "声光报警");
	theGlobal.m_Log.Communication(strLog);
}