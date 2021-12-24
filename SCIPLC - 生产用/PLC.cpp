#include "StdAfx.h"
#include <math.h>
#include "DlgParamSet.h"
extern "C" __declspec(dllexport) DHInterface* Export(void)
{
	return (DHInterface*)new CPLC();
}

CPLC* g_This = NULL;

CPLC::CPLC()
	:m_hvec_Data(1)
{
	g_This = this;
	m_pFinsTcp = NULL;
	m_pMdbsTcp = NULL;
	m_pAdsTcp = NULL;
	m_dCorrectValue = 0;
	m_dDiffValue = 0;
}

CPLC::~CPLC()
{
	if (m_pFinsTcp != NULL)
	{
		delete m_pFinsTcp;
		m_pFinsTcp = NULL;
	}
	if (m_pMdbsTcp != NULL)
	{
		delete m_pMdbsTcp;
		m_pMdbsTcp = NULL;
	}
	if (m_pAdsTcp != NULL)
	{
		delete m_pAdsTcp;
		m_pAdsTcp = NULL;
	}
}

/************************************
*@Method:    DoSomething
*@Access:    virtual public
*@Returns:   int
*@Author: 	 Fowindy
*@Parameter: void * message
*@Parameter: void * wParam
*@Parameter: void * lParam
*@Created:   2021/03/07 14:08
*@Describe:	 PLC_DoSomething函数
*************************************/
int CPLC::DoSomething(void* message, void* wParam, void* lParam)
{
	CString strMessage;
	strMessage.Format("%s", (char*)message);
	if ("INITIALIZE" == strMessage || "Initialize" == strMessage)
	{
		return Init(*(PPLCINIT)wParam);
	}
	else if ("PUSH_MESSAGE" == strMessage || "Push_Message" == strMessage)
	{
		return PushMessage(*(HTuple*)wParam);
	}
	else if ("SHOW" == strMessage || "Show" == strMessage)
	{
		return ShowParamDlg();
	}
	else if ("READINT32" == strMessage || "ReadInt32" == strMessage)
	{
		return ReadInt32(*(int*)wParam, *(int*)lParam);
	}
	else if ("WRITEINT32" == strMessage || "WriteInt32" == strMessage)
	{
		return WriteInt32(*(int*)wParam, *(int*)lParam);
	}
	else if ("READFLOAT32" == strMessage || "ReadFloat32" == strMessage)
	{
		return ReadFloat32(*(int*)wParam, *(float*)lParam);
	}
	else if ("WRITEFLOAT32" == strMessage || "WriteFloat32" == strMessage)
	{
		return WriteFloat32(*(int*)wParam, *(float*)lParam);
	}
	else if ("READINT32BATCH" == strMessage || "ReadInt32Batch" == strMessage)
	{
		std::vector<int> vecVal = *(std::vector<int>*)lParam;
		ReadInt32Batch(*(int*)wParam, vecVal.size(), vecVal);
		*(std::vector<int>*)lParam = vecVal;

		return TRUE;
	}
	else if ("READSTRING" == strMessage || "ReadString" == strMessage)
	{
		return ReadString(*(int*)wParam, *(CString*)lParam);
	}
	else if ("GETCONNECTION" == strMessage || "GetConnection" == strMessage)
	{
		if (FinsTCP == m_tPLCSet.nUsePlc && m_pFinsTcp != NULL)
		{
			*(int*)lParam = m_pFinsTcp->IsConnected();
			return TRUE;
		}
		else if (ModbusTCP == m_tPLCSet.nUsePlc && m_pMdbsTcp != NULL)
		{
			*(int*)lParam = m_pMdbsTcp->IsConnected();
			return TRUE;
		}
		else if (AdsTCP == m_tPLCSet.nUsePlc && m_pAdsTcp != NULL)
		{
			*(int*)lParam = m_pAdsTcp->IsConnected();
			return TRUE;
		}
		else
		{
			*(int*)lParam = FALSE;
			return FALSE;
		}
	}
	else if ("SETHEARTBEAT" == strMessage || "SetHeartBeat" == strMessage)
	{
		return SendHeartBeat(*(int*)wParam);
	}
	else if ("RECONNECT" == strMessage || "Reconnect" == strMessage)
	{
		if (FinsTCP == m_tPLCSet.nUsePlc)
		{
			return m_pFinsTcp->Reconnect();
		}
		else if (ModbusTCP == m_tPLCSet.nUsePlc)
		{
			return m_pMdbsTcp->Reconnect();
		}
		else if (AdsTCP == m_tPLCSet.nUsePlc)
		{
			return m_pAdsTcp->Reconnect();
		}
		else
		{
			return FALSE;
		}
	}
	else if ("WRITEALARMTOPLC" == strMessage || "WriteAlarmToPLC" == strMessage)
	{
		int nAddr = ALARM;
		return WriteInt32(nAddr, *(int*)wParam);
	}
	else if ("READPLCSTATUS" == strMessage || "ReadPLCStatus" == strMessage)
	{
		int nAddr = PLC_STATUS;
		return ReadInt32(nAddr, *(int*)wParam);
	}
	else if ("READTUBUSTATUS" == strMessage || "ReadTUBUStatus" == strMessage)
	{
		int nAddr = TUBU_STATUS;
		return ReadInt32(nAddr, *(int*)wParam);
	}
	else if ("READTUBUALARM" == strMessage || "ReadTUBUSAlarm" == strMessage)
	{
		int nAddr = TUBU_ALARM;
		return ReadInt32(nAddr, *(int*)wParam);
	}
	else if ("READALARMRESET" == strMessage || "ReadAlarmReset" == strMessage)
	{
		int nAddr = ALARM_RESET;
		return ReadInt32(nAddr, *(int*)wParam);
	}
	else if ("READUPSALARM" == strMessage || "ReadUPSAlarm" == strMessage)
	{
		int nAddr = UPS_ALARM;
		return ReadInt32(nAddr, *(int*)wParam);
	}
	else if ("GETCORRECTINGENABLE" == strMessage || "GetCorrectingEnable" == strMessage)
	{
		*(int*)lParam = m_tPLCSet.nUseCorrect;
		return TRUE;
	}
	else if ("GETCORRECTVALUE" == strMessage || "GetCorrectValue" == strMessage)
	{
		return GetCorrectVal(*(double*)lParam);
	}
	else if ("GETCORRECTDIFFVALUE" == strMessage || "GetCorrectDiffValue" == strMessage)
	{
		return GetCorrectDiffVal(*(double*)lParam);
	}
	else if ("READBATCHNAME" == strMessage || "ReadBatchName" == strMessage)
	{
		return GetBatchName(*(CString*)lParam);
	}
	else if ("WRITEBATCHNAMEVERIFY" == strMessage || "WriteBatchNameVerify" == strMessage)
	{
		return WriteBatchNameVerify(*(CString*)wParam);
	}
	else if("CCDENABLE" == strMessage || "CCDEnable" == strMessage)
	{
		return SendCCDEnable(*(int*)wParam);
	}
	else if ("PLCRUNDIRECTION" == strMessage || "PLCRunDirection" == strMessage)
	{
		return GetPLCRunDirection(*(int*)lParam);
	}
	else if ("PLCDIEHEADDIRECTION" == strMessage || "PLCDieHeadDirection" == strMessage)
	{
		return GetPLCDieHeadDirection(*(int*)lParam);
	}
	else if ("PLCDIEHEADPOSITION" == strMessage || "PLCDieHeadPosition" == strMessage)
	{
		return GetPLCDieHeadPosition(*(double*)lParam);
	}
	else if ("PLCSTARTCOAT" == strMessage || "PlcStartCoat" == strMessage)
	{
		return GetPLCStartCoat(*(int*)lParam);
	}
	else if ("CCDALARM" == strMessage || "CCDAlarm" == strMessage)
	{
		return SendAlarm(2);
	}
	else if ("SETPRODUCTINFO" == strMessage || "SetProductInfo" == strMessage)
	{
		m_sInit.tPdInfo = *(tagPDInfo*)wParam;
		return TRUE;
	}

	return TRUE;
}

void  hex_to_float(const float& val, HTuple hvdata)
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

