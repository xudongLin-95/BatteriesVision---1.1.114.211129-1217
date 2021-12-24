#include "StdAfx.h"
#include "IOCommunicate.h"
#include "DlgIOSet.h"
extern "C" __declspec(dllexport) DHInterface* Export(void)
{
	return (DHInterface*)new CIOCommunicate();
}
int  WM_COUNT = RegisterWindowMessage(_T("WM_COUNT"));
CPIO24BLib CIOCommunicate::m_IOCard[PIO24_COUNT];
CIOCommunicate* g_This = NULL;

CIOCommunicate::CIOCommunicate(void)
{
	m_strProductName = "";
	m_strConfigCommonFile = "";
	m_strConfigCommunicateFile = "";
	m_strConfigSystemFile = "";
	m_strConfigCalibrationFile = "";
	m_nMarkerLength = 0;
	m_hvLabelPointRow[0].Clear();
	m_hvLabelPointRow[1].Clear();
	m_hvDecreaseFrames[0].Clear();
	m_hvDecreaseFrames[1].Clear();
	m_hvResultFlag[0].Clear();
	m_hvResultFlag[1].Clear();
	m_bExit = FALSE;
	m_nCamRelation = 0;
	m_nIn[0] = 0;
	m_nIn[1] = 0;
	m_bIn[0] = FALSE;
	m_bIn[1] = FALSE;
	for (int i = 0; i < TB_CHANNEL; i++)
	{
		m_bLXTB[i] = FALSE;
	}
}


CIOCommunicate::~CIOCommunicate(void)
{
	m_bExit = TRUE;
	SetMessageQueueParam(m_hvQueue, "flush_queue", 1);
	for (int i=0;i<m_vecThreadHandle.size();i++)
	{
		if (NULL!=m_vecThreadHandle[i])
		{
			HALCON_TRY(m_vecThreadHandle[i]->Exit())
				delete m_vecThreadHandle[i];
			m_vecThreadHandle[i] = NULL;
		}
	}
}

