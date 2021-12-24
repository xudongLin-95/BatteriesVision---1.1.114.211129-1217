#include "StdAfx.h"
#include "CommunicationTool.h"
#include "SCIIO.in.h"
#include "SCIPLC.in.h"
#include "SCIRectifcation.in.h"
#include "LightControl.in.h"
#include "SCIMes.in.h"
#include "Temperature.in.h"
CCommunicationTool::CCommunicationTool()
{
	m_pIO = NULL;
	m_pPLC = NULL;
	//m_pRectification1 = NULL; 
	//m_pRectification2 = NULL;
	m_pMES = NULL;
	m_pLightControl = NULL;
	m_pMarkingControl = NULL;
	m_pUPSControl = NULL;
	m_pTemperature = NULL;

	m_nPlcRunDirection = 1;
	m_nPlcDieHeadDirection = -1;
	m_nPlcRunStart = 1;
	m_dPlcDieHeadPosition = 0;
	m_nPlcStartCoat = 1;
}

CCommunicationTool::~CCommunicationTool()
{

}

BOOL CCommunicationTool::Init()
{
	ReadParam();
	InitCommLibs();
	return TRUE;
}

BOOL CCommunicationTool::ReadParam()
{
	//从配置文件获取用到几个
	CString m_strTempSerialFile;
	m_strTempSerialFile.Format("%s\\Config\\Public\\Temperature\\ComSetting.ini", GetCurrentAppPath());
	m_nTempertatureNums = GetPrivateProfileInt("Config", "TempNum", 1, m_strTempSerialFile);

	return TRUE;
}

BOOL CCommunicationTool::WriteParam()
{

	return TRUE;
}