int CPLC::Init(PLCINIT& Init)
{
	m_sInit = Init;

	m_strConfigPLCFile.Format("%s\\Config\\Public\\SCIPLC\\PLC.ini", GetCurrentAppPath());
	ReadParam();

	m_pFinsTcp = new CFinsTcpExt;
	m_pMdbsTcp = new CModbusTcpExt;
	m_pAdsTcp = new CAdsExt;

	//HTuple hv_Width;
	//hv_Width.Append(0);
	//hv_Width.Append(2.662);
	//hv_Width.Append(0.2837);
	//hv_Width.Append(2.7256);
	//hv_Width.Append(0.3681);
	//hv_Width.Append(2.4755);
	//HTuple hv_FindResult0 = hv_Width.TupleFind(0);
	//HTuple hv_FindResultNoData = hv_Width.TupleFind(NO_DATA);
	//if (hv_FindResultNoData != -1 || hv_FindResultNoData.Length() == 0)
	//{
	//	//strLog.Format("SCIPLC_Correcting2：漏凹版值无效或者没找到边,不执行闭环");
	//	//Log(strLog, DebugLog);
	//	return FALSE;
	//}
	////2021.3.24，增加一种情况，判断凹版不为0的个数如果小于等于4个，就认为都可以闭环，包含涂膜覆盖凹版的时候
	//HTuple hvMod, hv_CorrectTemp, hv_CorrectValue;
	//TupleMod(hv_FindResult0, 2, &hvMod);
	////正常覆盖凹版的时候，找到0的数都是纯奇数或者纯偶数
	//if (hv_FindResultNoData != -1 || hv_FindResult0.Length() > 4)
	//{
	//	//strLog.Format("SCIPLC_Correcting2：漏凹版值没找到的边过多,不执行闭环");
	//	//Log(strLog, DebugLog);
	//	return FALSE;
	//}

	//for (int i = 0; i < hv_Width.Length() / 2; i++)
	//{
	//	hv_CorrectTemp = (hv_Width[2 * i] - hv_Width[2 * i + 1]) / 2;
	//	hv_CorrectValue.Append(hv_CorrectTemp);
	//}

	//HTuple hvMean = hv_CorrectValue.TupleMean().D();

	if (FinsTCP == m_tPLCSet.nUsePlc)
	{
		if (!m_pFinsTcp->Connect(m_tPLCSet.strPLCIP, m_tPLCSet.nPLCPort, CT_CLIENT))
		{
			Log("SCIPLC:FinsPLC连接失败,请检查网络", ExcpLog);
			m_tPLCSet.nUsePlc = None;
		}
	}
	else if (ModbusTCP == m_tPLCSet.nUsePlc)
	{
		if (!m_pMdbsTcp->Connect(m_tPLCSet.strPLCIP, m_tPLCSet.nPLCPort))
		{
			m_tPLCSet.nUsePlc = None;
			Log("SCIPLC:ModbusPLC连接失败,请检查网络", ExcpLog);
		}
	}
	else
	{
		if (!m_pAdsTcp->Connect(m_tPLCSet.strPLCIP, m_tPLCSet.nPLCPort))
		{
			m_tPLCSet.nUsePlc = None;
			Log("SCIPLC:AdsPLC连接失败,请检查网络", ExcpLog);
		}
	}

#pragma region "创建队列和线程"
	CreateMessageQueue(&m_hvQueue);//创建一个IO队列
	SetMessageQueueParam(m_hvQueue, "max_message_num", 25);
	HDevThreadContext Context;
	HDevThread* pThread = new HDevThread(Context,
		(void*)HThreadPlc, 0, 0);//开启一个具有2个输入的线程;
	HTuple ProcThreads;
	pThread->ParStart(&ProcThreads);
#pragma endregion

	return TRUE;
}

/************************************
*@Method:    ReadParam
*@Access:    public
*@Returns:   void
*@Author: 	 Fowindy
*@Created:   2021/02/27 10:53
*@Describe:	 PLC界面_读取配置文件参数
*************************************/
void CPLC::ReadParam()
{
	GetPrivateProfileString("Server", "IP", "127.0.0.1", m_tPLCSet.strPLCIP.GetBuffer(MAX_PATH), MAX_PATH, m_strConfigPLCFile);
	m_strConfigCalibrationFile.Format("%s\\Config\\Public\\Calibration\\0\\CameraParma.ini", GetCurrentAppPath());
	m_tPLCSet.strPLCIP.ReleaseBuffer();//读取产品名称
	m_tPLCSet.nPLCPort = GetPrivateProfileInt("Server", "Port", 9600, m_strConfigPLCFile);
	m_tPLCSet.nSingleCorrectAddress = GetPrivateProfileInt("Config", "SingleCorrectAddress", 0, m_strConfigPLCFile);
	m_tPLCSet.nDoubleCorrectAddress = GetPrivateProfileInt("Config", "DoubleCorrectAddress", 0, m_strConfigPLCFile);
	m_tPLCSet.nPlcHeartBeatAddress = GetPrivateProfileInt("Config", "PlcHeartBeatAddress", 0, m_strConfigPLCFile);
	m_tPLCSet.nPlcBatchNameAddress = GetPrivateProfileInt("Config", "PlcBatchNameAddress", 0, m_strConfigPLCFile);
	m_tPLCSet.nPlcBatchNameVerifyAddress = GetPrivateProfileInt("Config", "PlcBatchNameVerifyAddress", 0, m_strConfigPLCFile);
	m_tPLCSet.nPlcCCDEnableAddress = GetPrivateProfileInt("Config", "PlcCCDEnableAddress", 0, m_strConfigPLCFile);
	m_tPLCSet.nDoCorrectAddress = GetPrivateProfileInt("Config", "DoCorrectAddress", 0, m_strConfigPLCFile);
	m_tPLCSet.nPlcRunDirectionAddress = GetPrivateProfileInt("Config", "PlcRunDirectionAddress", 0, m_strConfigPLCFile);
	m_tPLCSet.nPlcDieHeadDirectionAddress = GetPrivateProfileInt("Config", "PlcDieHeadDirectionAddress", 0, m_strConfigPLCFile);
	m_tPLCSet.nPlcStartCoatAddress = GetPrivateProfileInt("Config", "PlcStartCoatAddress", 0, m_strConfigPLCFile);
	m_tPLCSet.nPlcDieHeadPosAddress = GetPrivateProfileInt("Config", "PlcDieHeadPosAddress", 0, m_strConfigPLCFile);
	m_tPLCSet.nPlcAlarmAddress = GetPrivateProfileInt("Config", "PlcAlarmAddress", 0, m_strConfigPLCFile);

	m_tPLCSet.nUsePlc = GetPrivateProfileInt("Config", "Use", FALSE, m_strConfigPLCFile);
	m_tPLCSet.nUseCorrect = GetPrivateProfileInt("Config", "UseCorrect", FALSE, m_strConfigPLCFile);
	m_tPLCSet.nCorrectDirection = GetPrivateProfileInt("Config", "CorrectDirection", 0, m_strConfigPLCFile);
	m_tPLCSet.nCorrectType = GetPrivateProfileInt("Config", "CorrectType", 0, m_strConfigPLCFile);
	m_tPLCSet.nCorrectLogicType = GetPrivateProfileInt("Config", "CorrectLogicType", 0, m_strConfigPLCFile);
	m_tPLCSet.dAlertDiff = GetPrivateProfileDouble("Config", "AlertDiff", 2, m_strConfigPLCFile);
	m_tPLCSet.nFilterType = GetPrivateProfileInt("Config", "FilterType", 0, m_strConfigPLCFile);
	m_tPLCSet.nFilterSize = GetPrivateProfileInt("Config", "FilterSize", 5, m_strConfigPLCFile);
	m_tPLCSet.dCorrectPercent = GetPrivateProfileDouble("Config", "CorrectPercent", 0.5, m_strConfigPLCFile);
	m_tPLCSet.dCorrectPercent = (m_tPLCSet.dCorrectPercent < 0 || m_tPLCSet.dCorrectPercent > 1) ? 1.0 : m_tPLCSet.dCorrectPercent;
	m_tPLCSet.dCamDistance = GetPrivateProfileDouble("Config", "CamDistance", 1.5, m_strConfigPLCFile);
	m_tPLCSet.dCorrectInterval = GetPrivateProfileDouble("Config", "CorrectInterval", 5, m_strConfigPLCFile);
	m_tPLCSet.dCorrectIntervalLong = GetPrivateProfileDouble("Config", "CorrectIntervalLong", 10, m_strConfigPLCFile);
	m_tPLCSet.dRowResolution = GetPrivateProfileDouble("CalibrationParam", "YResolution", 0.056600, m_strConfigCalibrationFile);

	m_tPLCSet.bJudgeGuidFinish = GetPrivateProfileInt("Config", "JudgeGuidFinish", FALSE, m_strConfigPLCFile);
	m_tPLCSet.nGuidFinishAddr = GetPrivateProfileInt("Config", "GuidFinishAddr", 0, m_strConfigPLCFile);
}