BOOL CIOCommunicate::GetDeviceList(LPGUID lpGuid,BOOL bsFlag)
{
	BOOL bFlag = TRUE;

	do 
	{
		HDEVINFO hDevInfo;
		SP_DEVINFO_DATA DeviceInfoData;     
		DWORD i;   
		// 得到所有设备 HDEVINFO      
		hDevInfo = SetupDiGetClassDevs(lpGuid, 0, 0, DIGCF_PRESENT/* | DIGCF_ALLCLASSES */);       
		if (hDevInfo == INVALID_HANDLE_VALUE)     
		{
			bFlag = FALSE;
			break;
		}
		DeviceInfo theItem;
		// 循环列举     
		DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);   

		for (i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &DeviceInfoData); i++)     
		{     
			TCHAR szClassBuf[MAX_PATH] = { 0 };
			TCHAR szDescBuf[MAX_PATH] = { 0 };
			TCHAR szDriver[MAX_PATH] = { 0 };
			TCHAR szFriName[MAX_PATH] = { 0 };

			// 获取类名  
			if (!SetupDiGetDeviceRegistryProperty(hDevInfo, &DeviceInfoData, SPDRP_CLASS, NULL, (PBYTE)szClassBuf, MAX_PATH - 1, NULL))         
			{
				///*continue*/;

			}

			theItem.szDeviceClass = szClassBuf;

			//获取设备描述信息
			if (!SetupDiGetDeviceRegistryProperty(hDevInfo, &DeviceInfoData, SPDRP_DEVICEDESC, NULL, (PBYTE)szDescBuf, MAX_PATH - 1, NULL))         
			{
				///*continue*/;

			}

			theItem.szDeviceDesc = szDescBuf;

			//获取设备驱动名
			if (!SetupDiGetDeviceRegistryProperty(hDevInfo, &DeviceInfoData, SPDRP_DRIVER, NULL, (PBYTE)szDriver, MAX_PATH - 1, NULL))         
			{
				///*continue*/;

			}

			theItem.szDriverName = szDriver;

			//获取设备友好名
			if (!SetupDiGetDeviceRegistryProperty(hDevInfo, &DeviceInfoData, SPDRP_FRIENDLYNAME, NULL, (PBYTE)szFriName, MAX_PATH - 1, NULL))         
			{
				///*continue*/;

			}
			//ERROR_INSUFFICIENT_BUFFER

			theItem.szDeviceName = szFriName;
			theItem.dwDevIns = DeviceInfoData.DevInst;
			theItem.Guid = DeviceInfoData.ClassGuid;

		}     

		//  释放     
		SetupDiDestroyDeviceInfoList(hDevInfo); 
		SetDeviceStatus(theItem,bsFlag);



	} while (FALSE);

	return bFlag;
}
BOOL CIOCommunicate::SetDeviceStatus(DeviceInfo &theDevice,BOOL bStatusFlag)
{
	BOOL bFlag = TRUE;
	do 
	{
		SP_DEVINFO_DATA DeviceInfoData;
		DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA); 
		HDEVINFO hDevInfo;
		// 得到设备 HDEVINFO      
		hDevInfo = SetupDiGetClassDevs(&theDevice.Guid, 0, 0, DIGCF_PRESENT /*| DIGCF_ALLCLASSES */);     
		if (hDevInfo == INVALID_HANDLE_VALUE)    
		{
			bFlag = FALSE;
			break;
		}

		//判断是否有这个设备
		bFlag = FALSE;
		int index = 0;
		while (SetupDiEnumDeviceInfo(hDevInfo, index ++, &DeviceInfoData))
		{
			if (DeviceInfoData.DevInst == theDevice.dwDevIns)
			{
				bFlag = TRUE;
				break;
			}
		}
		if (!bFlag)
		{

		}
		else
		{
			//初始化属性
			SP_PROPCHANGE_PARAMS propChange;
			propChange.ClassInstallHeader.cbSize = sizeof(SP_CLASSINSTALL_HEADER);
			propChange.ClassInstallHeader.InstallFunction = DIF_PROPERTYCHANGE;
			propChange.Scope = DICS_FLAG_GLOBAL;
			/*propChange.StateChange = bStatusFlag ? DICS_START: DICS_STOP;*/
			if(1==bStatusFlag)
			{
				propChange.StateChange = DICS_ENABLE;//启用：DICS_ENABLE  不启用：DICS_DISABLE
			}else
			{
				propChange.StateChange = DICS_DISABLE;//启用：DICS_ENABLE  不启用：DICS_DISABLE
			}



			propChange.HwProfile = 0;

			if (SetupDiSetClassInstallParams(hDevInfo, &DeviceInfoData, (SP_CLASSINSTALL_HEADER*)&propChange, sizeof(propChange)))
			{
				if (!SetupDiChangeState(hDevInfo, &DeviceInfoData))
				{

					bFlag = FALSE;
				}
			}
			else
			{

				bFlag = FALSE;
			}
		}

		//  释放     
		SetupDiDestroyDeviceInfoList(hDevInfo); 


	} while (FALSE);

	return TRUE;
}

