#include "StdAfx.h"
#include <math.h>
#include "DlgParamSet.h"
#include "BetaRay.in.h"
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
	m_hvKnifeDiffValue = HTuple(0).Append(0);
	m_hvKnifeCorrectValue = HTuple(0).Append(0);
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
	else if ("GETPRODUCTTYPENAME" == strMessage || "GetProductTypeName" == strMessage)
	{
	return GetProductTypeName(*(CString*)lParam);
	}
	else if ("WRITEPRODUCTTYPEVERIFY" == strMessage || "WriteProductTypeVerify" == strMessage)
	{
	return WriteProductTypeVerify(*(CString*)wParam);
	}
	else if ("WRITESOFTWAREVERSIONVERIFY" == strMessage || "WriteSoftwareVersionVerify" == strMessage)
	{
	return WriteSoftwareVersionVerify(*(CString*)wParam);
	}
	else if ("CCDENABLE" == strMessage || "CCDEnable" == strMessage)
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
	else if ("GETPLCKNIFEDIFF" == strMessage || "GetPLCKnifeDiffValue" == strMessage)
	{
		(*(HTuple*)lParam) = m_hvKnifeDiffValue;
		return TRUE;
	}
	else if ("GETPLCKNIFECORRECTVALUE" == strMessage || "GetPLCKnifeCorrectValue" == strMessage)
	{
		(*(HTuple*)lParam) = m_hvKnifeCorrectValue;
		return TRUE;
	}
	else if ("PLCLEFTKNIFEPOSITION" == strMessage || "PLCLeftKnifePosition" == strMessage)
	{
		double dPos = 0;
		BOOL bRet = GetPLCLeftKnifePosition(dPos);
		if (bRet)
		{
			m_dCurrentLeftKnifePos = dPos;
			(*(double*)lParam) = dPos;
		}
		return bRet;
	}
	else if ("PLCRIGHTKNIFEPOSITION" == strMessage || "PLCRightKnifePosition" == strMessage)
	{
		double dPos = 0;
		BOOL bRet = GetPLCRightKnifePosition(dPos);
		if (bRet)
		{
			m_dCurrentRightKnifePos = dPos;
			(*(double*)lParam) = dPos;
		}
		return bRet;
	}
	else if ("PLCSTARTCOAT" == strMessage || "PlcStartCoat" == strMessage)
	{
		return GetPLCStartCoat(*(int*)lParam);
	}
	else if ("CCDALARM" == strMessage || "CCDAlarm" == strMessage)
	{
		return SendAlarm(*(int*)wParam);
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

	_JudgeKnifeWeightOK();

	//HTuple s_hvFilterData;

	//s_hvFilterData.Append(0.0195);
	//s_hvFilterData.Append(-0.0158);
	//s_hvFilterData.Append(0.018);
	//s_hvFilterData.Append(-0.0061);
	//s_hvFilterData.Append(0.0033);
	//s_hvFilterData.Append(0.0271);
	//s_hvFilterData.Append(0.0139);
	//s_hvFilterData.Append(0.0256);

	//if ((s_hvFilterData.Length()*0.395) < 3)
	//{
	//	return FALSE;
	//}

	////判定数据是否有效
	//HTuple hvData;
	//HTuple hvInc = s_hvFilterData.TupleFind(NO_DATA);
	//if (hvInc != -1 && hvInc.Length() > 1)
	//{
	//	TupleRemove(s_hvFilterData, 0, &s_hvFilterData);
	//	return FALSE;
	//}
	//if (hvInc != -1)
	//{
	//	//剔除无效值
	//	TupleRemove(s_hvFilterData, hvInc, &hvData);
	//}
	//else
	//{
	//	hvData = s_hvFilterData;
	//}

	//HTuple s_dData = hvData.TupleMedian().D();

	//CString strLog;
	//strLog.Format("SCIPLC_单面闭环: 计算纠偏差值中值为%0.4f,收集数据:,", s_dData);
	//for (int i = 0; i < hvData.Length(); i++)
	//{
	//	CString strTmp;
	//	strTmp.Format("%0.4f,", hvData[i].D());
	//	strLog += strTmp;
	//}
	//m_sInit.Log(strLog, DebugLog, NoExcp);



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
	SetMessageQueueParam(m_hvQueue, "max_message_num", 5);
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
	m_tPLCSet.nProductTypeAddress = GetPrivateProfileInt("Config", "ProductTypeAddress", 0, m_strConfigPLCFile);
	m_tPLCSet.nPlcSoftwareVersionAddress = GetPrivateProfileInt("Config", "PlcSoftwareVersionAddress", 0, m_strConfigPLCFile);
	m_tPLCSet.nReadProductTypeAddress = GetPrivateProfileInt("Config", "ReadProductTypeAddress", 0, m_strConfigPLCFile);
	
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

	//刀距闭环
	m_tPLCSet.nUseKnifeCorrect = GetPrivateProfileInt("Config", "UseKnifeCorrect", FALSE, m_strConfigPLCFile);
	m_tPLCSet.nPlcLeftKnifeCorrectAddress = GetPrivateProfileInt("Config", "PlcLeftKnifeCorrectAddress", 0, m_strConfigPLCFile);
	m_tPLCSet.nPlcRightKnifeCorrectAddress = GetPrivateProfileInt("Config", "PlcRightKnifeCorrectAddress", 0, m_strConfigPLCFile);
	m_tPLCSet.nPlcKnifeDoCorrectAddress = GetPrivateProfileInt("Config", "PlcKnifeDoCorrectAddress", 0, m_strConfigPLCFile);
	m_tPLCSet.nPlcLeftKnifePosAddress = GetPrivateProfileInt("Config", "PlcLeftKnifePosAddress", 0, m_strConfigPLCFile);
	m_tPLCSet.nPlcRightKnifePosAddress = GetPrivateProfileInt("Config", "PlcRightKnifePosAddress", 0, m_strConfigPLCFile);
	m_tPLCSet.bJudgeKnifeDoFinish = GetPrivateProfileInt("Config", "JudgeKnifeDoFinish", FALSE, m_strConfigPLCFile);
	m_tPLCSet.nPlcKnifeDoFinishAddress = GetPrivateProfileInt("Config", "PlcKnifeDoFinishAddress", 0, m_strConfigPLCFile);

	m_tPLCSet.nUseWeightData = GetPrivateProfileInt("Config", "UseWeightData", FALSE, m_strConfigPLCFile);
	m_tPLCSet.dKnifeAlertMaxDiff = GetPrivateProfileDouble("Config", "KnifeAlertMaxDiff", 3, m_strConfigPLCFile);
	m_tPLCSet.dKnifeAlertMinDiff = GetPrivateProfileDouble("Config", "KnifeAlertMinDiff", 0.5, m_strConfigPLCFile);
	m_tPLCSet.dUnitKnifeCoatWidth = GetPrivateProfileDouble("Config", "UnitKnifeCoatWidth", 0.07, m_strConfigPLCFile);
	m_tPLCSet.dKnifeCorrectPercent = GetPrivateProfileDouble("Config", "KnifeCorrectPercent", 0.9, m_strConfigPLCFile);
	m_tPLCSet.dCamKnifeDistance = GetPrivateProfileDouble("Config", "CamKnifeDistance", 3, m_strConfigPLCFile);
	m_tPLCSet.dKnifeCorrectInterval = GetPrivateProfileDouble("Config", "KnifeCorrectInterval", 10, m_strConfigPLCFile);
	m_tPLCSet.dKnifeStartIngoreDistance = GetPrivateProfileDouble("Config", "KnifeStartIngoreDistance", 90, m_strConfigPLCFile);
	m_tPLCSet.dBetaRayDistance = GetPrivateProfileDouble("Config", "BetaRayDistance", 95, m_strConfigPLCFile);
	m_tPLCSet.dBetaRayWeightDiffLimit = GetPrivateProfileDouble("Config", "BetaRayWeightDiffLimit", 2, m_strConfigPLCFile);
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
	WritePrivateProfileInt("Config", "ProductTypeAddress", m_tPLCSet.nProductTypeAddress, m_strConfigPLCFile);
	WritePrivateProfileInt("Config", "PlcSoftwareVersionAddress", m_tPLCSet.nPlcSoftwareVersionAddress, m_strConfigPLCFile);
	WritePrivateProfileInt("Config", "ReadProductTypeAddress", m_tPLCSet.nReadProductTypeAddress, m_strConfigPLCFile);

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

	//刀距闭环
	WritePrivateProfileInt("Config", "UseKnifeCorrect", m_tPLCSet.nUseKnifeCorrect, m_strConfigPLCFile);
	WritePrivateProfileInt("Config", "PlcLeftKnifeCorrectAddress", m_tPLCSet.nPlcLeftKnifeCorrectAddress, m_strConfigPLCFile);
	WritePrivateProfileInt("Config", "PlcRightKnifeCorrectAddress", m_tPLCSet.nPlcRightKnifeCorrectAddress, m_strConfigPLCFile);
	WritePrivateProfileInt("Config", "PlcKnifeDoCorrectAddress", m_tPLCSet.nPlcKnifeDoCorrectAddress, m_strConfigPLCFile);
	WritePrivateProfileInt("Config", "PlcLeftKnifePosAddress", m_tPLCSet.nPlcLeftKnifePosAddress, m_strConfigPLCFile);
	WritePrivateProfileInt("Config", "PlcRightKnifePosAddress", m_tPLCSet.nPlcRightKnifePosAddress, m_strConfigPLCFile);
	WritePrivateProfileInt("Config", "JudgeKnifeDoFinish", m_tPLCSet.bJudgeKnifeDoFinish, m_strConfigPLCFile);
	WritePrivateProfileInt("Config", "PlcKnifeDoFinishAddress", m_tPLCSet.nPlcKnifeDoFinishAddress, m_strConfigPLCFile);

	WritePrivateProfileInt("Config", "UseWeightData", m_tPLCSet.nUseWeightData, m_strConfigPLCFile);
	WritePrivateProfileDouble("Config", "KnifeAlertMaxDiff", m_tPLCSet.dKnifeAlertMaxDiff, m_strConfigPLCFile);
	WritePrivateProfileDouble("Config", "KnifeAlertMinDiff", m_tPLCSet.dKnifeAlertMinDiff, m_strConfigPLCFile);
	WritePrivateProfileDouble("Config", "UnitKnifeCoatWidth", m_tPLCSet.dUnitKnifeCoatWidth, m_strConfigPLCFile);
	WritePrivateProfileDouble("Config", "CamKnifeDistance", m_tPLCSet.dCamKnifeDistance, m_strConfigPLCFile);
	WritePrivateProfileDouble("Config", "KnifeCorrectPercent", m_tPLCSet.dKnifeCorrectPercent, m_strConfigPLCFile);
	WritePrivateProfileDouble("Config", "KnifeCorrectInterval", m_tPLCSet.dKnifeCorrectInterval, m_strConfigPLCFile);
	WritePrivateProfileDouble("Config", "KnifeStartIngoreDistance", m_tPLCSet.dKnifeStartIngoreDistance, m_strConfigPLCFile);
	WritePrivateProfileDouble("Config", "BetaRayDistance", m_tPLCSet.dBetaRayDistance, m_strConfigPLCFile);
	WritePrivateProfileDouble("Config", "BetaRayWeightDiffLimit", m_tPLCSet.dBetaRayWeightDiffLimit, m_strConfigPLCFile);
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
			if (m_tPLCSet.nUseKnifeCorrect)
			{
				KnifeCorrecting(Output);//刀距闭环逻辑，W21
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
		if (H_ERR_MQOVL == e.ErrorCode())
		{
			SetMessageQueueParam(m_hvQueue, "flush_queue", 1);
			CString strException;
			strException.Format("SCIPLC_PushMessage: 队列已满,已清空队列");
			Log(strException, ExcpLog);
		}
		else
		{
			CString strException;
			strException.Format("SCIPLC_PushMessage: %s(%s)", e.ErrorMessage().Text(), e.ProcName().Text());
			Log(strException, ExcpLog);
		}


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
		BOOL bRet = m_pFinsTcp->ReadInt32(nDMAddr, nReadVal);
		if (!bRet)
		{
			CString strLog;
			strLog.Format("CPLC_ReadInt32: 读取值(%d)失败,请检查", nReadVal);
			Log(strLog, ExcpLog, SlightExcp);
		}
		return bRet;
	}
	return FALSE;
}