/************************************
*@Method:    WriteParam
*@Access:    public
*@Returns:   void
*@Author: 	 Fowindy
*@Created:   2021/02/27 10:57
*@Describe:	 PLC界面_写入配置文件参数
*************************************/
void CPLC::WriteParam()
{
	//初始化保存设置
	WritePrivateProfileString("Server", "IP", m_tPLCSet.strPLCIP, m_strConfigPLCFile);
	WritePrivateProfileInt("Server", "Port", m_tPLCSet.nPLCPort, m_strConfigPLCFile);
	WritePrivateProfileInt("Config", "SingleCorrectAddress", m_tPLCSet.nSingleCorrectAddress, m_strConfigPLCFile);
	WritePrivateProfileInt("Config", "DoubleCorrectAddress", m_tPLCSet.nDoubleCorrectAddress, m_strConfigPLCFile);
	WritePrivateProfileInt("Config", "PlcHeartBeatAddress", m_tPLCSet.nPlcHeartBeatAddress, m_strConfigPLCFile);
	WritePrivateProfileInt("Config", "PlcBatchNameAddress", m_tPLCSet.nPlcBatchNameAddress, m_strConfigPLCFile);
	WritePrivateProfileInt("Config", "PlcBatchNameVerifyAddress", m_tPLCSet.nPlcBatchNameVerifyAddress, m_strConfigPLCFile);
	WritePrivateProfileInt("Config", "PlcCCDEnableAddress", m_tPLCSet.nPlcCCDEnableAddress, m_strConfigPLCFile);
	WritePrivateProfileInt("Config", "DoCorrectAddress", m_tPLCSet.nDoCorrectAddress, m_strConfigPLCFile);
	WritePrivateProfileInt("Config", "PlcRunDirectionAddress", m_tPLCSet.nPlcRunDirectionAddress, m_strConfigPLCFile);
	WritePrivateProfileInt("Config", "PlcDieHeadDirectionAddress", m_tPLCSet.nPlcDieHeadDirectionAddress, m_strConfigPLCFile);
	WritePrivateProfileInt("Config", "PlcStartCoatAddress", m_tPLCSet.nPlcStartCoatAddress, m_strConfigPLCFile);
	WritePrivateProfileInt("Config", "PlcDieHeadPosAddress", m_tPLCSet.nPlcDieHeadPosAddress, m_strConfigPLCFile);
	WritePrivateProfileInt("Config", "PlcAlarmAddress", m_tPLCSet.nPlcAlarmAddress, m_strConfigPLCFile);

	WritePrivateProfileInt("Config", "Use", m_tPLCSet.nUsePlc, m_strConfigPLCFile);
	WritePrivateProfileInt("Config", "UseCorrect", m_tPLCSet.nUseCorrect, m_strConfigPLCFile);
	WritePrivateProfileInt("Config", "CorrectType", m_tPLCSet.nCorrectType, m_strConfigPLCFile);
	WritePrivateProfileInt("Config", "CorrectDirection", m_tPLCSet.nCorrectDirection, m_strConfigPLCFile);
	WritePrivateProfileInt("Config", "CorrectLogicType", m_tPLCSet.nCorrectLogicType, m_strConfigPLCFile);
	WritePrivateProfileDouble("Config", "AlertDiff", m_tPLCSet.dAlertDiff, m_strConfigPLCFile);
	WritePrivateProfileInt("Config", "FilterType", m_tPLCSet.nFilterType, m_strConfigPLCFile);
	WritePrivateProfileInt("Config", "FilterSize", m_tPLCSet.nFilterSize, m_strConfigPLCFile);
	m_tPLCSet.dCorrectPercent = (m_tPLCSet.dCorrectPercent < 0 || m_tPLCSet.dCorrectPercent > 1) ? 1.0 : m_tPLCSet.dCorrectPercent;
	WritePrivateProfileDouble("Config", "CorrectPercent", m_tPLCSet.dCorrectPercent, m_strConfigPLCFile);
	WritePrivateProfileDouble("Config", "CorrectInterval", m_tPLCSet.dCorrectInterval, m_strConfigPLCFile);
	WritePrivateProfileDouble("Config", "CorrectIntervalLong", m_tPLCSet.dCorrectIntervalLong, m_strConfigPLCFile);
	WritePrivateProfileDouble("Config", "CamDistance", m_tPLCSet.dCamDistance, m_strConfigPLCFile);

	WritePrivateProfileInt("Config", "JudgeGuidFinish", m_tPLCSet.bJudgeGuidFinish, m_strConfigPLCFile);
	WritePrivateProfileInt("Config", "GuidFinishAddr", m_tPLCSet.nGuidFinishAddr, m_strConfigPLCFile);
}

void* CPLC::HThreadPlc(void *hcthread)//io线程
{
	HDevThread*  hcppthread = (HDevThread*)hcthread;
	try
	{
		g_This->plc_thread();
		hcppthread->Exit();
		delete hcppthread;
	}
	catch (HException& e)
	{
		CString strException;
		strException.Format("SCIPLC:HThreadPlc出现异常:异常信息%s  [ %s() ]", e.ErrorMessage().Text()
			, e.ProcName().Text());
		g_This->Log(strException, ExcpLog);
		bool is_direct_call = hcppthread->IsDirectCall();
		hcppthread->Exit();
		delete hcppthread;
		if (is_direct_call)
			throw e;
	}
	return NULL;
}

/************************************
*@Method:    plc_thread
*@Access:    private
*@Returns:   void
*@Author: 	 Fowindy
*@Created:   2021/03/04 23:41
*@Describe:	 PLC线程函数
*************************************/
void CPLC::plc_thread()
{
	/*if (!m_tPLCSet.nUsePlc)
	{
		return;
	}
	BOOL bCon = m_pFinsTcp->Connect(m_tPLCSet.strPLCIP, m_tPLCSet.nPLCPort, CT_CLIENT);
	if (!bCon)
	{
		Log("SCIPLC:PLC连接失败,已退出通信线程", ExcpLog);
		return;
	}*/
	HTuple hv_MessageRecv, hv_StopProcInfo, hv_TimeStart, hv_TimeEnd, hv_ErrorFlag;
	C_MergeWork Output;
	HTuple hvQueue = m_hvQueue;
	while (TRUE)
	{
		DequeueMessage(hvQueue, "timeout", "infinite", &hv_MessageRecv);
		GetMessageParam(hv_MessageRecv, "key_exists", "stop", &hv_StopProcInfo);
		if (0 != (HTuple(hv_StopProcInfo[0])))
		{
			break;
		}
		try
		{
			CountSeconds(&hv_TimeStart);//开始计时
			Output.ReadMessage(hv_MessageRecv);
			HTuple hvImageSN, hvFlag, hvErrorIndices, Height;//图像号
			HObject hoGrabImage, hoErrorRegion;
			GenEmptyObj(&hoErrorRegion);

			if (FinsTCP == m_tPLCSet.nUsePlc)
			{
				if (m_pFinsTcp == NULL)
				{
					return;
				}
			}
			else if (ModbusTCP == m_tPLCSet.nUsePlc)
			{
				if (m_pMdbsTcp == NULL)
				{
					return;
				}
			}
			else if (AdsTCP == m_tPLCSet.nUsePlc)
			{
				if (m_pAdsTcp == NULL)
				{
					return;
				}
			}

			if (m_tPLCSet.nUseCorrect)
			{
				Correcting(Output);//闭环逻辑，含W21和W3
			}
			CountSeconds(&hv_TimeEnd);//结束计时
			hv_TimeEnd = hv_TimeEnd - hv_TimeStart;
			ClearMessage(hv_MessageRecv);
		}
		catch (HException& e)
		{
			CString strException;
			strException.Format("SCIPLC:plc_thread异常: %s", e.ErrorMessage().Text());
			Log(strException, ExcpLog);
		}
	}
}

int CPLC::PushMessage(const HTuple& hvMessageSend)//放置消息进入队列
{
	/*if (!m_tPLCSet.nUsePlc)
	{
		return FALSE;
	}*/
	try
	{
		EnqueueMessage(m_hvQueue, hvMessageSend, HTuple(), HTuple());//将消息投递到剔废车间
	}
	catch (HException& e)
	{
		CString strException;
		strException.Format("SCIPLC_PushMessage: %s(%s)", e.ErrorMessage().Text(), e.ProcName().Text());
		Log(strException, ExcpLog);

		return FALSE;
	}
	return TRUE;
}