void CIOCommunicate::IO_thread(HTuple hvQueue)
{
	HTuple hv_MessageRecv,hvMessageNum,hv_StopProcInfo,hv_TimeStart,hv_TimeEnd,hv_ErrorFlag;
	while(!m_bExit)
	{		
		DequeueMessage(hvQueue, "timeout", "infinite", &hv_MessageRecv);
		GetMessageParam(hv_MessageRecv, "key_exists", "stop", &hv_StopProcInfo);
		if (0 != (HTuple(hv_StopProcInfo[0])))
		{
			break;
		}
		//读取IO计数,如果该节点发生变化，代表前面检测到了NG缺陷
		//int nIn0(0), nIn1(0);
		//if (m_IOCard[0].hasCard)
		//{
		//	nIn0 = m_IOCard[0].readCounter(2);//
		//	nIn1 = m_IOCard[0].readCounter(3);
		//}
		//
		//if (m_nIn[0] != nIn0)
		//{
		//	m_bIn[0] = FALSE;
		//	m_tInit.Log("InChange", 0, FALSE);
		//}
		//if (m_nIn[1] != nIn1)
		//{
		//	m_bIn[1] = FALSE;
		//	m_tInit.Log("InChange", 1, FALSE);
		//}
		//m_nIn[0] = nIn0;//
		//m_nIn[1] = nIn1;

		C_MergeWork tOutput;
		CountSeconds(&hv_TimeStart);//开始计时
		tOutput.ReadMessage(hv_MessageRecv);
		ClearMessage(hv_MessageRecv);

		SendMeasureIO(tOutput);
		CountSeconds(&hv_TimeEnd);//结束计时

		CString strTime;
		strTime.Format("SCIIO:IO_thread%d:耗时%05.03fms",0,(hv_TimeEnd.D() - hv_TimeStart.D())*1000);
		Log(strTime,DebugLog);
	}
}
BOOL CIOCommunicate::SendMeasureIO(C_MergeWork& tOutput)
{
	try
	{
		CString strMess;
		//for (int i=0;i<PRODUCT_COUNT;i++)
		//{
		//	if (TRUE == tOutput.tEA[i].hvComplete.I())
		//	{
		//		if (TRUE != tOutput.tEA[i].hvResult.I() || TRUE == m_bLXTB[i])// || FALSE == m_bIn[i])
		//		{
		//			int nBis = tOutput.B[i].tMeasureOutput.tSide[i].hvMarkRow.D();
		//			int nImageSN = tOutput.B[i].A.hvImageSN;
		//			HTuple Width, Height;
		//			GetImageSize(tOutput.B[i].A.hoGrabImage, &Width, &Height);
		//			if (nBis < 0)
		//			{
		//				nImageSN -= 1;
		//				nBis = Height - nBis;
		//			}
		//			nBis /= 7;//因为接口卡做了6分频
		//			SendLabelIO(FALSE, nImageSN, i * 3, nBis);
		//			m_bIn[i] = TRUE;
		//			m_tInit.Log("AddCount", i, FALSE);
		//		}
		//		else
		//		{
		//			m_tInit.Log("AddCount", i, TRUE);
		//		}
		//	}	
		//}
	}
	catch (HException&e)
	{
		CString strExcp;
		strExcp.Format("IO_SendMeasureIO:%s", e.ErrorMessage().Text());
		Log(strExcp, ExcpLog, SlightExcp);
		return FALSE;
	}
	return TRUE;
}

BOOL CIOCommunicate::IO_heartBeat(BOOL bHeartBeat)
{
	if (!m_IOCard[0].hasCard)//红0 绿1 黄2 蜂鸣器3  心跳信号OUT7 
	{
		return false;
	}
	m_IOCard[0].setCardOutput(m_Commuicate.nAlarmHeatBeat, bHeartBeat);

	return true;
}

BOOL CIOCommunicate::SetAlarmStatus(const e_AlarmStatus& nParam)  //设置报警状态
{
	if (!m_IOCard[0].hasCard)//红0 绿1 黄2 蜂鸣器3  心跳信号OUT7 
	{
		return false;
	}

	if (nParam == ALARM_GREEN)//绿灯
	{
		m_IOCard[0].setCardOutput(m_Commuicate.nAlarmRedLight, 0);//红 0
		m_IOCard[0].setCardOutput(m_Commuicate.nAlarmGreenLight, 1);//绿 1
		m_IOCard[0].setCardOutput(m_Commuicate.nAlarmBuzzer, 0);//蜂鸣器 3
	}
	if (nParam == ALARM_RED) //红灯
	{
		m_IOCard[0].setCardOutput(m_Commuicate.nAlarmGreenLight, 0);
		m_IOCard[0].setCardOutput(m_Commuicate.nAlarmRedLight, 1);
		if (m_Alarms.bUseBuzzer) //蜂鸣器
		{
			m_IOCard[0].setCardOutput(m_Commuicate.nAlarmBuzzer, 1);
			Sleep(50);
			m_IOCard[0].setCardOutput(m_Commuicate.nAlarmBuzzer, 0);
		}
		else
		{
			m_IOCard[0].setCardOutput(m_Commuicate.nAlarmBuzzer, 0);
		}
	}
	return true;
}