void CCommunicationTool::InitCommLibs()
{
	//初始化IO通信库
	IOINIT IOInitParam;
	IOInitParam.strProductName.Format("%s", theGlobal.m_ProductTool.GetCurrentProductName());
	IOInitParam.Log = theGlobal.m_Log.LogCallBack;
	IOInitParam.nProductType = 0;
	m_pIO = LoadDHInterFaceDll("SCIIO");
	if (m_pIO == NULL ||
		TRUE != m_pIO->DoSomething("INITIALIZE", &IOInitParam, NULL))
	{
		CString strException;
		strException.Format("Fail to init SCIIO.dll,program exists.\r\SCIIO.dll初始化失败，程序将退出");
		AfxMessageBox(strException);
		exit(1);
	}

	////初始化PLC通信库
	PLCINIT PLCInitParam;
	PLCInitParam.tPdInfo = theGlobal.m_ProductTool.GetCurrentProductInfo();
	PLCInitParam.Log = theGlobal.m_Log.LogCallBack;
	PLCInitParam.Main = theGlobal.m_Callback.MainCallback;
	m_pPLC = LoadDHInterFaceDll("SCIPLC");
	if (m_pPLC == NULL ||
		TRUE != m_pPLC->DoSomething("INITIALIZE", &PLCInitParam, NULL))
	{
		CString strException;
		strException.Format("Fail to init SCIPLC.dll,program exists.\r\SCIPLC.dll初始化失败，程序将退出");
		AfxMessageBox(strException);
		exit(1);
	}

	//初始化MES通信库
	MESINIT MesInitParam;
	MesInitParam.Log = theGlobal.m_Log.LogCallBack;
	MesInitParam.Main = theGlobal.m_Callback.MainCallback;
	MesInitParam.tPdInfo = theGlobal.m_ProductTool.GetCurrentProductInfo();
	m_pMES = LoadDHInterFaceDll("SCIMes");
	if (m_pMES == NULL ||
		TRUE != m_pMES->DoSomething("INITIALIZE", &MesInitParam, NULL))
	{
		CString strException;
		strException.Format("Fail to init SCIMes.dll,program exists.\r\SCIMes.dll初始化失败，程序将退出");
		AfxMessageBox(strException);
		exit(1);
	}

	//初始化光源控制器库
	LIGHTINIT LightInitParam;
	LightInitParam.strProductName = theGlobal.m_ProductTool.GetCurrentProductName();
	LightInitParam.nProductType = theGlobal.m_ProductTool.GetCurrentProductInfo().nPDType;
	LightInitParam.Log = theGlobal.m_Log.LogCallBack;
	m_pLightControl = LoadDHInterFaceDll("LightControl");
	if (m_pLightControl == NULL ||
		TRUE != m_pLightControl->DoSomething("INITIALIZE", &LightInitParam, NULL))
	{
		CString strException;
		strException.Format("Fail to init LightControl.dll,program exists.\r\nLightControl.dll初始化失败，程序将退出");
		AfxMessageBox(strException);
		exit(1);
	}

	//初始化喷码机通信模块
	MARKINGINIT MarkingInitParam;
	MarkingInitParam.strProductName = theGlobal.m_ProductTool.GetCurrentProductName();
	MarkingInitParam.tPdInfo = theGlobal.m_ProductTool.GetCurrentProductInfo();
	MarkingInitParam.Log = theGlobal.m_Log.LogCallBack;
	MarkingInitParam.Main = theGlobal.m_Callback.MainCallback;
	m_pMarkingControl = LoadDHInterFaceDll("Marking");
	if (m_pMarkingControl == NULL ||
		TRUE != m_pMarkingControl->DoSomething("INITIALIZE", &MarkingInitParam, NULL))
	{
		CString strException;
		strException.Format("Fail to init Marking.dll,program exists.\r\nMarking.dll初始化失败，程序将退出");
		AfxMessageBox(strException);
		exit(1);
	}

	//初始化UPS通信模块
	UPSINIT UPSInitParam;
	UPSInitParam.tPdInfo = theGlobal.m_ProductTool.GetCurrentProductInfo();
	UPSInitParam.Log = theGlobal.m_Log.LogCallBack;
	UPSInitParam.Main = theGlobal.m_Callback.MainCallback;
	m_pUPSControl = LoadDHInterFaceDll("UpsControl");
	if (m_pUPSControl == NULL ||
		TRUE != m_pUPSControl->DoSomething("INITIALIZE", &UPSInitParam, NULL))
	{
		CString strException;
		strException.Format("Fail to init UpsControl.dll,program exists.\r\n UpsControl.dll初始化失败，程序将退出");
		AfxMessageBox(strException);
		exit(1);
	}

	//初始化温控器通信模块
	TEMPERATUREINIT TemperatureInitParam;
	TemperatureInitParam.tPdInfo = theGlobal.m_ProductTool.GetCurrentProductInfo();
	TemperatureInitParam.Log = theGlobal.m_Log.LogCallBack;
	TemperatureInitParam.Main = theGlobal.m_Callback.MainCallback;
	m_pTemperature = LoadDHInterFaceDll("Temperature");
	if (m_pTemperature == NULL ||
		TRUE != m_pTemperature->DoSomething("INITIALIZE", &TemperatureInitParam, NULL))
	{
		CString strException;
		strException.Format("Fail to init Temperature.dll,program exists.\r\n Temperature.dll初始化失败，程序将退出");
		AfxMessageBox(strException);
		exit(1);
	}
}

int	CCommunicationTool::DoSomething(eCommType eType, void *message, void *wParam, void *lParam)
{
	if (IO == eType && NULL != m_pIO)
		return m_pIO->DoSomething(message, wParam, lParam);
	else if (PLC == eType && NULL != m_pPLC)
		return m_pPLC->DoSomething(message, wParam, lParam);
	else if (MES == eType && NULL != m_pMES)
		return m_pMES->DoSomething(message, wParam, lParam);
	else if (LightController == eType && NULL != m_pLightControl)
		return m_pLightControl->DoSomething(message, wParam, lParam);
	else if (Marking == eType && NULL != m_pMarkingControl)
		return m_pMarkingControl->DoSomething(message, wParam, lParam);
	else if (UPS == eType && NULL != m_pUPSControl)
		return m_pUPSControl->DoSomething(message, wParam, lParam);
	else if (Temperature == eType && NULL != m_pTemperature)
		return m_pTemperature->DoSomething(message, wParam, lParam);
	return -1;
}