INT32 CPLC::TransInt(double dIn, int nDecimal)
{
	if (nDecimal < 0)
	{
		nDecimal = 0;
	}
	INT32 nRes = (INT32)(dIn * pow(10.0, nDecimal) + 0.5);//取整四舍五入

	return nRes;
}

BOOL CPLC::ShowParamDlg()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CDlgParamSet dlg;
	//CDlgIOSetting dlg;
	dlg.m_pOwner = this;
	dlg.DoModal();

	return TRUE;
}

BOOL CPLC::ReadInt32(const int& nDMAddr, int& nReadVal)
{
	if (FinsTCP == m_tPLCSet.nUsePlc)
	{
		return m_pFinsTcp->ReadInt32(nDMAddr, nReadVal);
	}
	return FALSE;
}

BOOL CPLC::WriteInt32(const int& nDMAddr, const int& nWriteVal)
{
	if (FinsTCP == m_tPLCSet.nUsePlc)
	{
		return m_pFinsTcp->WriteInt32(nDMAddr, nWriteVal);
	}
	return FALSE;
}

BOOL CPLC::ReadString(const int& nDMAddrStart, CString& strReadVal)
{
	if (FinsTCP == m_tPLCSet.nUsePlc)
	{
		return m_pFinsTcp->ReadStringBatch(nDMAddrStart, 80, strReadVal);
	}
	return FALSE;
}

BOOL CPLC::ReadInt32Batch(const int& nDMAddrStart, const int& nReadNum, std::vector<int>& vecReadVal)
{
	if (FinsTCP == m_tPLCSet.nUsePlc)
	{
		return m_pFinsTcp->ReadInt32Batch(nDMAddrStart, nReadNum, vecReadVal);
	}
	return FALSE;
}

BOOL CPLC::ReadFloat32(const int& nDMAddr, float& fReadVal)
{
	if (FinsTCP == m_tPLCSet.nUsePlc)
	{
		return m_pFinsTcp->ReadFloat32(nDMAddr, fReadVal);
	}
	return FALSE;
}

BOOL CPLC::WriteFloat32(const int& nDMAddr, const float& fWriteVal)
{
	if (FinsTCP == m_tPLCSet.nUsePlc)
	{
		return m_pFinsTcp->WriteFloat32(nDMAddr, fWriteVal);
	}
	return FALSE;
}

BOOL CPLC::Log(const CString& strLog, const int& nType, const int&nExcpLevel)
{
	if (m_sInit.Log == NULL) return FALSE;
	return m_sInit.Log(strLog, nType, nExcpLevel);
}