BOOL CIOCommunicate::SendLabelIO(const BOOL& bFlag, const int& nPictureSN, const int& nChannel, int& nBias)
{
	if(!m_IOCard[0].hasCard) return FALSE;

	BOOL bSend = FALSE;
	if (REJECT_DEFULT == m_Commuicate.nRejectType)
	{
		bSend = !bFlag;
		
		//判断连续NG报警
		if (TRUE==m_Alarms.bEffective)
		{
			if (!bFlag)
				m_Alarms.nCurrentContinuesError++;
			else
				m_Alarms.nCurrentContinuesError=0;

			if (m_Alarms.nCurrentContinuesError>m_Alarms.nMaxContinuesError)
			{
				if (m_IOCard[0].hasCard)
				{
					m_IOCard[0].setCardOutput(m_Commuicate.nAlarmPort,TRUE);
					Sleep(m_Commuicate.nPulseWidth);
					m_IOCard[0].setCardOutput(m_Commuicate.nAlarmPort,FALSE);
				}
				m_Alarms.nCurrentContinuesError=0;
			}
		}
	}
	else
	{
		m_Alarms.nCurrentContinuesError = 0;
		return TRUE;
	}

	if (!bSend) return FALSE;
	
	unsigned short bySignEnable(1);	
	bySignEnable = bySignEnable<<0;

	m_IOCard[0].sendChckRslt(bySignEnable,nPictureSN,nChannel,nBias);//打标队列

	//int nCount = m_IOCard[0].readCounter(28);

	//CString strInfo;
	//static int s_nCount[4]={0};
	//s_nCount[nChannel]++;
	//strInfo.Format("SendLabelIO:Channel：%d SendNum：%d ImageSN：%d,Bias：%d",nChannel,s_nCount[nChannel],nPictureSN,nBias);
	//strInfo.Format("SendLabelIO_sendChckRslt:Channel:%d SendNum：%d ImageSN:%d IOCardImageSN:%d MarkRow:%d Flag:%d", nChannel, s_nCount[nChannel], nPictureSN, nCount, nBias, bFlag);
	//Log(strInfo, DebugLog);

	return TRUE;
}