int CCommunicationTool::ShowParamDlg(eCommType eType, void* wParam)
{
	if (IO == eType && NULL != m_pIO)
		return m_pIO->DoSomething("SHOW", wParam, NULL);
	else if (PLC == eType && NULL != m_pPLC)
		return m_pPLC->DoSomething("SHOW", wParam, NULL);
	else if (MES == eType && NULL != m_pMES)
		return m_pMES->DoSomething("SHOW", wParam, NULL);
	else if (LightController == eType && NULL != m_pLightControl)
		return m_pLightControl->DoSomething("SHOW", wParam, NULL);
	else if (Marking == eType &&NULL != m_pMarkingControl)
		return m_pMarkingControl->DoSomething("SHOW", wParam, NULL);
	else if (UPS == eType &&NULL != m_pUPSControl)
		return m_pUPSControl->DoSomething("SHOW", wParam, NULL);
	else if (Temperature == eType &&NULL != m_pTemperature)
		return m_pTemperature->DoSomething("SHOW", wParam, NULL);
	return FALSE;
}

void CCommunicationTool::Enqueue(eCommType eType, HTuple& MessageHandle)
{
	if (IO == eType && NULL != m_pIO)
		m_pIO->DoSomething("PUSH_MESSAGE", &MessageHandle, NULL);
	if (PLC == eType && NULL != m_pPLC)
		m_pPLC->DoSomething("PUSH_MESSAGE", &MessageHandle, NULL);
	if (MES == eType && NULL != m_pMES)
		m_pMES->DoSomething("PUSH_MESSAGE", &MessageHandle, NULL);

	/*else if (PLC==eType && NULL!=m_pPLC)
	return m_pPLC->DoSomething("SHOW",wParam,NULL);
	else if (Rectifcation==eType && NULL!=m_pRectification)
	return m_pRectification->DoSomething("SHOW",wParam,NULL);
	*/
}

BOOL WINAPI CCommunicationTool::RecificationCallBack(PRECTIFICATIONCALL tCall)
{
	/*CString strLog;
	strLog.Format("CommunicationTool_RecificationCallBack: bReLoad:%d ErrorCode%d Type:%d",tCall->bReLoad,tCall->nErrorCode,tCall->nType);
	theGlobal.m_Log.Debug(strLog);*/
	return TRUE;
}