/************************************
*@Method:    Correcting
*@Access:    private
*@Returns:   BOOL
*@Author: 	 Fowindy
*@Parameter: C_MergeWork & Output
*@Created:   2021/03/08 11:11
*@Describe:	 纠偏逻辑
*************************************/
BOOL CPLC::Correcting(C_MergeWork &Output)
{
	CString strLog;
	HTuple hv_Width, hv_Height;
	GetImageSize(Output.vecB[0].vecA[0].hoGrabImage, &hv_Width, &hv_Height);
	hv_Height = hv_Height.D() * m_tPLCSet.dRowResolution / 1000;
	HTuple hv_CorrectValue;

	//2021.4.19 按照涂布机涂头左正右负的逻辑，AB面错位改为错位求均值，再进行中值滤波，然后滤波值发送给PLC

	static double s_dDelayInterval = m_tPLCSet.dCorrectInterval + m_tPLCSet.dCamDistance;//设定延迟距离，用于动态调整延迟距离
	static double s_dDelayCamDistanceCount = 0;
	static double s_dDelayDistance[2] = { 0,0 };
	static double s_dData[2] = { 0,0 };
	static HTuple s_hvFilterData = HTuple();

#pragma region 料带跑偏报警
	//基材位置
	static HTuple s_hvEdgeRealX;
	HTuple hvEdgeCol, hvEdgeColReal = HTuple();
	hvEdgeCol = (Output.vecB[0].tMeasure.tLine.hv_StartColumn[0].D() + Output.vecB[0].tMeasure.tLine.hv_EndColumn[0].D()) / 2;
	m_sInit.Main(MT_PLC,"CalColPoint","Side0", &hvEdgeCol, &hvEdgeColReal);
	if (hvEdgeColReal.Length() > 0 && hvEdgeColReal != NO_DATA && TRUE == Output.vecB[0].tMeasure.hv_HasCoat.I())
	{
		s_hvEdgeRealX.Append(hvEdgeColReal);
		int nLength = 30.0 / hv_Height.D();
		if (s_hvEdgeRealX.Length() > nLength)
			TupleRemove(s_hvEdgeRealX, 0, &s_hvEdgeRealX);
		if ((s_hvEdgeRealX.TupleMax().D() - s_hvEdgeRealX.TupleMin().D()) > 10)
		{
			CString strLog;
			strLog.Format("料带30米内位置偏差(%0.2fmm)已超过10mm，可能影响闭环工作，请检查", (s_hvEdgeRealX.TupleMax().D() - s_hvEdgeRealX.TupleMin().D()));
			m_sInit.Log(strLog, AlarmLog, NoExcp);
			s_hvEdgeRealX.Clear();
		}
	}
	else
	{
		s_hvEdgeRealX.Clear();
	}
#pragma endregion

	//先判断单面,只做A面闭环，双面只做B面闭环
#pragma region 单面纠偏逻辑
	try
	{
		if ((m_sInit.tPdInfo.nCoatingType == OneSide/* || m_tPLCSet.nCorrectType == SingleSide*/) && TRUE == Output.vecB[0].tMeasure.hv_HasCoat.I())
		{
			static double nNGMeterCount = 0;
			if (!_CalCorrectDiff(Output, hv_CorrectValue))
			{
				if (Output.vecB[0].tMeasure.hv_HasCoat.I())
				{
					nNGMeterCount += hv_Height.D();
					if (nNGMeterCount > 10)
					{
						nNGMeterCount = 0;
						CString strLog;
						strLog.Format("单面闭环: 连续%0.2fm含有无效测量值，闭环未工作，请检查", 10);
						m_sInit.Log(strLog, AlarmLog, NoExcp);
					}
				}
				else
				{
					nNGMeterCount = 0;
				}
				return FALSE;
			}
			else
			{
				nNGMeterCount = 0;
			}

			m_dDiffValue = hv_CorrectValue.D();

			if (_JudgeGuideFinish())
			{
				s_dDelayCamDistanceCount += hv_Height.D();
				if (s_dDelayCamDistanceCount < m_tPLCSet.dCamDistance)
				{
					strLog.Format("SCIPLC_单面闭环: 未达到相机到模头设定距离(%0.3fm)，返回", s_dDelayCamDistanceCount);
					Log(strLog, DebugLog);
					return FALSE;
				}
			}
			else
			{
				//模头未执行到位，重新记米
				strLog.Format("SCIPLC_单面闭环: 模头未执行到位，重新记录延迟距离");
				Log(strLog, DebugLog);
				s_dDelayDistance[SingleSide] = -hv_Height.D();
				s_dDelayCamDistanceCount = 0;
				s_hvFilterData.Clear();
				return FALSE;
			}

			s_hvFilterData.Append(m_dDiffValue);

			s_dDelayDistance[SingleSide] += hv_Height.D();
			if (s_dDelayDistance[SingleSide] < s_dDelayInterval)
			{
				strLog.Format("SCIPLC_单面闭环: 未达到设定纠偏延迟距离(%0.3fm)，返回", s_dDelayDistance[SingleSide]);
				Log(strLog, DebugLog);
				return FALSE;
			}

			if (m_tPLCSet.nFilterType == 1)
				s_dData[SingleSide] = s_hvFilterData.TupleMean().D();
			else if (m_tPLCSet.nFilterType == 2)
				s_dData[SingleSide] = s_hvFilterData.TupleMedian().D();
			else
				s_dData[SingleSide] = s_hvFilterData[s_hvFilterData.Length() - 1].D();

			/*if (s_hvFilterData.TupleDeviation().D() > 0.2)
			{
				TupleRemove(s_hvFilterData, 0, &s_hvFilterData);
				strLog.Format("SCIPLC_单面闭环: 标准差大于0.2，不执行闭环");
				Log(strLog, DebugLog);
				return FALSE;
			}*/

			if (m_tPLCSet.nCorrectDirection == 1)
				s_dData[SingleSide] = -s_dData[SingleSide];

			static double s_dBigDiffMeterCount = 0;
			if (abs(s_dData[SingleSide]) > 0.3)
				s_dBigDiffMeterCount += hv_Height.D();
			else
				s_dBigDiffMeterCount = 0;
			if (s_dBigDiffMeterCount > 15)
			{
				s_dBigDiffMeterCount = 0;
				CString strLog;
				strLog.Format("单面闭环: 纠偏差值连续%0.2fm超过±0.3mm，请检查闭环和模头是否正常工作", 15);
				m_sInit.Log(strLog, AlarmLog, NoExcp);
			}

			double dDiffTmp = s_dData[SingleSide] * m_tPLCSet.dCorrectPercent;

			if (abs(dDiffTmp) <= m_tPLCSet.dAlertDiff)//极耳宽度差值/2小于报警差值
			{
				_SendCorrectValue1(dDiffTmp);
				s_dDelayDistance[SingleSide] = -hv_Height.D();
				s_dDelayCamDistanceCount = 0;
				s_hvFilterData.Clear();
				//如果纠偏差值小于0.15，就使用长延迟周期，目的是减少纠偏频率，以减少波动
				if (abs(s_dData[SingleSide]) < 0.15)
					s_dDelayInterval = m_tPLCSet.dCorrectIntervalLong;
				else
					s_dDelayInterval = m_tPLCSet.dCorrectInterval;

				strLog.Format("SCIPLC_单面闭环: 纠偏差值:%0.3fmm, 闭环值:%0.3fmm, 下次闭环周期:%0.3fm", m_dDiffValue, dDiffTmp, s_dDelayInterval);
				Log(strLog, DebugLog);
			}
			else
			{
				//偏差大于警戒值，不纠偏，报警处理
				strLog.Format("SCIPLC_Correcting2：闭环值(%0.3f)大于警戒值(+-%.3f),不执行闭环", dDiffTmp, m_tPLCSet.dAlertDiff);
				Log(strLog, DebugLog);
			}
		}
	}
	catch (HException&e)
	{
		strLog.Format("SCIPLC_Correcting2：单面闭环错位异常, %s(%s)", e.ErrorMessage().Text(), e.ProcName().Text());
		Log(strLog, ExcpLog);
	}
#pragma endregion

#pragma region 双面纠偏逻辑
	try
	{
		if (m_sInit.tPdInfo.nCoatingType == TwoSide/* && m_tPLCSet.nCorrectType == DoubleSide*/ && TRUE == Output.vecB[0].tMeasure.hv_HasCoat && TRUE == Output.vecB[1].tMeasure.hv_HasCoat)
		{
			static double nNGMeterCount = 0;
			if (!_CalCorrectDiff(Output, hv_CorrectValue))
			{
				if (Output.vecB[0].tMeasure.hv_HasCoat.I()
					&& Output.vecB[1].tMeasure.hv_HasCoat.I())
				{
					nNGMeterCount += hv_Height.D();
					if (nNGMeterCount > 10)
					{
						nNGMeterCount = 0;
						CString strLog;
						strLog.Format("双面闭环: 连续%0.2fm含有无效测量值，闭环未工作，请检查", 10);
						m_sInit.Log(strLog, AlarmLog, NoExcp);
					}
				}
				else
				{
					nNGMeterCount = 0;
				}
				return FALSE;
			}
			else
			{
				nNGMeterCount = 0;
			}

			m_dDiffValue = hv_CorrectValue.D();

			if (_JudgeGuideFinish())
			{
				s_dDelayCamDistanceCount += hv_Height.D();
				if (s_dDelayCamDistanceCount < m_tPLCSet.dCamDistance)
				{
					strLog.Format("SCIPLC_双面闭环: 未达到相机到模头设定距离(%0.3fm)，返回", s_dDelayCamDistanceCount);
					Log(strLog, DebugLog);
					return FALSE;
				}
			}
			else
			{
				//模头未执行到位，重新记米
				strLog.Format("SCIPLC_双面闭环: 模头未执行到位，重新记录延迟距离");
				Log(strLog, DebugLog);
				s_dDelayDistance[DoubleSide] = -hv_Height.D();
				s_dDelayCamDistanceCount = 0;
				s_hvFilterData.Clear();
				return FALSE;
			}

			s_hvFilterData.Append(m_dDiffValue);
			s_dDelayDistance[DoubleSide] += hv_Height.D();
			if (s_dDelayDistance[DoubleSide] < s_dDelayInterval)
			{
				strLog.Format("SCIPLC_双面闭环: 未达到设定纠偏延迟距离(%0.3fm)，返回", s_dDelayDistance[DoubleSide]);
				Log(strLog, DebugLog);
				return FALSE;
			}

			if (m_tPLCSet.nFilterType == 1)
				s_dData[DoubleSide] = s_hvFilterData.TupleMean().D();
			else if (m_tPLCSet.nFilterType == 2)
				s_dData[DoubleSide] = s_hvFilterData.TupleMedian().D();
			else
				s_dData[DoubleSide] = s_hvFilterData[s_hvFilterData.Length() - 1].D();

			/*if (s_hvFilterData.TupleDeviation().D() > 0.2)
			{
				TupleRemove(s_hvFilterData, 0, &s_hvFilterData);
				strLog.Format("SCIPLC_双面闭环: 标准差大于0.2，不执行闭环");
				Log(strLog, DebugLog);
				return FALSE;
			}*/

			if (m_tPLCSet.nCorrectDirection == 1)
				s_dData[DoubleSide] = -s_dData[DoubleSide];

			double dDiffTmp2 = s_dData[DoubleSide] * m_tPLCSet.dCorrectPercent;

			static double s_dBigDiffMeterCount = 0;
			if (abs(s_dData[DoubleSide]) > 0.3)
				s_dBigDiffMeterCount += hv_Height.D();
			else
				s_dBigDiffMeterCount = 0;
			if (s_dBigDiffMeterCount > 15)
			{
				s_dBigDiffMeterCount = 0;
				CString strLog;
				strLog.Format("双面闭环: 纠偏差值连续%0.2fm超过±0.3mm，请检查闭环和模头是否正常工作", 15);
				m_sInit.Log(strLog, AlarmLog, NoExcp);
			}
			
			if (abs(dDiffTmp2) <= m_tPLCSet.dAlertDiff)//极耳宽度差值/2小于报警差值
			{
				_SendCorrectValue2(dDiffTmp2);
				s_dDelayDistance[DoubleSide] = -hv_Height.D();
				s_dDelayCamDistanceCount = 0;
				s_hvFilterData.Clear();
				if (abs(s_dData[DoubleSide]) < 0.1)
					s_dDelayInterval = m_tPLCSet.dCorrectIntervalLong;
				else
					s_dDelayInterval = m_tPLCSet.dCorrectInterval;

				strLog.Format("SCIPLC_双面闭环: 纠偏差值:%0.3fmm, 闭环值:%0.3fmm, 下次闭环周期:%0.3fm", m_dDiffValue, dDiffTmp2, s_dDelayInterval);
				Log(strLog, DebugLog);
			}
			else
			{
				//偏差大于警戒值，不纠偏，报警处理
				strLog.Format("SCIPLC_Correcting2：闭环值(%0.3f)大于警戒值(+-%.3f),不执行闭环", dDiffTmp2, m_tPLCSet.dAlertDiff);
				Log(strLog, DebugLog);
			}
		}
	}
	catch (HException&e)
	{
		strLog.Format("SCIPLC_Correcting2：双面闭环错位异常, %s(%s)", e.ErrorMessage().Text(), e.ProcName().Text());
		Log(strLog, ExcpLog);
	}
#pragma endregion

	return TRUE;
}

BOOL CPLC::_IsDataDeviationBig(const int& nIndex, const double& dStdevLimit)
{
	try
	{
		HTuple hv_DataTmp = m_hvec_Data[nIndex].T();
		if (hv_DataTmp.Length() < 2) return FALSE;
		HTuple hvStdev;
		TupleDeviation(hv_DataTmp, &hvStdev);
		if (hvStdev.D() > abs(dStdevLimit)) return TRUE;
		else return FALSE;
	}
	catch (HException&)
	{
		return FALSE;
	}
	
}