int CIOCommunicate::DoSomething(void* message,void* wParam,void* lParam)
{
	CString strMessage;
	strMessage.Format("%s",(char*)message);
	if ("INITIALIZE" == strMessage || "Initialize" == strMessage)
	{
		return Init((PIOINIT)wParam);
	}else if ("PUSH_MESSAGE" == strMessage || "Push_Message" == strMessage)
	{
		return PushMessage(*(HTuple*)wParam);
	}else if ("SHOW" == strMessage || "Show" == strMessage)
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
		CDlgIOSet dlg;
		dlg.m_pCommuicate = this;
		dlg.DoModal();
		m_OutputhWnd=dlg.m_hWnd;
	}else if ("SHOWPARAM" == strMessage || "ShowParam" == strMessage)
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
		CDlgIOSet dlg;
		dlg.m_pCommuicate = this;
		dlg.DoModal();
		m_OutputhWnd=dlg.m_hWnd;
	}
	else if ("READY" == strMessage || "Ready" == strMessage)
	{
		CString strParam;
		strParam.Format("%s", wParam);
		return ReadyOutput(strParam);
	}
	else if ("READCOUNT" == strMessage || "ReadCount" == strMessage)
	{
		CString strCountSN;
		strCountSN.Format("%d",*(int*)wParam);
		if ("28" == strCountSN)
		{
			if (m_IOCard[0].hasCard)
			{
				*(int*)lParam = m_IOCard[0].readCounter(28);
			}else
			{
				*(int*)lParam = 0;
			}
		}else if ("29" == strCountSN)
		{

			if (m_IOCard[0].hasCard)
			{
				*(int*)lParam = m_IOCard[0].readCounter(29);
			}else
			{
				*(int*)lParam = 0;
			}
		}else if ("16" == strCountSN)
		{
			if (m_IOCard[0].hasCard)
			{
				*(int*)lParam = m_IOCard[0].readCounter(16);
			}else
			{
				*(int*)lParam = 0;
			}
		}else if ("4" == strCountSN)//读取输出0的计数
		{
			if (m_IOCard[0].hasCard)
			{
				*(int*)lParam = m_IOCard[0].readCounter(4);
			}else
			{
				*(int*)lParam = 0;
			}
		}else if ("5" == strCountSN)//读取输出1的计数
		{

			if (m_IOCard[0].hasCard)
			{
				*(int*)lParam = m_IOCard[0].readCounter(5);
			}else
			{
				*(int*)lParam = 0;
			}
		}else if ("6" == strCountSN)//读取输出2的计数
		{

			if (m_IOCard[0].hasCard)
			{
				*(int*)lParam = m_IOCard[0].readCounter(6);
			}else
			{
				*(int*)lParam = 0;
			}
		}else if ("32" == strCountSN)
		{

			if (m_IOCard[0].hasCard)
			{
				*(int*)lParam = m_IOCard[0].readParam(32);
			}else
			{
				*(int*)lParam = 0;
			}
		}else if ("49" == strCountSN)
		{

			if (m_IOCard[0].hasCard)
			{
				*(int*)lParam = m_IOCard[0].readParam(49);
			}else
			{
				*(int*)lParam = 0;
			}
		}else if ("57" == strCountSN)//延时队列0
		{

			if (m_IOCard[0].hasCard)
			{
				*(int*)lParam = m_IOCard[0].readParam(57);
			}else
			{
				*(int*)lParam = 0;
			}
		}
	}
	else if ("GETPARAM" == strMessage || "GetParam" == strMessage)
	{
		CString strParam;
		strParam.Format("%s",wParam);
		if ("MESSAGE_NUM" == strParam)
		{
			GetMessageQueueParam(m_hvQueue,"message_num",(HTuple*)lParam);
			return TRUE;
		}
	}
	else if ("ENABLE" == strMessage || "Enable" == strMessage)
	{
		for(int i=0;i<PIO24_COUNT;i++)
		{
			if (m_IOCard[i].hasCard)
			{
				m_IOCard[i].softReset();
				m_IOCard[i].enable(true);
				m_IOCard[i].softReset();//need reset twice
			}
		}
	}
	else if ("OUTPUT" == strMessage || "Output" == strMessage)
	{
		int nPort = *(int*)wParam;
		CString strParam;
		strParam.Format("%s",lParam);
		if (0 <= nPort && nPort <= 7)
		{
			if (m_IOCard[0].hasCard)
			{
				if ("ON" == strParam)
				{
					m_IOCard[0].setCardOutput(nPort,1);
				}else if ("OFF" == strParam)
				{
					m_IOCard[0].setCardOutput(nPort,0);
				}
			}
		}
	}
	else if ("ENCODER_COUNT" == strMessage || "Encoder_Count" == strMessage)
	{
		if (m_IOCard[0].hasCard)
		{
			int nCounter47,nCounter49,nCounter50,nCounter51;
			nCounter47 = m_IOCard[0].readCounter(47);
			nCounter49 = m_IOCard[0].readCounter(49);
			nCounter50 = m_IOCard[0].readCounter(50);
			nCounter51 = m_IOCard[0].readCounter(51);
			*(LONG*)lParam = (long)nCounter47 + (long)nCounter49 * 65536 + (long)nCounter50 * 65536 * 65536 
				+ (long)nCounter51 * 65536 * 65536 * 65536;
			return TRUE;
		}
	}
	else if ("GETCONNECTION" == strMessage || "GetConnection" == strMessage)
	{
		*(int*)lParam = m_nCurrentStatus;
		return TRUE;
	}
	else if ("SETALARMSTATUS" == strMessage || "SetAlarmStatus" == strMessage)//设置报警状态
	{		
		return SetAlarmStatus(*(e_AlarmStatus*) wParam);
	}
	else if ("IO_HEARTBEAT" == strMessage || "IO_heartBeat" == strMessage)//设置心跳接口
	{
		return IO_heartBeat((BOOL)wParam);
	}
	else
	{
		CString strMessageBox;
		strMessageBox.Format("IOCommunicate发现没有响应的DoSomething:%s",strMessage);
		Log(strMessageBox,ExcpLog);
		return FALSE;
	}
	return TRUE;
}

int CIOCommunicate::PushMessage(const HTuple& hvMessageSend)//放置消息进入队列
{
	try
	{
		EnqueueMessage(m_hvQueue, hvMessageSend, HTuple(), HTuple());//将消息投递到剔废车间
	}catch(HException& e)
	{
		if (H_ERR_MQOVL == e.ErrorCode())
		{
			CString strException;
			strException.Format("IO队列已满,整合线程已退出,请排查原因");
			Log(strException,ExcpLog);
		}else
		{
			throw e;
		}
		return FALSE;
	}
	return TRUE;
}

void* CIOCommunicate::HThreadIO(void *hcthread)//io线程
{
	HDevThread*  hcppthread = (HDevThread*) hcthread;
	try
	{
		const HTuple        &cbhv_Queue = hcppthread->GetInputCtrlParamTuple(0);
		g_This->IO_thread(cbhv_Queue);
		//hcppthread->Exit();
		//delete hcppthread;
	}
	catch(HException& e)
	{
		CString strException;
		strException.Format("HThreadIO出现异常\r\n异常信息%s  [ %s() ]",e.ErrorMessage().Text()
			,e.ProcName().Text());
		g_This->Log(strException,ExcpLog);
		bool is_direct_call = hcppthread->IsDirectCall();
		hcppthread->Exit();
		delete hcppthread;
		if (is_direct_call)
			throw e;
	}
	return NULL;
}