int CCommunicationTool::Rectify(const C_MergeWork& tOutput)
{
	//if (TRUE == theGlobal.m_tSystemSet.bOffsetServerConnect)
	//{
	//	if (tOutput.B[0].tMeasureOutput.hvCoatWidth > 0 && tOutput.B[1].tMeasureOutput.hvCoatWidth > 0)
	//	{
	//		if (0 == theGlobal.m_tSystemSet.nOffsetType)//涂布区域
	//		{
	//			try
	//			{
	//				HTuple Data, Column1, Column2;
	//				Data.Append("Offset");
	//				for (int i = 0; i < PRODUCT_COUNT; i++)
	//				{
	//					Data.Append(tOutput.B[i].tMeasureOutput.hvCoatWidth);
	//					SmallestRectangle1(tOutput.B[i].tMeasureOutput.hoRegionCoat, NULL, &Column1, NULL, &Column2);
	//					double dRegionWidth = Column2 - Column1;
	//					//需要对Col1,Col2进行修正,
	//					double dScaleX = nImageZoomWidth / (dRegionWidth * (1+ 2* dImageDilation));
	//					Column1 = dRegionWidth * dImageDilation;
	//					Column2 = Column1 + dRegionWidth;
	//					Column1 *= dScaleX;
	//					Column2 *= dScaleX;
	//					Data.Append(Column1);
	//					Data.Append(Column2);
	//				}
	//				for (int i = 0; i < PRODUCT_COUNT; i++)
	//				{
	//					HTuple hvMean, hvSigma, hvCpk;
	//					CalculateCPK(i, tOutput.B[i].tMeasureOutput.hvCoatWidth.D(), &hvMean, &hvSigma, &hvCpk);
	//					Data.Append(hvMean);
	//					Data.Append(hvSigma);
	//					Data.Append(hvCpk);
	//				}
	//				SendTuple(theGlobal.m_hOffsetSocket, Data);
	//			}
	//			catch (HException e)
	//			{
	//				theGlobal.m_tSystemSet.bOffsetServerConnect = FALSE;
	//				CString strLog;
	//				strLog.Format("闭环IP:%s:%d 连接建立失败", theGlobal.m_tSystemSet.strOffsetServerIP, theGlobal.m_tSystemSet.nOffsetServerPort);
	//				theGlobal.m_Log.Std(strLog);
	//			}
	//		}
	//		else if (1 == theGlobal.m_tSystemSet.nOffsetType)//总宽度
	//		{
	//			try
	//			{
	//				HTuple Data, Column1, Column2;
	//				Data.Append("Offset");
	//				for (int i = 0; i < PRODUCT_COUNT; i++)
	//				{
	//					double dSend(0);
	//					dSend = tOutput.B[i].tMeasureOutput.hvWidth;
	//					
	//					Data.Append(dSend);
	//					HObject RegionUnion;
	//					Union2(tOutput.B[i].tMeasureOutput.hoRegionCoat, tOutput.B[i].tMeasureOutput.tSide[i].hoRegionAT9, &RegionUnion);
	//					SmallestRectangle1(RegionUnion, NULL, &Column1, NULL, &Column2);
	//					HTuple Width;
	//					GetImageSize(tOutput.B[i].A.hoGrabImage, &Width, NULL);
	//					double dRegionWidth = Column2 - Column1;
	//					//需要对Col1,Col2进行修正,
	//					double dScaleX = nImageZoomWidth / (dRegionWidth * (1 + 2 * dImageDilation));
	//					Column1 = dRegionWidth * dImageDilation;
	//					Column2 = Column1 + dRegionWidth;
	//					Column1 *= dScaleX;
	//					Column2 *= dScaleX;
	//					Data.Append(Column1);
	//					Data.Append(Column2);
	//				}
	//				for (int i = 0; i < PRODUCT_COUNT; i++)
	//				{
	//					HTuple hvMean, hvSigma, hvCpk;
	//					double dSend;
	//					dSend = tOutput.B[i].tMeasureOutput.hvWidth;
	//					CalculateCPK(i, dSend, &hvMean, &hvSigma, &hvCpk);
	//					Data.Append(hvMean);
	//					Data.Append(hvSigma);
	//					Data.Append(hvCpk);
	//				}
	//				SendTuple(theGlobal.m_hOffsetSocket, Data);
	//			}
	//			catch (HException e)
	//			{
	//				theGlobal.m_tSystemSet.bOffsetServerConnect = FALSE;
	//				CString strLog;
	//				strLog.Format("闭环IP:%s:%d 连接建立失败", theGlobal.m_tSystemSet.strOffsetServerIP, theGlobal.m_tSystemSet.nOffsetServerPort);
	//				theGlobal.m_Log.Std(strLog);
	//			}
	//		}
	//	}
	//}
	return TRUE;
}

int CCommunicationTool::SendCutRectifyValue(const C_MergeWork& tOutput)
{
	//#define DEBUG_LINE_RECITIFICATION
// #ifdef DEBUG_LINE_RECITIFICATION
// 	return TestLineRectify(tOutput);
// #else
// 	if (tOutput.B[2].tDetectOutput.hv_CoatWidth.Length() >= 4)
// 	{
// 		double dRectifyVal = (tOutput.B[2].tDetectOutput.hv_CoatWidth[0].D() - tOutput.B[2].tDetectOutput.hv_CoatWidth[3].D()) / 2;
// 		try
// 		{
// 			CString strLog;
// 			strLog.Format("CommunicationTool_SendCutRectifyValue: ImageSN:%d RectifyCutDiff:%0.3fmm", tOutput.B[0].A.hvImageSN.I(), dRectifyVal);
// 			theGlobal.m_Log.Debug(strLog);
// 			int nMianThread(FALSE);
// 			DoSomething(Rectifcation3Stage, "WriteDiff", &dRectifyVal, &nMianThread);
// 		}
// 		catch (HException& e)
// 		{
// 			CString strEXCP;
// 			strEXCP.Format("CommunicationTool_SendCutRectifyValue: %s", e.ErrorMessage().Text());
// 			theGlobal.m_Log.Exception(strEXCP);
// 		}
// 	}
// #endif // DEBUG_LINE_RECITIFICATION

	return TRUE;
}