BOOL CPLC::_CalCorrectDiff(C_MergeWork &Output, HTuple& hvDiff)
{
	hvDiff = 0;
	CString strLog;
	HTuple hv_CorrectTemp, hv_CorrectValue;
	if (m_tPLCSet.nCorrectLogicType == 0)
	{
		//求标准差的逻辑,W21用
		if ((m_sInit.tPdInfo.nCoatingType == OneSide || m_tPLCSet.nCorrectType == SingleSide) && TRUE == Output.vecB[0].tMeasure.hv_HasCoat.I())
		{
			if (m_sInit.tPdInfo.bExistAT11)
			{
				//有at11,at11会跟着涂膜左右移动，用极耳的尺寸减去极耳平均值，计算平方和最大的移动步数
				HTuple hv_FindResult0 = Output.vecB[0].tMeasure.tFoil.hv_Width.TupleFind(0);
				HTuple hv_FindResultNoData = Output.vecB[0].tMeasure.tFoil.hv_Width.TupleFind(NO_DATA);
				if (Output.vecB[0].tMeasure.tFoil.hv_Width.Length() == 0 || (hv_FindResult0 != -1 && hv_FindResult0.Length() == 0)
					|| (hv_FindResultNoData != -1 && hv_FindResultNoData.Length() == 0))
				{
					strLog.Format("SCIPLC_Correcting2：极耳值无效或者没找到边,不执行闭环");
					Log(strLog, DebugLog);
					return FALSE;
				}
				hv_FindResult0 = Output.vecB[0].tMeasure.tCoat.hv_Width.TupleFind(0);
				hv_FindResultNoData = Output.vecB[0].tMeasure.tCoat.hv_Width.TupleFind(NO_DATA);
				if (Output.vecB[0].tMeasure.tCoat.hv_Width.Length() == 0 || (hv_FindResult0 != -1 && hv_FindResult0.Length() == 0)
					|| (hv_FindResultNoData != -1 && hv_FindResultNoData.Length() == 0))
				{
					strLog.Format("SCIPLC_Correcting2：涂布值无效或者没找到边,不执行闭环");
					Log(strLog, DebugLog);
					return FALSE;
				}
				//求极耳平均值
				HTuple hvMean;
				int nNum = 2;
				if (Output.vecB[0].tMeasure.tFoil.hv_Width.Length() > 2)
					nNum = 2 + (Output.vecB[0].tMeasure.tFoil.hv_Width.Length() - 1) * 2;
				hvMean = Output.vecB[0].tMeasure.tFoil.hv_Width.TupleSum().D() / (double)nNum;

				for (int i = 0; i < Output.vecB[0].tMeasure.tFoil.hv_Width.Length(); i++)
				{
					if (i == 0 || i == Output.vecB[0].tMeasure.tFoil.hv_Width.Length() - 1)
						hv_CorrectTemp = Output.vecB[0].tMeasure.tFoil.hv_Width[i].D() - hvMean;
					else
						hv_CorrectTemp = Output.vecB[0].tMeasure.tFoil.hv_Width[i].D() / 2 - hvMean;
					hv_CorrectValue.Append(hv_CorrectTemp);
				}
			}
			else if (m_sInit.tPdInfo.bExistBaseCoat)
			{
				//有凹版，凹版是固定在基材上的，涂膜要和凹版中心对齐
				HTuple hv_FindResult0 = Output.vecB[0].tMeasure.tBaseCoat.hv_Width.TupleFind(0);
				HTuple hv_FindResultNoData = Output.vecB[0].tMeasure.tBaseCoat.hv_Width.TupleFind(NO_DATA);
				if (hv_FindResultNoData != -1 || hv_FindResultNoData.Length() == 0)
				{
					strLog.Format("SCIPLC_Correcting2：漏凹版值无效或者没找到边,不执行闭环");
					Log(strLog, DebugLog);
					return FALSE;
				}
				//2021.3.24，增加一种情况，判断凹版不为0的个数如果小于等于4个，就认为都可以闭环，包含涂膜覆盖凹版的时候
				HTuple hvMod;
				TupleMod(hv_FindResult0, 2, &hvMod);
				//正常覆盖凹版的时候，找到0的数都是纯奇数或者纯偶数
				if (hv_FindResultNoData != -1 || hv_FindResult0.Length() > 4)
				{
					strLog.Format("SCIPLC_Correcting2：漏凹版值没找到的边过多,不执行闭环");
					Log(strLog, DebugLog);
					return FALSE;
				}

				for (int i = 0; i < Output.vecB[0].tMeasure.tBaseCoat.hv_Width.Length() / 2; i++)
				{
					hv_CorrectTemp = (Output.vecB[0].tMeasure.tBaseCoat.hv_Width[2 * i] - Output.vecB[0].tMeasure.tBaseCoat.hv_Width[2 * i + 1]) / 2;
					hv_CorrectValue.Append(hv_CorrectTemp);
				}
			}
			else
			{
				//只有极耳，用涂膜中心偏差，计算平方和最大时的移动量
				HTuple hv_FindResult0 = Output.vecB[0].tMeasure.tFoil.hv_Width.TupleFind(0);
				HTuple hv_FindResultNoData = Output.vecB[0].tMeasure.tFoil.hv_Width.TupleFind(NO_DATA);

				if (Output.vecB[0].tMeasure.tFoil.hv_Width.Length() == 0 || (hv_FindResult0 != -1 && hv_FindResult0.Length() == 0)
					|| (hv_FindResultNoData != -1 && hv_FindResultNoData.Length() == 0))
				{
					strLog.Format("SCIPLC_Correcting2：极耳值无效或者没找到边,不执行闭环");
					Log(strLog, DebugLog);
					return FALSE;
				}
				hv_FindResult0 = Output.vecB[0].tMeasure.tCoat.hv_Width.TupleFind(0);
				hv_FindResultNoData = Output.vecB[0].tMeasure.tCoat.hv_Width.TupleFind(NO_DATA);

				if (Output.vecB[0].tMeasure.tCoat.hv_Width.Length() == 0 || (hv_FindResult0 != -1 && hv_FindResult0.Length() == 0)
					|| (hv_FindResultNoData != -1 && hv_FindResultNoData.Length() == 0))
				{
					strLog.Format("SCIPLC_Correcting2：涂布值无效或者没找到边,不执行闭环");
					Log(strLog, DebugLog);
					return FALSE;
				}

				//2021.4.15 修改为两边外面涂膜中心偏差
				HTuple hvWidth1, hvWidth2;
				hvWidth1 = Output.vecB[0].tMeasure.tFoil.hv_Width[0] + Output.vecB[0].tMeasure.tCoat.hv_Width[0] / 2;
				hvWidth2 = Output.vecB[0].tMeasure.tFoil.hv_Width[Output.vecB[0].tMeasure.tFoil.hv_Width.Length() - 1] +
					Output.vecB[0].tMeasure.tCoat.hv_Width[Output.vecB[0].tMeasure.tCoat.hv_Width.Length() - 1] / 2;
				hv_CorrectValue = hvWidth1 - hvWidth2;
			}
		}
		else if (m_sInit.tPdInfo.nCoatingType == TwoSide && m_tPLCSet.nCorrectType == DoubleSide && TRUE == Output.vecB[0].tMeasure.hv_HasCoat.I() && TRUE == Output.vecB[1].tMeasure.hv_HasCoat.I())
		{
			HTuple hv_FindResultNoData = Output.tMerge.hv_Misalignment.TupleFind(NO_DATA);
			if (hv_FindResultNoData != -1 && hv_FindResultNoData.Length() > 0 || Output.tMerge.hv_Misalignment.Length() == 0)
			{
				strLog.Format("SCIPLC_Correcting2：错位值无效,不执行闭环");
				return FALSE;
			}

			//错位值取均值,求标准差最小的时候做均值
			hv_CorrectValue = Output.tMerge.hv_Misalignment.TupleMean().D();
		}
		else
		{
			return FALSE;
		}

		hvDiff = hv_CorrectValue.TupleMean().D();
	}
	else if (m_tPLCSet.nCorrectLogicType == 1)
	{
		//均值逻辑，W3用
		if ((m_sInit.tPdInfo.nCoatingType == OneSide || m_tPLCSet.nCorrectType == SingleSide) && TRUE == Output.vecB[0].tMeasure.hv_HasCoat.I())
		{
			if (m_sInit.tPdInfo.bExistBaseCoat)
			{
				//有凹版，凹版是固定在基材上的，涂膜要和凹版中心对齐
				HTuple hv_FindResult0 = Output.vecB[0].tMeasure.tBaseCoat.hv_Width.TupleFind(0);
				HTuple hv_FindResultNoData = Output.vecB[0].tMeasure.tBaseCoat.hv_Width.TupleFind(NO_DATA);
				if (hv_FindResultNoData != -1 || hv_FindResultNoData.Length() == 0)
				{
					strLog.Format("SCIPLC_Correcting2：漏凹版值无效或者没找到边,不执行闭环");
					Log(strLog, DebugLog);
					return FALSE;
				}
				//2021.3.24，增加一种情况，判断凹版不为0的个数如果小于等于4个，就认为都可以闭环，包含涂膜覆盖凹版的时候
				HTuple hvMod;
				TupleMod(hv_FindResult0, 2, &hvMod);
				//正常覆盖凹版的时候，找到0的数都是纯奇数或者纯偶数
				if (hv_FindResultNoData != -1 || hv_FindResult0.Length() > 4)
				{
					strLog.Format("SCIPLC_Correcting2：漏凹版值没找到的边过多,不执行闭环");
					Log(strLog, DebugLog);
					return FALSE;
				}

				for (int i = 0; i < Output.vecB[0].tMeasure.tBaseCoat.hv_Width.Length() / 2; i++)
				{
					hv_CorrectTemp = (Output.vecB[0].tMeasure.tBaseCoat.hv_Width[2 * i] - Output.vecB[0].tMeasure.tBaseCoat.hv_Width[2 * i + 1]) / 2;
					hv_CorrectValue.Append(hv_CorrectTemp);
				}
			}
			else
			{
				//无凹版，使用左右极耳对称的逻辑
				int nLength = Output.vecB[0].tMeasure.tFoil.hv_Width.Length();
				if (Output.vecB[0].tMeasure.tFoil.hv_Width.D() == NO_DATA || Output.vecB[0].tMeasure.tFoil.hv_Width[0].D() == 0
					|| Output.vecB[0].tMeasure.tFoil.hv_Width[nLength - 1].D() == 0 || Output.vecB[0].tMeasure.tFoil.hv_Width[nLength - 1].D() == NO_DATA)
				{
					strLog.Format("SCIPLC_Correcting2：极耳值无效或者没找到边,不执行闭环");
					return FALSE;
				}
				hv_CorrectValue = (Output.vecB[0].tMeasure.tFoil.hv_Width[0].D() - Output.vecB[0].tMeasure.tFoil.hv_Width[nLength - 1].D()) / 2;	//正面值
			}
			
		}
		else if (m_sInit.tPdInfo.nCoatingType == TwoSide && m_tPLCSet.nCorrectType == DoubleSide && TRUE == Output.vecB[0].tMeasure.hv_HasCoat.I() && TRUE == Output.vecB[1].tMeasure.hv_HasCoat.I())
		{
			HTuple hv_FindResultNoData = Output.tMerge.hv_Misalignment.TupleFind(NO_DATA);
			if (hv_FindResultNoData != -1 && hv_FindResultNoData.Length() > 0 || Output.tMerge.hv_Misalignment.Length() == 0)
			{
				strLog.Format("SCIPLC_Correcting2：错位值无效,不执行闭环");
				return FALSE;
			}

			//错位值取均值,求标准差最小的时候做均值
			hv_CorrectValue = Output.tMerge.hv_Misalignment.TupleMean().D();
		}
		else
		{
			return FALSE;
		}

		hvDiff = hv_CorrectValue.TupleMean().D();
	}
}