int CIOCommunicate::Init(PIOINIT pInit)//初始化
{
	///强制启用接口卡
	CString szGUID = _T("e7a1d5b1-64f7-4414-8dd7-e1479749ccbd");//接口卡类的guid号
	GUID guid;                  //全局唯一标识符(GUID，GloballyUnique Identifier)
	UuidFromString( (unsigned char*)((LPCTSTR)(szGUID.Left(36))), &guid);//多字节
	GetDeviceList(&guid,1);

#pragma region "读取配置文件"
	CString strValue;
	m_strProductName = pInit->strProductName;
	m_nProductType=pInit->nProductType;
	m_strConfigSystemFile.Format("%s\\Config\\System.vis",GetCurrentAppPath());
	m_strConfigCommonFile.Format("%s\\Config\\Common.dh",GetCurrentAppPath());
	m_strConfigCommunicateFile.Format("%s\\Config\\Public\\SCIIO\\IO.ini",GetCurrentAppPath());
	m_strConfigCalibrationFile.Format("%s\\Config\\Public\\Calibration\\0\\CameraParma.ini",GetCurrentAppPath());

	ReadParam();
	CString strKeyName;

	m_Commuicate.nPulseWidth = GetPrivateProfileInt("IO","PulseWidth",20,m_strConfigCommunicateFile);//输出信号脉冲宽度
	m_Commuicate.nRejectType = GetPrivateProfileInt("IO","RejectType",0,m_strConfigCommunicateFile);//剔废类型 如果为1是间隔剔废 如果为2为连续剔废 默认为0 根据结果好坏进行剔废
	m_Commuicate.nReadyPort = GetPrivateProfileInt("IO","Ready",0,m_strConfigCommunicateFile);//Ready信号
	m_Commuicate.nAlarmPort = GetPrivateProfileInt("IO","ContinuesErrorAlarms",7,m_strConfigCommunicateFile);//当发现连续错误大于最大个数以后发送信号的端口号

	m_Commuicate.nATotalDistance = GetPrivateProfileInt("IO","nATotalDistance",0,m_strConfigCommunicateFile);//A标机总距离
	m_Commuicate.nAFrameNum = GetPrivateProfileInt("IO","nAFrameNum",0,m_strConfigCommunicateFile);//A标机延迟帧数
	m_Commuicate.nBTotalDistance = GetPrivateProfileInt("IO","nBTotalDistance",0,m_strConfigCommunicateFile); // B标机总距离
	m_Commuicate.nBFrameNum = GetPrivateProfileInt("IO","nBFrameNum",0,m_strConfigCommunicateFile);//B标机延迟帧数
	m_Commuicate.nSingleImageHeight = GetPrivateProfileInt("IO","nSingleImageHeight",6000,m_strConfigCommunicateFile);//单张采集高度

	m_Alarms.bEffective = GetPrivateProfileInt("ContinuesErrorAlarms","Effective",0,m_strConfigCommunicateFile);//是否生效
	m_Alarms.nMaxContinuesError = GetPrivateProfileInt("ContinuesErrorAlarms","MaxContinuesError",5,m_strConfigCommunicateFile);//最大错误个数
	m_Alarms.nCurrentContinuesError = GetPrivateProfileInt("ContinuesErrorAlarms","CurrentErrorCount",0,m_strConfigCommunicateFile);//当前连续错误个数
	m_Commuicate.dMarkerLengthMM = GetPrivateProfileDouble("Marker","Length",20,m_strConfigCommunicateFile);
	m_nMarkerLength = (int)(m_Commuicate.dMarkerLengthMM / m_Common.dRowResolution[0]);//打标机标的长度（mm）

	m_Commuicate.nAlarmGreenLight = GetPrivateProfileInt("IO", "AlarmGreenLight", 1, m_strConfigCommunicateFile);
	m_Commuicate.nAlarmRedLight = GetPrivateProfileInt("IO", "AlarmRedLight", 0, m_strConfigCommunicateFile);
	m_Commuicate.nAlarmBuzzer = GetPrivateProfileInt("IO", "AlarmBuzzer", 3, m_strConfigCommunicateFile);
	m_Commuicate.nAlarmYellowLight = GetPrivateProfileInt("IO", "AlarmYellowLight", 2, m_strConfigCommunicateFile);
	m_Commuicate.nAlarmHeatBeat = GetPrivateProfileInt("IO", "AlarmHeatBeat", 7, m_strConfigCommunicateFile);
	for (int i=0;i<MAX_LANE;i++)
	{
		m_hvLabelPointRow[i].Clear();//贴标列坐标数组初始化
		m_hvDecreaseFrames[i].Clear();//延迟帧数递减处理数组初始化
	}

#pragma endregion
	m_tInit.Log = pInit->Log;
	m_tInit.nProductType = pInit->nProductType;
	m_tInit.strProductName.Format("%s",pInit->strProductName);
	g_This = this;
#pragma region "创建接口卡"
	for (int i=0;i<PIO24_COUNT;i++)
	{
		CString strConfigFile;
		strConfigFile.Format("PIO24B_reg_init.txt");
		if (0 != i)
		{
			strConfigFile.Format("PIO24B_reg_init%d.txt",i);
		}
		if(m_IOCard[i].openCard(i))
		{
			m_IOCard[i].hardReset();
			if (!m_IOCard[i].initRegisters(strConfigFile, "PIO24B"))
			{
				CString strMessageBox;
				strMessageBox.Format("SCIIO:%s注册失败",strConfigFile);
				Log(strMessageBox,ExcpLog);
				m_nCurrentStatus = FALSE;
			}
			else
			{
				m_nCurrentStatus = TRUE;
			}
		}else
		{
			CString strException;
			strException.Format("SCIIO:未发现接口卡%d",i);
			if (i==0)
			{
				Log(strException,ExcpLog);
				m_nCurrentStatus = FALSE;
			}
		}
	}

	CString strFilePath;
	strFilePath="D:\\贴标信息.txt";
	if (PathFileExists(strFilePath))
	{
		DeleteFile((HTuple)strFilePath);
	}
	



#pragma endregion
	InitThread();
	return TRUE;
}