BOOL CPLC::WriteInt32(const int& nDMAddr, const int& nWriteVal)
{
	if (FinsTCP == m_tPLCSet.nUsePlc)
	{
		BOOL bRet = m_pFinsTcp->WriteInt32(nDMAddr, nWriteVal);
		if (!bRet)
		{
			CString strLog;
			strLog.Format("CPLC_WriteInt32: 写入值(%d)失败,请检查", nWriteVal);
			Log(strLog, ExcpLog, SlightExcp);
		}
		return bRet;
	}
	return FALSE;
}

BOOL CPLC::ReadString(const int& nDMAddrStart, CString& strReadVal)
{
	if (FinsTCP == m_tPLCSet.nUsePlc)
	{
		BOOL bRet = m_pFinsTcp->ReadStringBatch(nDMAddrStart, 80, strReadVal);
		if (!bRet)
		{
			CString strLog;
			strLog.Format("CPLC_ReadString: 读取值(%s)失败,请检查", strReadVal);
			Log(strLog, ExcpLog, SlightExcp);
		}
		return bRet;
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
		BOOL bRet = m_pFinsTcp->ReadFloat32(nDMAddr, fReadVal);
		if (!bRet)
		{
			CString strLog;
			strLog.Format("CPLC_ReadFloat32: 读取值(%f)失败,请检查", fReadVal);
			Log(strLog, ExcpLog, SlightExcp);
		}
		return bRet;
	}
	return FALSE;
}