/************************************
*@Method:    FilterData
*@Access:    private
*@Returns:   double
*@Author: 	 Fowindy
*@Parameter: const int & nIndex
*@Parameter: const double & dData
*@Created:   2021/02/19 21:09
*@Describe:	 数据滤波
*************************************/
double CPLC::FilterData(const int& nIndex, const double& dData)
{
	double dRet = 0;
	HTuple hv_DataTmp = m_hvec_Data[nIndex].T();
	if (hv_DataTmp.Length() >= m_tPLCSet.nFilterSize)
	{
		TupleSelectRange(hv_DataTmp, 1, hv_DataTmp.Length() - 1, &hv_DataTmp);
		hv_DataTmp.Append(dData);
	}
	else
	{
		hv_DataTmp.Append(dData);
	}
	m_hvec_Data[nIndex].T() = hv_DataTmp;//原始数据保存
										 
	if (1 == m_tPLCSet.nFilterType)
	{
		//均值滤波
		dRet = hv_DataTmp.TupleMean();
	}
	else if (2 == m_tPLCSet.nFilterType)
	{
		//中值滤波
		dRet = hv_DataTmp.TupleMedian().D();
	}
	else
	{
		//不滤波
		dRet = dData;
	}
	return dRet;
}

/************************************
*@Method:    SendHeartBeat
*@Access:    private
*@Returns:   int
*@Author: 	 Fowindy
*@Parameter: const int& nStatus
*@Created:   2021/03/09 18:15
*@Describe:	 向PLC发送心跳信号
*************************************/
int CPLC::SendHeartBeat(const int& nStatus)
{
	BOOL bRet = FALSE;
	DoSomething("GetConnection", NULL, &bRet);
	CString strMess;
	if (CPLC::FinsTCP == m_tPLCSet.nUsePlc && bRet && m_tPLCSet.nPlcHeartBeatAddress>0)
	{
		bRet = m_pFinsTcp->WriteInt32(m_tPLCSet.nPlcHeartBeatAddress, nStatus);
		if (!bRet)
		{
			strMess.Format("SCIPLC_SendHeartBeat:心跳信号写入失败，请检查!");
			Log(strMess, ExcpLog);
			return FALSE;
		}
		return TRUE;
	}
	return FALSE;
}

int CPLC::SendCCDEnable(const int& nStatus)
{
	BOOL bRet = FALSE;
	DoSomething("GetConnection", NULL, &bRet);
	CString strMess;
	if (CPLC::FinsTCP == m_tPLCSet.nUsePlc && bRet && m_tPLCSet.nPlcCCDEnableAddress>0)
	{
		bRet = m_pFinsTcp->WriteInt32(m_tPLCSet.nPlcCCDEnableAddress, nStatus);
		if (!bRet)
		{
			strMess.Format("SCIPLC_SendCCDEnable:视觉启用信号写入失败，请检查!");
			Log(strMess, ExcpLog);
			return FALSE;
		}
		return TRUE;
	}
	return FALSE;
}

int CPLC::SendAlarm(const int& nAlarmCode)
{
	BOOL bRet = FALSE;
	DoSomething("GetConnection", NULL, &bRet);
	CString strMess;
	if (CPLC::FinsTCP == m_tPLCSet.nUsePlc && bRet && m_tPLCSet.nPlcAlarmAddress>0)
	{
		bRet = m_pFinsTcp->WriteInt32(m_tPLCSet.nPlcAlarmAddress, nAlarmCode);
		if (!bRet)
		{
			strMess.Format("SCIPLC_SendAlarm:报警信号写入失败，请检查!");
			Log(strMess, ExcpLog);
			return FALSE;
		}
		return TRUE;
	}
	return FALSE;
}

int CPLC::GetPLCRunDirection(int& nDirection)
{
	BOOL bRet = FALSE;
	DoSomething("GetConnection", NULL, &bRet);
	nDirection = 0;
	CString strMess;
	if (CPLC::FinsTCP == m_tPLCSet.nUsePlc && bRet && m_tPLCSet.nPlcRunDirectionAddress>0)
	{
		bRet = m_pFinsTcp->ReadInt32(m_tPLCSet.nPlcRunDirectionAddress, nDirection);
		if (!bRet)
		{
			strMess.Format("SCIPLC_GetPLCRunDirection:设备运动方向获取失败，请检查!");
			Log(strMess, ExcpLog);
			return FALSE;
		}
		return TRUE;
	}
	return FALSE;
}

int CPLC::GetPLCDieHeadDirection(int& nDirection)
{
	BOOL bRet = FALSE;
	DoSomething("GetConnection", NULL, &bRet);
	nDirection = 0;
	CString strMess;
	if (CPLC::FinsTCP == m_tPLCSet.nUsePlc && bRet && m_tPLCSet.nPlcDieHeadDirectionAddress>0)
	{
		bRet = m_pFinsTcp->ReadInt32(m_tPLCSet.nPlcDieHeadDirectionAddress, nDirection);
		if (!bRet)
		{
			strMess.Format("SCIPLC_GetPLCDieHeadDirection:设备模头进退刀方向获取失败，请检查!");
			Log(strMess, ExcpLog);
			return FALSE;
		}
		return TRUE;
	}
	return FALSE;
}