BOOL CIOCommunicate::ReadyOutput(CString &strParam)
{
	if (!m_IOCard[0].hasCard)
	{
		return FALSE;
	}
	if ("ON" == strParam)
	{
		m_IOCard[0].setCardOutput(m_Commuicate.nReadyPort,1);
	}else if ("OFF" == strParam)
	{
		m_IOCard[0].setCardOutput(m_Commuicate.nReadyPort,0);
	}else
	{
		return FALSE;
	}
	return TRUE;
}

void CIOCommunicate::InitThread()
{
#pragma region "创建队列和线程"
	CreateMessageQueue(&m_hvQueue);//创建一个IO队列
	SetMessageQueueParam(m_hvQueue,"max_message_num",80);
	HDevThreadContext Context;
	HDevThread* pThread = new HDevThread(Context,
		(void*)HThreadIO,1,0);//开启一个具有1个输入的线程;
	pThread->SetInputCtrlParamTuple(0,m_hvQueue);
	pThread->Start();
	m_vecThreadHandle.push_back(pThread);
#pragma endregion
	
}

void CIOCommunicate::ReadParam()
{
	//公共参数 ： 设备类型/纵向分辨率/横向分辨率
	m_Common.emCoatType = (e_CoatType)GetPrivateProfileInt(m_strProductName,"CoatType",0, m_strConfigCommonFile);
	m_Common.dRowResolution[0] = GetPrivateProfileDouble("CalibrationParam","YResolution",0.056600, m_strConfigCalibrationFile);
	m_Common.dRowResolution[1] = GetPrivateProfileDouble("CalibrationParam","YResolution",0.056600, m_strConfigCalibrationFile);
	m_Common.dColumnResolution[0] = GetPrivateProfileDouble("CalibrationParam","XResolution",0.055103, m_strConfigCalibrationFile);
	m_Common.dColumnResolution[1] = GetPrivateProfileDouble("CalibrationParam","XResolution",0.055103, m_strConfigCalibrationFile);
}

BOOL CIOCommunicate::Log(const CString& strLog,const int& nType,const int&nExcpLevel)
{
	if(m_tInit.Log==NULL) return FALSE;
	return m_tInit.Log(strLog,nType,nExcpLevel);
}