BOOL CPLC::WriteFloat32(const int& nDMAddr, const float& fWriteVal)
{
	if (FinsTCP == m_tPLCSet.nUsePlc)
	{
		BOOL bRet = m_pFinsTcp->WriteFloat32(nDMAddr, fWriteVal);
		if (!bRet)
		{
			CString strLog;
			strLog.Format("CPLC_WriteFloat32: 写入值(%f)失败,请检查", fWriteVal);
			Log(strLog, ExcpLog, SlightExcp);
		}
		return bRet;
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

	static double s_dDelayInterval = m_tPLCSet.dCorrectInterval;//设定延迟距离，用于动态调整延迟距离
	static double s_dDelayCamDistanceCount[2] = { 0,0 };
	//static double s_dDelayDistance[2] = { 0,0 };
	static double s_dData[2] = { 0,0 };
	static HTuple s_hvFilterData[2] = { HTuple(),HTuple() };

#pragma region 料带跑偏报警
	//基材位置
	static HTuple s_hvEdgeRealX;
	HTuple hvEdgeCol, hvEdgeColReal = HTuple();
	hvEdgeCol = (Output.vecB[0].tMeasure.tLine.hv_StartColumn[0].D() + Output.vecB[0].tMeasure.tLine.hv_EndColumn[0].D()) / 2;
	m_sInit.Main(MT_PLC, "CalColPoint", "Side0", &hvEdgeCol, &hvEdgeColReal);
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
						strLog.Format("单面闭环: 连续%0.2fm含有无效测量值，闭环未工作，请检查", 10.0);
						m_sInit.Log(strLog, AlarmLog, NoExcp);
					}
				}
				else
				{
					nNGMeterCount = 0;
				}

				//return FALSE;
			}
			else
			{
				nNGMeterCount = 0;
			}

			m_dDiffValue = hv_CorrectValue.D();

			if (_JudgeGuideFinish())
			{
				s_dDelayCamDistanceCount[SingleSide] += hv_Height.D();
				if (s_dDelayCamDistanceCount[SingleSide] < m_tPLCSet.dCamDistance)
				{
					strLog.Format("SCIPLC_单面闭环: 未达到相机到模头设定距离(%0.3fm)，返回", s_dDelayCamDistanceCount[SingleSide]);
					Log(strLog, DebugLog);
					return FALSE;
				}
			}
			else
			{
				//模头未执行到位，重新记米
				strLog.Format("SCIPLC_单面闭环: 模头未执行到位，重新记录延迟距离");
				Log(strLog, DebugLog);

				s_dDelayCamDistanceCount[SingleSide] = 0;
				return FALSE;
			}

			s_hvFilterData[SingleSide].Append(hv_CorrectValue.D());

			if ((s_hvFilterData[SingleSide].Length()*hv_Height.D()) < s_dDelayInterval)
			{
				strLog.Format("SCIPLC_单面闭环: 未达到设定纠偏延迟距离(%0.3fm)，返回", (s_hvFilterData[SingleSide].Length()*hv_Height.D()));
				Log(strLog, DebugLog);
				return FALSE;
			}


			//判定数据是否有效
			HTuple hvData;
			HTuple hvInc = s_hvFilterData[SingleSide].TupleFind(NO_DATA);
			if (hvInc != -1 && hvInc.Length() > 1)
			{
				TupleRemove(s_hvFilterData[SingleSide], 0, &s_hvFilterData[SingleSide]);
				strLog.Format("SCIPLC_单面闭环: 收集的数据中包含2以上的无效值，返回");
				Log(strLog, DebugLog);
				TupleRemove(s_hvFilterData[SingleSide], 0, &s_hvFilterData[SingleSide]);
				return FALSE;
			}
			if (hvInc != -1)
			{
				//剔除无效值
				TupleRemove(s_hvFilterData[SingleSide], hvInc, &hvData);
			}
			else
			{
				hvData = s_hvFilterData[SingleSide];
			}

			if (m_tPLCSet.nFilterType == 1)
			{
				s_dData[SingleSide] = hvData.TupleMean().D();
				CString strLog;
				strLog.Format("SCIPLC_单面闭环: 计算纠偏差值均值为%0.4f,收集数据:,", s_dData[SingleSide]);
				for (int i = 0; i < hvData.Length(); i++)
				{
					CString strTmp;
					strTmp.Format("%0.4f,", hvData[i].D());
					strLog += strTmp;
				}
				m_sInit.Log(strLog, DebugLog, NoExcp);
			}
			else if (m_tPLCSet.nFilterType == 2)
			{
				s_dData[SingleSide] = hvData.TupleMedian().D();
				CString strLog;
				strLog.Format("SCIPLC_单面闭环: 计算纠偏差值中值为%0.4f,收集数据:,", s_dData[SingleSide]);
				for (int i = 0; i < hvData.Length(); i++)
				{
					CString strTmp;
					strTmp.Format("%0.4f,", hvData[i].D());
					strLog += strTmp;
				}
				m_sInit.Log(strLog, DebugLog, NoExcp);
			}
			else
			{
				s_dData[SingleSide] = hvData[hvData.Length() - 1].D();
				CString strLog;
				strLog.Format("SCIPLC_单面闭环: 计算纠偏差值最新值为%0.4f,收集数据:,", s_dData[SingleSide]);
				for (int i = 0; i < hvData.Length(); i++)
				{
					CString strTmp;
					strTmp.Format("%0.4f,", hvData[i].D());
					strLog += strTmp;
				}
				m_sInit.Log(strLog, DebugLog, NoExcp);
			}

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
				strLog.Format("单面闭环: 纠偏差值连续%0.2fm超过±0.3mm，请检查闭环和模头是否正常工作", 15.0);
				m_sInit.Log(strLog, AlarmLog, NoExcp);
			}

			double dDiffTmp = s_dData[SingleSide] * m_tPLCSet.dCorrectPercent;

			if (abs(dDiffTmp) <= m_tPLCSet.dAlertDiff)//极耳宽度差值/2小于报警差值
			{
				_SendCorrectValue1(dDiffTmp);
				s_dDelayCamDistanceCount[SingleSide] = 0;
				s_hvFilterData[SingleSide].Clear();
				//如果纠偏差值小于0.15，就使用长延迟周期，目的是减少纠偏频率，以减少波动
				if (abs(s_dData[SingleSide]) < 0.15)
					s_dDelayInterval = m_tPLCSet.dCorrectIntervalLong;
				else
					s_dDelayInterval = m_tPLCSet.dCorrectInterval;

				strLog.Format("SCIPLC_单面闭环: 纠偏差值:%0.4fmm, 闭环值:%0.4fmm, 下次闭环周期:%0.3fm", s_dData[SingleSide], dDiffTmp, s_dDelayInterval + m_tPLCSet.dCamDistance);
				Log(strLog, DebugLog);
			}
			else
			{
				//偏差大于警戒值，不纠偏，报警处理
				strLog.Format("单面闭环值(%0.4f)超出上下限(+-%.4f),不执行闭环", dDiffTmp, m_tPLCSet.dAlertDiff);
				Log(strLog, AlarmLog);
				TupleRemove(s_hvFilterData[SingleSide], 0, &s_hvFilterData[SingleSide]);
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
						strLog.Format("双面闭环: 连续%0.2fm含有无效测量值，闭环未工作，请检查", 10.0);
						m_sInit.Log(strLog, AlarmLog, NoExcp);
					}
				}
				else
				{
					nNGMeterCount = 0;
				}
				//return FALSE;
			}
			else
			{
				nNGMeterCount = 0;
			}

			m_dDiffValue = hv_CorrectValue.D();

			if (_JudgeGuideFinish())
			{
				s_dDelayCamDistanceCount[DoubleSide] += hv_Height.D();
				if (s_dDelayCamDistanceCount[DoubleSide] < m_tPLCSet.dCamDistance)
				{
					strLog.Format("SCIPLC_双面闭环: 未达到相机到模头设定距离(%0.3fm)，返回", s_dDelayCamDistanceCount[DoubleSide]);
					Log(strLog, DebugLog);
					return FALSE;
				}
			}
			else
			{
				//模头未执行到位，重新记米
				strLog.Format("SCIPLC_双面闭环: 模头未执行到位，重新记录延迟距离");
				Log(strLog, DebugLog);
				s_dDelayCamDistanceCount[DoubleSide] = 0;
				s_hvFilterData[DoubleSide].Clear();
				return FALSE;
			}

			s_hvFilterData[DoubleSide].Append(hv_CorrectValue.D());

			if ((s_hvFilterData[DoubleSide].Length()*hv_Height.D()) < s_dDelayInterval)
			{
				strLog.Format("SCIPLC_双面闭环: 未达到设定纠偏延迟距离(%0.3fm)，返回", (s_hvFilterData[DoubleSide].Length()*hv_Height.D()));
				Log(strLog, DebugLog);
				return FALSE;
			}

			//判定数据是否有效
			HTuple hvData;
			HTuple hvInc = s_hvFilterData[DoubleSide].TupleFind(NO_DATA);
			if (hvInc != -1 && hvInc.Length() > 1)
			{
				TupleRemove(s_hvFilterData[DoubleSide], 0, &s_hvFilterData[DoubleSide]);
				strLog.Format("SCIPLC_双面闭环: 收集的数据中包含2个及以上的无效值，返回");
				Log(strLog, DebugLog);
				TupleRemove(s_hvFilterData[DoubleSide], 0, &s_hvFilterData[DoubleSide]);
				return FALSE;
			}
			if (hvInc != -1)
			{
				//剔除无效值
				TupleRemove(s_hvFilterData[DoubleSide], hvInc, &hvData);
			}
			else
			{
				hvData = s_hvFilterData[DoubleSide];
			}

			if (m_tPLCSet.nFilterType == 1)
			{
				s_dData[DoubleSide] = hvData.TupleMean().D();
				CString strLog;
				strLog.Format("SCIPLC_双面闭环: 计算纠偏差值均值为%0.4f,收集数据:,", s_dData[DoubleSide]);
				for (int i = 0; i < hvData.Length(); i++)
				{
					CString strTmp;
					strTmp.Format("%0.4f,", hvData[i].D());
					strLog += strTmp;
				}
				m_sInit.Log(strLog, DebugLog, NoExcp);
			}
			else if (m_tPLCSet.nFilterType == 2)
			{
				s_dData[DoubleSide] = hvData.TupleMedian().D();
				CString strLog;
				strLog.Format("SCIPLC_双面闭环: 计算纠偏差值中值为%0.4f,收集数据:,", s_dData[DoubleSide]);
				for (int i = 0; i < hvData.Length(); i++)
				{
					CString strTmp;
					strTmp.Format("%0.4f,", hvData[i].D());
					strLog += strTmp;
				}
				m_sInit.Log(strLog, DebugLog, NoExcp);
			}
			else
			{
				s_dData[DoubleSide] = hvData[hvData.Length() - 1].D();
				CString strLog;
				strLog.Format("SCIPLC_双面闭环: 计算纠偏差值最新值为%0.4f,收集数据:,", s_dData[DoubleSide]);
				for (int i = 0; i < hvData.Length(); i++)
				{
					CString strTmp;
					strTmp.Format("%0.4f,", hvData[i].D());
					strLog += strTmp;
				}
				m_sInit.Log(strLog, DebugLog, NoExcp);
			}


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
				strLog.Format("双面闭环: 纠偏差值连续%0.2fm超过±0.3mm，请检查闭环和模头是否正常工作", 15.0);
				m_sInit.Log(strLog, AlarmLog, NoExcp);
			}

			if (abs(dDiffTmp2) <= m_tPLCSet.dAlertDiff)//极耳宽度差值/2小于报警差值
			{
				_SendCorrectValue2(dDiffTmp2);
				s_dDelayCamDistanceCount[DoubleSide] = 0;
				s_hvFilterData[DoubleSide].Clear();
				if (abs(s_dData[DoubleSide]) < 0.1)
					s_dDelayInterval = m_tPLCSet.dCorrectIntervalLong;
				else
					s_dDelayInterval = m_tPLCSet.dCorrectInterval;

				strLog.Format("SCIPLC_双面闭环: 纠偏差值:%0.4fmm, 闭环值:%0.4fmm, 下次闭环周期:%0.3fm", s_dData[DoubleSide], dDiffTmp2, s_dDelayInterval + m_tPLCSet.dCamDistance);
				Log(strLog, DebugLog);
			}
			else
			{
				//偏差大于警戒值，不纠偏，报警处理
				strLog.Format("双面闭环值(%0.4f)超出上下限(+-%.3f),不执行闭环", dDiffTmp2, m_tPLCSet.dAlertDiff);
				Log(strLog, AlarmLog);
				TupleRemove(s_hvFilterData[DoubleSide], 0, &s_hvFilterData[DoubleSide]);
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

//刀距闭环
BOOL CPLC::KnifeCorrecting(C_MergeWork &Output)
{
	//刀距闭环只针对正面湿膜的情况，利用进退刀使得涂膜尺寸达到规格要求
	//进1刀，尺寸减小0.07mm，退1刀，尺寸增大0.07mm
	const int LEFT_KNIFE = 0;
	const int RIGHT_KNIFE = 1;
	CString strLog;
	HTuple hv_Width, hv_Height;
	GetImageSize(Output.vecB[0].vecA[0].hoGrabImage, &hv_Width, &hv_Height);
	hv_Height = hv_Height.D() * m_tPLCSet.dRowResolution / 1000;
	m_dImageHeightMM = hv_Height.D();//计算单张图物理尺寸

	static double s_dDelayInterval = m_tPLCSet.dCorrectInterval;//设定延迟距离，用于动态调整延迟距离
	static double s_dDelayCamDistanceCount = 0;
	static double s_dData[2] = { 0,0 };//刀量中值数组：膜宽差值/0.07mm 中值
	static HTuple s_hvFilterData[2] = { HTuple() , HTuple() };//刀量数组：膜宽差值/0.07mm

	static double s_dMeterCount = Output.hv_MeterCount.D();//米数

	try
	{
		HTuple hvLeftCorrectValue, hvRightCorrectValue;//刀量。涂布均差/0.07mm
		static double s_dNoneCoatMeter = 0;
		if (TRUE == Output.vecB[0].tMeasure.hv_HasCoat.I())
		{
			static double nNGMeterCount = 0;
			if (!_CalKnifeCorrectDiff(Output, hvLeftCorrectValue, hvRightCorrectValue))
			{
				//如果涂膜测宽数据有问题，则不进行闭环
				//if (Output.vecB[0].tMeasure.hv_HasCoat.I())
				{
					nNGMeterCount += hv_Height.D();
					if (nNGMeterCount > 10)//NG米数超过限定值，报警
					{
						nNGMeterCount = 0;
						CString strLog;
						strLog.Format("刀距闭环: 连续%0.2fm含有无效测量值，闭环未工作，请检查", 10.0);
						m_sInit.Log(strLog, AlarmLog, NoExcp);
					}
				}
				s_dMeterCount = Output.hv_MeterCount.D();
				//return FALSE;
			}
			else
			{
				nNGMeterCount = 0;
			}

			static BOOL s_bFirstCoating = FALSE;//开机到位标志位
			if (s_dNoneCoatMeter > 5)//如果无涂层，计米
			{
				s_bFirstCoating = TRUE;
			}
			s_dNoneCoatMeter = 0;

			//判断开机到位
			if (s_bFirstCoating)
			{
				if (s_dMeterCount > Output.hv_MeterCount.D())s_dMeterCount = Output.hv_MeterCount.D();
				if (Output.hv_MeterCount.D() < s_dMeterCount + m_tPLCSet.dKnifeStartIngoreDistance)
				{
					CString strLog;
					strLog.Format("刀距闭环: 设备起涂，当前记米%0.2fm，%0.2fm内不执行闭环", Output.hv_MeterCount.D(), s_dMeterCount + m_tPLCSet.dKnifeStartIngoreDistance);
					m_sInit.Log(strLog, DebugLog, NoExcp);
					return FALSE;
				}
			}

			//重量判定
			if (!_JudgeKnifeWeightOK()) return FALSE;

			s_bFirstCoating = FALSE;
			m_hvKnifeDiffValue[LEFT_KNIFE] = hvLeftCorrectValue;
			m_hvKnifeDiffValue[RIGHT_KNIFE] = hvRightCorrectValue;

			if (_JudgeKnifeDoFinish())//判断刀距是否到位
			{
				s_dDelayCamDistanceCount += hv_Height.D();
				if (s_dDelayCamDistanceCount < m_tPLCSet.dCamKnifeDistance)
				{
					strLog.Format("SCIPLC_刀距闭环: 未达到相机到模头设定距离(%0.3fm)，返回", s_dDelayCamDistanceCount);
					Log(strLog, DebugLog);
					return FALSE;
				}
			}
			else
			{
				//刀距未执行到位，重新记米
				strLog.Format("SCIPLC_刀距闭环: 刀距未执行到位，重新记录延迟距离");
				Log(strLog, DebugLog);

				s_dDelayCamDistanceCount = 0;
				return FALSE;
			}

			s_hvFilterData[LEFT_KNIFE].Append(hvLeftCorrectValue.D());//刀量数组：膜宽差值/0.07mm
			s_hvFilterData[RIGHT_KNIFE].Append(hvRightCorrectValue.D());//完成一次左右数据数据收集

			if ((s_hvFilterData[LEFT_KNIFE].Length()*hv_Height.D()) < s_dDelayInterval)
			{
				strLog.Format("SCIPLC_刀距闭环: 未达到设定纠偏延迟距离(%0.3fm)，返回", (s_hvFilterData[LEFT_KNIFE].Length()*hv_Height.D()));
				Log(strLog, DebugLog);
				return FALSE;
			}

			if (FALSE==_JudgeKnifeWeightOK())
			{
				
			}

			//判定数据是否有效
			HTuple hvData[2];//刀量数组：膜宽差值/0.07mm
			HTuple hvInc;
			//左刀距闭环值
			hvInc = s_hvFilterData[LEFT_KNIFE].TupleFind(NO_DATA);
			if (hvInc != -1 && hvInc.Length() > 1)//超过两个无效值
			{
				TupleRemove(s_hvFilterData[LEFT_KNIFE], 0, &s_hvFilterData[LEFT_KNIFE]);
				strLog.Format("SCIPLC_刀距闭环: 左刀收集的数据中包含2以上的无效值，返回");
				Log(strLog, DebugLog);
				TupleRemove(s_hvFilterData[LEFT_KNIFE], 0, &s_hvFilterData[LEFT_KNIFE]);
				TupleRemove(s_hvFilterData[RIGHT_KNIFE], 0, &s_hvFilterData[RIGHT_KNIFE]);
				return FALSE;
			}
			if (hvInc != -1)//只有一个无效值，剔除
			{
				//剔除无效值
				TupleRemove(s_hvFilterData[LEFT_KNIFE], hvInc, &hvData[LEFT_KNIFE]);
			}
			else  //没有无效值
			{
				hvData[LEFT_KNIFE] = s_hvFilterData[LEFT_KNIFE];
			}
			//右刀距闭环值
			hvInc = s_hvFilterData[RIGHT_KNIFE].TupleFind(NO_DATA);
			if (hvInc != -1 && hvInc.Length() > 1)
			{
				TupleRemove(s_hvFilterData[RIGHT_KNIFE], 0, &s_hvFilterData[RIGHT_KNIFE]);
				strLog.Format("SCIPLC_刀距闭环: 左刀收集的数据中包含2以上的无效值，返回");
				Log(strLog, DebugLog);
				TupleRemove(s_hvFilterData[LEFT_KNIFE], 0, &s_hvFilterData[LEFT_KNIFE]);
				TupleRemove(s_hvFilterData[RIGHT_KNIFE], 0, &s_hvFilterData[RIGHT_KNIFE]);
				return FALSE;
			}
			if (hvInc != -1)
			{
				//剔除无效值
				TupleRemove(s_hvFilterData[RIGHT_KNIFE], hvInc, &hvData[RIGHT_KNIFE]);
			}
			else
			{
				hvData[RIGHT_KNIFE] = s_hvFilterData[RIGHT_KNIFE];
			}

			s_dData[LEFT_KNIFE] = hvData[LEFT_KNIFE].TupleMedian().D();//求刀量中值
			s_dData[RIGHT_KNIFE] = hvData[RIGHT_KNIFE].TupleMedian().D();
			CString strLog, strTmp;
			strLog.Format("SCIPLC_刀距闭环: 计算左刀距差值中值为%0.4f,收集数据:,", s_dData[LEFT_KNIFE]);
			for (int i = 0; i < hvData[LEFT_KNIFE].Length(); i++)
			{
				strTmp.Format("%0.4f,", hvData[LEFT_KNIFE][i].D());
				strLog += strTmp;
			}
			strTmp.Format("右刀距差值中值为%0.4f,收集数据:,", s_dData[RIGHT_KNIFE]);
			strLog += strTmp;
			for (int i = 0; i < hvData[RIGHT_KNIFE].Length(); i++)
			{
				strTmp.Format("%0.4f,", hvData[RIGHT_KNIFE][i].D());
				strLog += strTmp;
			}
			m_sInit.Log(strLog, DebugLog, NoExcp);

			double dDiffTmp = s_dData[LEFT_KNIFE] * m_tPLCSet.dKnifeCorrectPercent;//刀量中值（膜宽差值/0.07mm）*单次刀距闭环比例
			double dDiffTmp2 = s_dData[RIGHT_KNIFE] * m_tPLCSet.dKnifeCorrectPercent;
			

			//-1进刀尺寸增大，1退刀尺寸减小
			if (abs(dDiffTmp) < m_tPLCSet.dKnifeAlertMinDiff && abs(dDiffTmp2) < m_tPLCSet.dKnifeAlertMinDiff)
			{
				strLog.Format("刀距闭环: 左进退刀量(%0.2fum),右进退刀量(%0.2fum),小于下限(+-%0.2fum),不执行闭环", dDiffTmp, dDiffTmp2, m_tPLCSet.dKnifeAlertMinDiff);
				Log(strLog, DebugLog);
				TupleRemove(s_hvFilterData[LEFT_KNIFE], 0, &s_hvFilterData[LEFT_KNIFE]);
				TupleRemove(s_hvFilterData[RIGHT_KNIFE], 0, &s_hvFilterData[RIGHT_KNIFE]);
				return FALSE;
			}

			if (abs(dDiffTmp) < m_tPLCSet.dKnifeAlertMinDiff)
			{
				strLog.Format("刀距闭环: 左进退刀量(%0.2fum)小于下限(+-%0.2fum),刀量设置为0", dDiffTmp, m_tPLCSet.dKnifeAlertMinDiff);
				Log(strLog, DebugLog);
				dDiffTmp = 0;
			}
			if (abs(dDiffTmp2) < m_tPLCSet.dKnifeAlertMinDiff)
			{
				strLog.Format("刀距闭环: 右进退刀量(%0.2fum)小于下限(+-%0.2fum),刀量设置为0", dDiffTmp2, m_tPLCSet.dKnifeAlertMinDiff);
				Log(strLog, DebugLog);
				dDiffTmp2 = 0;
			}

			if (abs(dDiffTmp) > m_tPLCSet.dKnifeAlertMaxDiff || abs(dDiffTmp2) > m_tPLCSet.dKnifeAlertMaxDiff)
			{
				//偏差大于警戒值，不纠偏，报警处理
				strLog.Format("刀距闭环: 左进退刀量(%0.2fum),右进退刀量(%0.2fum),大于上限(+-%0.2fum),将不执行闭环", dDiffTmp, dDiffTmp2,
					m_tPLCSet.dKnifeAlertMaxDiff);
				Log(strLog, AlarmLog);
				TupleRemove(s_hvFilterData[LEFT_KNIFE], 0, &s_hvFilterData[LEFT_KNIFE]);//除去头部数组数据
				TupleRemove(s_hvFilterData[RIGHT_KNIFE], 0, &s_hvFilterData[RIGHT_KNIFE]);

				s_dDelayCamDistanceCount = 0;
				s_hvFilterData[LEFT_KNIFE].Clear();
				s_hvFilterData[RIGHT_KNIFE].Clear();
				s_dDelayInterval = m_tPLCSet.dKnifeCorrectInterval;

				return FALSE;
				/*if(abs(dDiffTmp) > m_tPLCSet.dKnifeAlertMaxDiff)
					dDiffTmp = dDiffTmp > 0 ? abs(m_tPLCSet.dKnifeAlertMaxDiff) : (-abs(m_tPLCSet.dKnifeAlertMaxDiff));
				if (abs(dDiffTmp2) > m_tPLCSet.dKnifeAlertMaxDiff)
					dDiffTmp2 = dDiffTmp2 > 0 ? abs(m_tPLCSet.dKnifeAlertMaxDiff) : (-abs(m_tPLCSet.dKnifeAlertMaxDiff));*/
			}

			//if (abs(dDiffTmp2) > m_tPLCSet.dKnifeAlertMaxDiff)
			//{
			//	//偏差大于警戒值，不纠偏，报警处理
			//	strLog.Format("刀距闭环: 右进退刀量(%0.2fum)超出上下限(+-%0.2fum),不执行闭环", dDiffTmp2, m_tPLCSet.dKnifeAlertMaxDiff);
			//	Log(strLog, AlarmLog);
			//	return FALSE;
			//	//nDiffTmp2 = nDiffTmp2 > 0 ? abs(m_tPLCSet.nKnifeAlertDiff) : (-abs(m_tPLCSet.nKnifeAlertDiff));
			//}

			//刀距减小，重量减轻
			_SendKnifeCorrectValue(dDiffTmp, dDiffTmp2);
			s_dDelayCamDistanceCount = 0;
			s_hvFilterData[LEFT_KNIFE].Clear();
			s_hvFilterData[RIGHT_KNIFE].Clear();
			s_dDelayInterval = m_tPLCSet.dKnifeCorrectInterval;

			strLog.Format("SCIPLC_刀距闭环: 左刀距纠偏差值:%0.2fum,右刀距纠偏差值:%0.2fum,左刀距闭环值:%0.1fum,右刀距闭环值:%0.1fum,下次闭环周期:%0.3fm,上一次左刀距闭环值:%0.1fum,上一次右刀距闭环值:%0.1fum",
				s_dData[LEFT_KNIFE], s_dData[RIGHT_KNIFE], dDiffTmp, dDiffTmp2, s_dDelayInterval + m_tPLCSet.dCamKnifeDistance, m_hvKnifeCorrectValue[LEFT_KNIFE].D(), m_hvKnifeCorrectValue[RIGHT_KNIFE].D());
			Log(strLog, DebugLog);

			m_hvKnifeCorrectValue[LEFT_KNIFE] = dDiffTmp;
			m_hvKnifeCorrectValue[RIGHT_KNIFE] = dDiffTmp2;
		}
		else
		{
			s_dNoneCoatMeter += hv_Height.D();
			s_dMeterCount = Output.hv_MeterCount.D();
		}
	}
	catch (HException&e)
	{
		strLog.Format("SCIPLC_KnifeCorrecting：刀距闭环异常, %s(%s)", e.ErrorMessage().Text(), e.ProcName().Text());
		Log(strLog, ExcpLog);
	}
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
					strLog.Format("SCIPLC_CalCorrectDiff：极耳值无效或者没找到边,不执行闭环");
					Log(strLog, DebugLog);
					hvDiff = NO_DATA;
					return FALSE;
				}
				hv_FindResult0 = Output.vecB[0].tMeasure.tCoat.hv_Width.TupleFind(0);
				hv_FindResultNoData = Output.vecB[0].tMeasure.tCoat.hv_Width.TupleFind(NO_DATA);
				if (Output.vecB[0].tMeasure.tCoat.hv_Width.Length() == 0 || (hv_FindResult0 != -1 && hv_FindResult0.Length() == 0)
					|| (hv_FindResultNoData != -1 && hv_FindResultNoData.Length() == 0))
				{
					strLog.Format("SCIPLC_CalCorrectDiff：涂布值无效或者没找到边,不执行闭环");
					Log(strLog, DebugLog);
					hvDiff = NO_DATA;
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
					strLog.Format("SCIPLC_CalCorrectDiff：漏凹版值无效或者没找到边,不执行闭环");
					Log(strLog, DebugLog);
					hvDiff = NO_DATA;
					return FALSE;
				}
				//2021.3.24，增加一种情况，判断凹版不为0的个数如果小于等于4个，就认为都可以闭环，包含涂膜覆盖凹版的时候
				HTuple hvMod;
				TupleMod(hv_FindResult0, 2, &hvMod);
				//正常覆盖凹版的时候，找到0的数都是纯奇数或者纯偶数
				if (hv_FindResultNoData != -1 || hv_FindResult0.Length() > Output.vecB[0].tMeasure.tBaseCoat.hv_Width.Length()/2)
				{
					strLog.Format("SCIPLC_CalCorrectDiff：漏凹版值没找到的边过多,不执行闭环");
					Log(strLog, DebugLog);
					hvDiff = NO_DATA;
					return FALSE;
				}
				//判断凹版值大于测量值3倍，就不计算纠偏差值
				HTuple hvGreater;
				TupleGreaterElem(Output.vecB[0].tMeasure.tBaseCoat.hv_Width, m_sInit.tPdInfo.tParam.vecBaseCoatWidth[0].dStandard * 4, &hvGreater);
				HTuple hv_FindResult1 = hvGreater.TupleFind(1);
				if (hv_FindResult1.Length() > 0 && hv_FindResult1 != -1)
				{
					strLog.Format("SCIPLC_CalCorrectDiff：漏凹版值超过标准规格(%0.2f)2倍,不执行闭环", m_sInit.tPdInfo.tParam.vecBaseCoatWidth[0].dStandard);
					Log(strLog, DebugLog);
					hvDiff = NO_DATA;
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
					strLog.Format("SCIPLC_CalCorrectDiff：极耳值无效或者没找到边,不执行闭环");
					hvDiff = NO_DATA;
					return FALSE;
				}
				hv_CorrectValue = (Output.vecB[0].tMeasure.tFoil.hv_Width[0].D() - Output.vecB[0].tMeasure.tFoil.hv_Width[nLength - 1].D()) / 2;	//正面值

				//只有极耳，用涂膜中心偏差，计算平方和最大时的移动量
				//HTuple hv_FindResult0 = Output.vecB[0].tMeasure.tFoil.hv_Width.TupleFind(0);
				//HTuple hv_FindResultNoData = Output.vecB[0].tMeasure.tFoil.hv_Width.TupleFind(NO_DATA);

				//if (Output.vecB[0].tMeasure.tFoil.hv_Width.Length() == 0 || (hv_FindResult0 != -1 && hv_FindResult0.Length() == 0)
				//	|| (hv_FindResultNoData != -1 && hv_FindResultNoData.Length() == 0))
				//{
				//	strLog.Format("SCIPLC_Correcting2：极耳值无效或者没找到边,不执行闭环");
				//	Log(strLog, DebugLog);
				//	hvDiff = NO_DATA;
				//	return FALSE;
				//}
				//hv_FindResult0 = Output.vecB[0].tMeasure.tCoat.hv_Width.TupleFind(0);
				//hv_FindResultNoData = Output.vecB[0].tMeasure.tCoat.hv_Width.TupleFind(NO_DATA);

				//if (Output.vecB[0].tMeasure.tCoat.hv_Width.Length() == 0 || (hv_FindResult0 != -1 && hv_FindResult0.Length() == 0)
				//	|| (hv_FindResultNoData != -1 && hv_FindResultNoData.Length() == 0))
				//{
				//	strLog.Format("SCIPLC_Correcting2：涂布值无效或者没找到边,不执行闭环");
				//	Log(strLog, DebugLog);
				//	hvDiff = NO_DATA;
				//	return FALSE;
				//}

				////2021.4.15 修改为两边外面涂膜中心偏差
				//HTuple hvWidth1, hvWidth2;
				//hvWidth1 = Output.vecB[0].tMeasure.tFoil.hv_Width[0] + Output.vecB[0].tMeasure.tCoat.hv_Width[0] / 2;
				//hvWidth2 = Output.vecB[0].tMeasure.tFoil.hv_Width[Output.vecB[0].tMeasure.tFoil.hv_Width.Length() - 1] +
				//	Output.vecB[0].tMeasure.tCoat.hv_Width[Output.vecB[0].tMeasure.tCoat.hv_Width.Length() - 1] / 2;
				//hv_CorrectValue = hvWidth1 - hvWidth2;
			}
		}
		else if (m_sInit.tPdInfo.nCoatingType == TwoSide && m_tPLCSet.nCorrectType == DoubleSide && TRUE == Output.vecB[0].tMeasure.hv_HasCoat.I() && TRUE == Output.vecB[1].tMeasure.hv_HasCoat.I())
		{
			HTuple hv_FindResultNoData = Output.tMerge.hv_Misalignment.TupleFind(NO_DATA);
			if (hv_FindResultNoData != -1 && hv_FindResultNoData.Length() > 0 || Output.tMerge.hv_Misalignment.Length() == 0)
			{
				strLog.Format("SCIPLC_CalCorrectDiff：错位值无效,不执行闭环");
				hvDiff = NO_DATA;
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
					strLog.Format("SCIPLC_CalCorrectDiff：漏凹版值无效或者没找到边,不执行闭环");
					Log(strLog, DebugLog);
					hvDiff = NO_DATA;
					return FALSE;
				}
				//2021.3.24，增加一种情况，判断凹版不为0的个数如果小于等于4个，就认为都可以闭环，包含涂膜覆盖凹版的时候
				HTuple hvMod;
				TupleMod(hv_FindResult0, 2, &hvMod);
				//正常覆盖凹版的时候，找到0的数都是纯奇数或者纯偶数
				if (hv_FindResultNoData != -1 || hv_FindResult0.Length() > 4)
				{
					strLog.Format("SCIPLC_CalCorrectDiff：漏凹版值没找到的边过多,不执行闭环");
					Log(strLog, DebugLog);
					hvDiff = NO_DATA;
					return FALSE;
				}

				//判断凹版值大于测量值2倍，就不计算纠偏差值
				HTuple hvGreater;
				TupleGreaterElem(Output.vecB[0].tMeasure.tBaseCoat.hv_Width, m_sInit.tPdInfo.tParam.vecBaseCoatWidth[0].dStandard * 2, &hvGreater);
				HTuple hv_FindResult1 = hvGreater.TupleFind(1);
				if (hv_FindResult1.Length() > 0 && hv_FindResult1 != -1)
				{
					strLog.Format("SCIPLC_CalCorrectDiff：漏凹版值超过标准规格(%0.2f)2倍,不执行闭环", m_sInit.tPdInfo.tParam.vecBaseCoatWidth[0].dStandard);
					Log(strLog, DebugLog);
					hvDiff = NO_DATA;
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
					strLog.Format("SCIPLC_CalCorrectDiff：极耳值无效或者没找到边,不执行闭环");
					hvDiff = NO_DATA;
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
				strLog.Format("SCIPLC_CalCorrectDiff：错位值无效,不执行闭环");
				hvDiff = NO_DATA;
				return FALSE;
			}

			//错位值取均值,求标准差最小的时候做均值
			hv_CorrectValue = Output.tMerge.hv_Misalignment.TupleMean().D();
		}
		else
		{
			hvDiff = NO_DATA;
			return FALSE;
		}

		hvDiff = hv_CorrectValue.TupleMean().D();
	}
	return TRUE;
}