int CPLC::GetPLCStartCoat(int& nStartCoat)
{
	BOOL bRet = FALSE;
	DoSomething("GetConnection", NULL, &bRet);
	nStartCoat = 0;
	CString strMess;
	if (CPLC::FinsTCP == m_tPLCSet.nUsePlc && bRet && m_tPLCSet.nPlcStartCoatAddress>0)
	{
		bRet = m_pFinsTcp->ReadInt32(m_tPLCSet.nPlcStartCoatAddress, nStartCoat);
		if (!bRet)
		{
			strMess.Format("SCIPLC_GetPLCDieHeadDirection:设备模头起涂状态获取失败，请检查!");
			Log(strMess, ExcpLog);
			return FALSE;
		}
		return TRUE;
	}
	return FALSE;
}

int CPLC::GetPLCDieHeadPosition(double& dPos)
{
	BOOL bRet = FALSE;
	DoSomething("GetConnection", NULL, &bRet);
	float fPos = 0;
	CString strMess;
	if (CPLC::FinsTCP == m_tPLCSet.nUsePlc && bRet && m_tPLCSet.nPlcDieHeadPosAddress>0)
	{
		bRet = m_pFinsTcp->ReadFloat32(m_tPLCSet.nPlcDieHeadPosAddress, fPos);
		dPos = fPos;
		if (!bRet)
		{
			strMess.Format("SCIPLC_GetPLCDieHeadDirection:设备模头方向获取失败，请检查!");
			Log(strMess, ExcpLog);
			return FALSE;
		}
		return TRUE;
	}
	return FALSE;
}

/************************************
*@Method:    CalCorrect
*@Access:    private
*@Returns:   BOOL
*@Author: 	 Fowindy
*@Parameter: HTuple hvOriginal,double& dDstVal
*@Created:   2021/03/08 10:20
*@Describe:	 计算闭环纠偏
*************************************/
BOOL CPLC::CalCorrect(HTuple hvOriginal, double& dDstVal)
{
	static HTuple s_hvOldVal = 0;
	HTuple hv_tuple0, hv_tuple1, hv_max, hv_min, hv_num, hv_i, hv_d, hv_Square, hv_SquareSum, hv_Indices, hv_Selected, hv_Target;
	try
	{
		TupleMax(hvOriginal, &hv_max);
		TupleMin(hvOriginal, &hv_min);
		if (hv_max.D()-hv_min.D() < 0.05) return FALSE;
		TupleDiv(hv_max - hv_min, 0.05, &hv_num);
		HTuple end_val6 = hv_num;
		HTuple step_val6 = 1;
		for (hv_i = -hv_num; hv_i.Continue(end_val6, step_val6); hv_i += step_val6)
		{
			hv_d = hvOriginal - (0.05*hv_i);
			TupleMult(hv_d, hv_d, &hv_Square);
			TupleSum(hv_Square, &hv_SquareSum);
			hv_tuple0 = hv_tuple0.TupleConcat(0.05*hv_i);
			hv_tuple1 = hv_tuple1.TupleConcat(hv_SquareSum);
		}
		//**对分析数据进行排序,挑选最优
		TupleSortIndex(hv_tuple1, &hv_Indices);
		if (hv_Indices.Length() == 0 || hv_Indices == -1)
		{
			CString strMess;
			strMess.Format("SCIPLC_CalCorrect: 无合适的闭环值");
			Log(strMess, DebugLog);

			dDstVal = NO_DATA;
			return FALSE;
		}
		//**挑选最优纠偏量_Selected为纠偏量
		TupleSelect(hv_tuple0, HTuple(hv_Indices[0]), &hv_Selected);
		//****闭环之后的效果
		TupleSub(hvOriginal, hv_Selected, &hv_Target);
	}
	catch (HException&e)
	{
		CString strMess;
		strMess.Format("SCIPLC_CalCorrect: %s(%s)", e.ErrorMessage().Text(), e.ProcName().Text());
		Log(strMess, ExcpLog);
		dDstVal = NO_DATA;
	}
	dDstVal = hv_Selected.D();
	return TRUE;
}

BOOL CPLC::GetBatchName(CString& strBatch)
{
	BOOL bRet = FALSE;
	DoSomething("GetConnection", NULL, &bRet);
	CString strMess,strBatchName;
	strBatch = "None";
	if (CPLC::FinsTCP == m_tPLCSet.nUsePlc && bRet && m_tPLCSet.nPlcBatchNameAddress>0)
	{
		bRet = m_pFinsTcp->ReadStringBatchInverse(m_tPLCSet.nPlcBatchNameAddress, 16, strBatchName);
		if (!bRet)
		{
			strMess.Format("SCIPLC_GetBatchName: 读取膜卷号失败");
			Log(strMess, ExcpLog);
			return FALSE;
		}
		if (strBatchName == "") strBatchName = "None";
		strBatch = strBatchName;
		return TRUE;
	}
	return FALSE;
}

BOOL CPLC::WriteBatchNameVerify(CString& strBatch)
{
	BOOL bRet = FALSE;
	DoSomething("GetConnection", NULL, &bRet);
	CString strMess;
	if (CPLC::FinsTCP == m_tPLCSet.nUsePlc && bRet && m_tPLCSet.nPlcBatchNameVerifyAddress>0)
	{
		bRet = m_pFinsTcp->WriteStringBatchInverse(m_tPLCSet.nPlcBatchNameVerifyAddress, strBatch);
		if (!bRet)
		{
			strMess.Format("SCIPLC_WriteBatchNameVerify: 写入校验膜卷号失败");
			Log(strMess, ExcpLog);
			return FALSE;
		}
		return TRUE;
	}
	return FALSE;
}

BOOL CPLC::_SendCorrectValue1(double& dValue)
{
	CString strLog;
	static double s_dOldData = 0;

	if (s_dOldData == dValue && dValue != 0) dValue += 0.001;
	m_dCorrectValue = dValue;
	if (s_dOldData != dValue) s_dOldData = dValue;
	//发送A面纠偏移动距离
	int nCorrDiff = TransInt(dValue, 3);
	if (m_pFinsTcp != NULL && m_pFinsTcp->IsConnected() && m_tPLCSet.nSingleCorrectAddress>0)
	{
		m_pFinsTcp->WriteInt32(m_tPLCSet.nSingleCorrectAddress, nCorrDiff);//W3地址10586;W21地址:15086
		strLog.Format("SCIPLC_给PLC发的数值：%d,发送PLC成功!", nCorrDiff);
		Log(strLog, DebugLog);
		if (m_tPLCSet.nDoCorrectAddress > 0)
		{
			m_pFinsTcp->WriteInt32(m_tPLCSet.nDoCorrectAddress, 1);//让PLC执行闭环，嘉拓用
		}
		return TRUE;
	}
	return FALSE;
}

BOOL CPLC::_SendCorrectValue2(double& dValue)
{
	CString strLog;
	static double s_dOldData = 0;

	if (s_dOldData == dValue && dValue != 0) dValue += 0.001;
	m_dCorrectValue = dValue;
	if (s_dOldData != dValue) s_dOldData = dValue;
	//发送A面纠偏移动距离
	int nCorrDiff = TransInt(dValue, 3);
	if (m_pFinsTcp != NULL && m_pFinsTcp->IsConnected() && m_tPLCSet.nDoubleCorrectAddress>0)
	{
		m_pFinsTcp->WriteInt32(m_tPLCSet.nDoubleCorrectAddress, nCorrDiff);//W3地址10586;W21地址:15086
		strLog.Format("SCIPLC_给PLC发的数值：%d,发送PLC成功!", nCorrDiff);
		Log(strLog, DebugLog);
		if (m_tPLCSet.nDoCorrectAddress > 0)
		{
			m_pFinsTcp->WriteInt32(m_tPLCSet.nDoCorrectAddress, 1);//让PLC执行闭环，嘉拓用
		}
		return TRUE;
	}
	return FALSE;
}

BOOL CPLC::_JudgeGuideFinish()
{
	CString strLog;
	int nFinishSignal = 2;
	if (m_tPLCSet.bJudgeGuidFinish)
	{
		if (m_pFinsTcp != NULL && m_pFinsTcp->IsConnected() && m_tPLCSet.nGuidFinishAddr > 0)
		{
			m_pFinsTcp->ReadInt32(m_tPLCSet.nGuidFinishAddr, nFinishSignal);//纠偏完成标志位地址：10588
		}
	}

	if (nFinishSignal != 2)
	{
		strLog.Format("SCIPLC：纠偏到位标志位不为2, 模头电机未执行到位");
		Log(strLog, DebugLog);
		return FALSE;
	}
	return TRUE;
}