int CCommunicationTool::SendAT9RectifyValue(const C_MergeWork& tOutput)
{
	/*TestLineRectify2(tOutput);
	return TRUE;*/
	/*if (tOutput.B[0].tDetectOutput.hv_LocateMissing.D() != NO_DATA
		&& tOutput.B[1].tDetectOutput.hv_LocateMissing.D() != NO_DATA)
	{
		double dRectifyVal = -tOutput.B[0].tDetectOutput.hv_LocateMissing.D()  + tOutput.B[1].tDetectOutput.hv_LocateMissing.D();
		try
		{
			CString strLog;
			strLog.Format("CommunicationTool_SendAT9RectifyValue: ImageSN:%d RectifyAT9Diff:%0.3fmm Col-0:%0.3fmm Col-1:%0.3fmm", tOutput.B[0].A.hvImageSN.I(), dRectifyVal,
				(tOutput.B[0].tDetectOutput.Coat.hv_StartColumn.D()+ tOutput.B[0].tDetectOutput.Coat.hv_EndColumn.D())/2, (tOutput.B[1].tDetectOutput.Coat.hv_StartColumn.D() + tOutput.B[1].tDetectOutput.Coat.hv_EndColumn.D())/2);
			theGlobal.m_Log.Std(strLog);
			int nMianThread(FALSE);
			DoSomething(Rectifcation3Stage, "WriteDiff", &dRectifyVal, &nMianThread);
		}
		catch (HException& e)
		{
			CString strEXCP;
			strEXCP.Format("CommunicationTool_SendAT9RectifyValue: %s", e.ErrorMessage().Text());
			theGlobal.m_Log.Exception(strEXCP);
		}
	}*/

	return TRUE;
}

int CCommunicationTool::EnableIOCard(const BOOL& bEnable)
{
	if (bEnable)
		return theGlobal.m_CommunicationTool.DoSomething(CCommunicationTool::IO, "Enable", "True", NULL);
	else
		return theGlobal.m_CommunicationTool.DoSomething(CCommunicationTool::IO, "Enable", "False", NULL);
}

double CCommunicationTool::GetIOCardLineRate()
{
	int nLineRate = 0;
	int nReadCount = 16;
	theGlobal.m_CommunicationTool.DoSomething(CCommunicationTool::IO, "ReadCount", &nReadCount, &nLineRate);

	return (double)nLineRate;
}

BOOL CCommunicationTool::RefreshRunStatus()
{
	//设置心跳信号，和PLC或者服务器通信
	int nStatus = 1;
	return DoSomething(CCommunicationTool::PLC, "SetHeartBeat", &nStatus, NULL);
}

BOOL CCommunicationTool::PLCReconnect()
{
	return DoSomething(CCommunicationTool::PLC, "Reconnect", NULL, NULL);
}

BOOL CCommunicationTool::PLCCCDEnable(BOOL bEnable)
{
	int nEnable = 0;
	if (bEnable) nEnable = 1;
	return DoSomething(CCommunicationTool::PLC, "CCDEnable", &nEnable, NULL);
}

BOOL CCommunicationTool::CCDAlarm()
{
	//W3嘉拓，信号是1，设备声光报警
	int nCode = 1;
	return DoSomething(CCommunicationTool::PLC, "CCDAlarm", &nCode, NULL);
}

BOOL CCommunicationTool::CCDStop()
{
	//W3嘉拓，信号是2，设备停机报警
	int nCode = 2;
	return DoSomething(CCommunicationTool::PLC, "CCDAlarm", &nCode, NULL);
}