BOOL CPLC::_CalKnifeCorrectDiff(C_MergeWork &Output, HTuple& hvLeftDiff, HTuple& hvRightDiff)
{
	CString strLog;
	//均值
	if (TRUE)
	{
		tagPDInfo tPdInfo = m_sInit.tPdInfo;

		//判断差值
		HTuple hv_FindResult0 = Output.vecB[0].tMeasure.tCoat.hv_Width.TupleFind(0);
		HTuple hv_FindResultNoData = Output.vecB[0].tMeasure.tCoat.hv_Width.TupleFind(NO_DATA);
		if (hv_FindResultNoData != -1 || hv_FindResultNoData.Length() == 0)
		{
			strLog.Format("SCIPLC_CalKnifeCorrectDiff：涂膜值无效或者没找到边,不执行闭环");
			Log(strLog, DebugLog);
			hvLeftDiff = NO_DATA;
			hvRightDiff = NO_DATA;
			return FALSE;
		}
		//使用涂膜宽度与配方规格差值进行判断
		double dUnitKnifeCoatWidth = abs(m_tPLCSet.dUnitKnifeCoatWidth);
		if (tPdInfo.nPDType == TwoCavity)
		{
			//一出二，左右两个刀对应一个涂膜
			double dDiff = Output.vecB[0].tMeasure.tCoat.hv_Width.D() - tPdInfo.tParam.vecCoatWidth[0].dStandard;
			double dKnifeUnit = dDiff / dUnitKnifeCoatWidth;
			hvLeftDiff = dKnifeUnit;
			hvRightDiff = dKnifeUnit;
		}
		else if (tPdInfo.nPDType == FourCavity)
		{
			//一出四，左右两个刀对应左右两个涂膜
			double dLeftDiff = Output.vecB[0].tMeasure.tCoat.hv_Width[0].D() - tPdInfo.tParam.vecCoatWidth[0].dStandard;
			double dRightDiff = Output.vecB[0].tMeasure.tCoat.hv_Width[1].D() - tPdInfo.tParam.vecCoatWidth[0].dStandard;
			double dLeftKnifeUnit = dLeftDiff / dUnitKnifeCoatWidth;
			double dRightKnifeUnit = dRightDiff / dUnitKnifeCoatWidth;
			hvLeftDiff = dLeftKnifeUnit;
			hvRightDiff = dRightKnifeUnit;
		}
		else if (tPdInfo.nPDType == SixCavity)
		{
			//一出六，左右两个刀对应左右两个涂膜，左右两个刀对应中间的涂膜
			double dLeftDiff = Output.vecB[0].tMeasure.tCoat.hv_Width[0].D() - tPdInfo.tParam.vecCoatWidth[0].dStandard;
			double dCenterDiff = Output.vecB[0].tMeasure.tCoat.hv_Width[1].D() - tPdInfo.tParam.vecCoatWidth[0].dStandard;
			double dRightDiff = Output.vecB[0].tMeasure.tCoat.hv_Width[2].D() - tPdInfo.tParam.vecCoatWidth[0].dStandard;
			double dLeftKnifeUnit = (dLeftDiff + dCenterDiff) / 2 / dUnitKnifeCoatWidth;
			double dRightKnifeUnit = (dRightDiff + dCenterDiff) / 2 / dUnitKnifeCoatWidth;
			hvLeftDiff = dLeftKnifeUnit;
			hvRightDiff = dRightKnifeUnit;
		}
		else if (tPdInfo.nPDType == EightCavity)
		{
			//一出八，左右两个刀对应左右两个涂膜，左右两个刀对应中间的涂膜
			double dLeftDiff = (Output.vecB[0].tMeasure.tCoat.hv_Width[0].D() + Output.vecB[0].tMeasure.tCoat.hv_Width[1].D()) / 2 - tPdInfo.tParam.vecCoatWidth[0].dStandard;
			double dRightDiff = (Output.vecB[0].tMeasure.tCoat.hv_Width[2].D() + Output.vecB[0].tMeasure.tCoat.hv_Width[3].D()) / 2 - tPdInfo.tParam.vecCoatWidth[0].dStandard;
			double dLeftKnifeUnit = dLeftDiff / dUnitKnifeCoatWidth;
			double dRightKnifeUnit = dRightDiff / dUnitKnifeCoatWidth;
			hvLeftDiff = dLeftKnifeUnit;
			hvRightDiff = dRightKnifeUnit;
		}
	}
	return TRUE;
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
	if (CPLC::FinsTCP == m_tPLCSet.nUsePlc && bRet && m_tPLCSet.nPlcHeartBeatAddress > 0)
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
	if (CPLC::FinsTCP == m_tPLCSet.nUsePlc && bRet && m_tPLCSet.nPlcCCDEnableAddress > 0)
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
	if (CPLC::FinsTCP == m_tPLCSet.nUsePlc && bRet && m_tPLCSet.nPlcAlarmAddress > 0)
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
	if (CPLC::FinsTCP == m_tPLCSet.nUsePlc && bRet && m_tPLCSet.nPlcRunDirectionAddress > 0)
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

int CPLC::GetPLCLeftKnifePosition(double& dPos)
{
	BOOL bRet = FALSE;
	DoSomething("GetConnection", NULL, &bRet);
	float fPos = 0;
	CString strMess;
	if (CPLC::FinsTCP == m_tPLCSet.nUsePlc && bRet && m_tPLCSet.nPlcLeftKnifePosAddress > 0)
	{
		bRet = m_pFinsTcp->ReadFloat32(m_tPLCSet.nPlcLeftKnifePosAddress, fPos);
		dPos = fPos;
		if (!bRet)
		{
			strMess.Format("SCIPLC_GetPLCLeftKnifePosition:设备左刀距位置获取失败，请检查!");
			Log(strMess, ExcpLog);
			return FALSE;
		}
		return TRUE;
	}
	return FALSE;
}

int CPLC::GetPLCRightKnifePosition(double& dPos)
{
	BOOL bRet = FALSE;
	DoSomething("GetConnection", NULL, &bRet);
	float fPos = 0;
	CString strMess;
	if (CPLC::FinsTCP == m_tPLCSet.nUsePlc && bRet && m_tPLCSet.nPlcRightKnifePosAddress > 0)
	{
		bRet = m_pFinsTcp->ReadFloat32(m_tPLCSet.nPlcRightKnifePosAddress, fPos);
		dPos = fPos;
		if (!bRet)
		{
			strMess.Format("SCIPLC_GetPLCRightKnifePosition:设备右刀距位置获取失败，请检查!");
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
	if (CPLC::FinsTCP == m_tPLCSet.nUsePlc && bRet && m_tPLCSet.nPlcDieHeadDirectionAddress > 0)
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
	if (CPLC::FinsTCP == m_tPLCSet.nUsePlc && bRet && m_tPLCSet.nPlcStartCoatAddress > 0)
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
	if (CPLC::FinsTCP == m_tPLCSet.nUsePlc && bRet && m_tPLCSet.nPlcDieHeadPosAddress > 0)
	{
		bRet = m_pFinsTcp->ReadFloat32(m_tPLCSet.nPlcDieHeadPosAddress, fPos);
		dPos = fPos;
		if (!bRet)
		{
			strMess.Format("SCIPLC_GetPLCDieHeadPosition:设备模头移动位置获取失败，请检查!");
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
		if (hv_max.D() - hv_min.D() < 0.05) return FALSE;
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
	CString strMess, strBatchName;
	strBatch = "None";
	if (CPLC::FinsTCP == m_tPLCSet.nUsePlc && bRet && m_tPLCSet.nPlcBatchNameAddress > 0)
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
	if (CPLC::FinsTCP == m_tPLCSet.nUsePlc && bRet && m_tPLCSet.nPlcBatchNameVerifyAddress > 0)
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

BOOL CPLC::GetProductTypeName(CString& strProductType)
{
	BOOL bRet = FALSE;
	DoSomething("GetConnection", NULL, &bRet);
	CString strMess, strProductTypeName;
	strProductType = "None";
	if (CPLC::FinsTCP == m_tPLCSet.nUsePlc && bRet && m_tPLCSet.nPlcBatchNameAddress > 0)
	{
		bRet = m_pFinsTcp->ReadStringBatchInverse(m_tPLCSet.nReadProductTypeAddress, 16, strProductTypeName);
		if (!bRet)
		{
			strMess.Format("SCIPLC_GetProductTypeName: 读取品种号失败");
			Log(strMess, ExcpLog);
			return FALSE;
		}
		if (strProductTypeName == "") strProductTypeName = "None";
		strProductType = strProductTypeName;
		return TRUE;
	}
	return FALSE;
}

BOOL CPLC::WriteProductTypeVerify(CString& strProductType)
{
	BOOL bRet = FALSE;
	DoSomething("GetConnection", NULL, &bRet);
	CString strMess;
	if (CPLC::FinsTCP == m_tPLCSet.nUsePlc && bRet && m_tPLCSet.nProductTypeAddress > 0)
	{
		bRet = m_pFinsTcp->WriteStringBatchInverse(m_tPLCSet.nProductTypeAddress, strProductType);
		if (!bRet)
		{
			strMess.Format("SCIPLC_WriteProductTypeVerify: 写入品种号失败");
			Log(strMess, ExcpLog);
			return FALSE;
		}
		return TRUE;
	}
	return FALSE;
}

BOOL CPLC::WriteSoftwareVersionVerify(CString& strSoftwareVersion)
{
	BOOL bRet = FALSE;
	DoSomething("GetConnection", NULL, &bRet);
	CString strMess;
	if (CPLC::FinsTCP == m_tPLCSet.nUsePlc && bRet && m_tPLCSet.nPlcSoftwareVersionAddress > 0)
	{
		bRet = m_pFinsTcp->WriteStringBatchInverse(m_tPLCSet.nPlcSoftwareVersionAddress, strSoftwareVersion);
		if (!bRet)
		{
			strMess.Format("SCIPLC_WriteSoftwareVersionVerify: 写入软件版本号失败");
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
	if (m_pFinsTcp != NULL && m_pFinsTcp->IsConnected() && m_tPLCSet.nSingleCorrectAddress > 0)
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
	if (m_pFinsTcp != NULL && m_pFinsTcp->IsConnected() && m_tPLCSet.nDoubleCorrectAddress > 0)
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

BOOL CPLC::_SendKnifeCorrectValue(double& dLeftValue, double& dRightValue)
{
	CString strLog;
	if (m_pFinsTcp != NULL && m_pFinsTcp->IsConnected() && m_tPLCSet.nPlcLeftKnifeCorrectAddress > 0 && m_tPLCSet.nPlcRightKnifeCorrectAddress > 0)
	{
		int nLeftValue = dLeftValue * 10;
		int nRightValue = dRightValue * 10;
		BOOL bRet1 = m_pFinsTcp->WriteInt32(m_tPLCSet.nPlcLeftKnifeCorrectAddress, nLeftValue);
		BOOL bRet2 = m_pFinsTcp->WriteInt32(m_tPLCSet.nPlcRightKnifeCorrectAddress, nRightValue);
		if (bRet1 && bRet2)
		{
			strLog.Format("SCIPLC_SendKnifeCorrectValue：向PLC发送左进退刀量(%d)和右进退刀量(%d)成功", nLeftValue, nRightValue);
			Log(strLog, DebugLog);
		}
		if (m_tPLCSet.nPlcKnifeDoCorrectAddress > 0)
		{
			m_pFinsTcp->WriteInt32(m_tPLCSet.nPlcKnifeDoCorrectAddress, 1);//让PLC执行刀距闭环
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

BOOL CPLC::_JudgeKnifeDoFinish()
{
	CString strLog;
	int nFinishSignal = 2;
	if (m_tPLCSet.bJudgeKnifeDoFinish)
	{
		if (m_pFinsTcp != NULL && m_pFinsTcp->IsConnected() && m_tPLCSet.nPlcKnifeDoFinishAddress > 0)
		{
			m_pFinsTcp->ReadInt32(m_tPLCSet.nPlcKnifeDoFinishAddress, nFinishSignal);//纠偏完成标志位地址：10588
		}
	}

	if (nFinishSignal != 2)
	{
		strLog.Format("SCIPLC：刀距移动到位标志位不为2, 刀距闭环未执行到位");
		Log(strLog, DebugLog);
		return FALSE;
	}
	return TRUE;
}

BOOL CPLC::_JudgeKnifeWeightOK()
{
	if (!m_tPLCSet.nUseWeightData) return TRUE;

	BOOL bConnect = FALSE;
	m_sInit.Main(MT_PLC, "GetBetaRayConnection", "", NULL, &bConnect);
	if (!bConnect) return TRUE;

	try
	{
		HTupleVector hvWeightData(1), hvWeightDataX(1);
		m_sInit.Main(MT_PLC, "GetBetaRayMMData", "", &hvWeightDataX, &hvWeightData);

		if (hvWeightDataX.Length() == 0 || hvWeightData.Length() == 0) return TRUE;
		if (hvWeightData[0].T().Length() <= 5) return TRUE;

		//BetaRayPdInfo tBetaRayPdInfo;
		//m_sInit.Main(MT_PLC, "GetBetaRayPdInfo", "", NULL, &tBetaRayPdInfo);

		CString strLog;
		if (m_sInit.tPdInfo.nPDType != ThreeCavity)
		{
			//取重量数据
			HTuple hvCoatMeanWeight;
			int nCoatNum = m_sInit.tPdInfo.nPDType;
			for (int i = 0; i < nCoatNum; i++)
			{
				int nWeightNum = hvWeightData[0].T().Length() / nCoatNum;
				if (i == nCoatNum - 1)
				{
					HTuple hvTmpData;
					TupleSelectRange(hvWeightData[0].T(), i*nWeightNum, hvWeightData[0].T().Length() - 1, &hvTmpData);
					hvCoatMeanWeight.Append(hvTmpData.TupleMean());
				}
				else
				{
					HTuple hvTmpData;
					TupleSelectRange(hvWeightData[0].T(), i*nWeightNum, (i + 1)*nWeightNum - 1, &hvTmpData);
					hvCoatMeanWeight.Append(hvTmpData.TupleMean());
				}
			}

			//判定每个涂膜重量数据变化,左右涂膜平均重量超过设定值，
			double dWeightDiff = hvCoatMeanWeight[0].D() - hvCoatMeanWeight[hvCoatMeanWeight.Length() - 1].D();
			//记录重量差值
			CString  sWeightDiffLog;
			sWeightDiffLog.Format("刀距闭环-左重量均值：%0.2f，右重量均值：%0.2f，重量差值：%0.2f，偏差限定值为：%0.2f"
				, hvCoatMeanWeight[0].D(), hvCoatMeanWeight[hvCoatMeanWeight.Length() - 1].D(),dWeightDiff, m_tPLCSet.dBetaRayWeightDiffLimit);
			Log(sWeightDiffLog, DebugLog);
			if (abs(dWeightDiff) > m_tPLCSet.dBetaRayWeightDiffLimit)
			{
				strLog.Format("SCIPLC_刀距闭环: 左右涂膜平均重量差值%0.2f，超出最大限制%0.2f，不执行刀距闭环。平均重量数据:,");
				for (int i = 0; i < hvCoatMeanWeight.Length(); i++)
				{
					CString strTmp;
					strTmp.Format("%0.2f,", hvCoatMeanWeight[i].D());
					strLog += strTmp;
				}
				Log(strLog, AlarmLog);
				return FALSE;
			}
		}
	}
	catch (HException&e)
	{
		CString strLog;
		strLog.Format("CPLC_JudgeKnifeWeightOK: 异常:%s,%s", e.ErrorMessage().Text(), e.ProcName().Text());
		Log(strLog, ExcpLog, SlightExcp);
		
		return FALSE;
	}
	

	return TRUE;
}