int  CCommunicationTool::GetPlcRunDirection()
{
	//设备运行方向 1代表前进，-1代表后退
	int nDirection=1;
	int nRet = DoSomething(CCommunicationTool::PLC, "PLCRunDirection", NULL, &nDirection);
	if (nRet) return nDirection;
	else return 1;
}

int  CCommunicationTool::GetPlcDieHeadDirection()
{
	//1代表模头进刀，0模头推刀
	int nDirection = 1;
	int nRet = DoSomething(CCommunicationTool::PLC, "PLCDieHeadDirection", NULL, &nDirection);
	if (nRet) return nDirection;
	else return 1;
}

int  CCommunicationTool::GetPlcStartCoat()
{
	int nDirection = 1;
	int nRet = DoSomething(CCommunicationTool::PLC, "PlcStartCoat", NULL, &nDirection);
	if (nRet) return nDirection;
	else return 1;
}

void CCommunicationTool::GetPlcEquipmentSignal()
{
	int nRet = 0;
	CString strLog;

	int nConnected = FALSE;
	theGlobal.m_CommunicationTool.DoSomething(CCommunicationTool::PLC, "GetConnection", NULL, &nConnected);
	if (FALSE == nConnected) return;

	//设备走带方向
	static int s_nRunDirection = m_nPlcRunDirection;
	m_nPlcRunDirection = GetPlcRunDirection();
	if (s_nRunDirection != m_nPlcRunDirection)
	{
		if (m_nPlcRunDirection == 0)
		{
			strLog.Format("....设备停止....");
		}
		else if (m_nPlcRunDirection == 1)
		{
			strLog.Format("....设备开启(正转)....");
		}
		else
		{
			strLog.Format("....设备开启(反转)....");
		}
		theGlobal.m_Log.Operate(strLog);
	}
	s_nRunDirection = m_nPlcRunDirection;

	//设备开启标志位
	if (m_nPlcRunDirection == 0) m_nPlcRunStart = 0;
	else m_nPlcRunStart = 1;

	//设备进刀退刀
	static int s_nPlcDieHeadDirection = m_nPlcDieHeadDirection;
	m_nPlcDieHeadDirection = GetPlcDieHeadDirection();
	if (s_nPlcDieHeadDirection != m_nPlcDieHeadDirection)
	{
		if (m_nPlcDieHeadDirection == 0)
		{
			strLog.Format("....设备模头退刀....");
		}
		else
		{
			strLog.Format("....设备模头进刀....");
		}
		theGlobal.m_Log.Operate(strLog);
	}
	s_nPlcDieHeadDirection = m_nPlcDieHeadDirection;

	//起涂信号
	static int s_nPlcStartCoat = m_nPlcStartCoat;
	m_nPlcStartCoat = GetPlcStartCoat();
	if (s_nPlcStartCoat != m_nPlcStartCoat)
	{
		if (m_nPlcStartCoat == 1)
		{
			strLog.Format("....设备开始涂....");
		}
		else
		{
			strLog.Format("....设备停止涂....");
		}
		theGlobal.m_Log.Operate(strLog);
	}
	s_nPlcStartCoat = m_nPlcStartCoat;

	nRet = DoSomething(CCommunicationTool::PLC, "PLCDieHeadPosition", NULL, &m_dPlcDieHeadPosition);
	if (nRet == 0) m_dPlcDieHeadPosition = 0;
}

BOOL CCommunicationTool::Mark(C_MergeWork& tOutput)
{
	if (m_pMarkingControl != NULL)
		return m_pMarkingControl->DoSomething("SendData", &tOutput, NULL);

	return FALSE;
}

BOOL CCommunicationTool::UploadDeviceInfoToMes()
{
	tagMesDeviceInfo tInfo;
	tInfo.strDeviceCode = theGlobal.m_tSystemSet.strDeviceID;
	if (theGlobal.IsStart())
		tInfo.strDeviceStatus = "Run";
	else
		tInfo.strDeviceStatus = "Idle";

	return m_pMES->DoSomething("UploadDeviceInfo", &tInfo, NULL);
}