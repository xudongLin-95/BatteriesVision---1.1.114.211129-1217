#include "StdAfx.h"
#include "Global.h"
#include "DlgLoading.h"
#include "DlgAlarm.h"
#include "afxinet.h"
#include "CrashRecord.in.h"

int     WM_DISP = RegisterWindowMessage (_T("WM_DISP"));
int     WM_GRABBER = RegisterWindowMessage (_T("WM_GRABBER"));
int     WM_DETECT = RegisterWindowMessage (_T("WM_DETECT"));
int     WM_RESTARTPROCESS = RegisterWindowMessage (_T("WM_RESTARTPROCESS"));
int		WM_REFRESH_DISP = RegisterWindowMessage (_T("WM_REFRESH_DISP"));
int		WM_CHECK_PMSN = RegisterWindowMessage(_T("WM_CHECK_PMSN"));
int     WM_CHANGEBATCH = RegisterWindowMessage(_T("WM_CHANGEBATCH"));
int     WM_UPDATE_PRODUCT = RegisterWindowMessage(_T("WM_UPDATE_PRODUCT"));
int     WM_UPDATE_ONLINEOFFSET = RegisterWindowMessage(_T("WM_UPDATE_ONLINEOFFSET"));
CGlobal theGlobal;
CGlobal::CGlobal()
	:m_hovecHistroyImage(1)
{
	//版本号： 主版本.次版本(功能改动).修改版本(bug修复/小变动).日期 开发阶段(Beta,RC,Release)
	//例如1.0.7.201020 Beta
	m_strVersion = "1.1.96.210717";
	try 
	{
		SetSystem("width", 50000);
		SetSystem("height", 10000);
		SetSystem("parallelize_operators", "true");
		SetSystem("use_window_thread", "true");
		SetHcppInterfaceStringEncodingIsUtf8(false);
		SetSystem("filename_encoding", "locale");
		SetSystem("write_halcon_files_encoding", "locale");
		SetSystem("read_halcon_files_encoding_fallback", "locale");
		SetSystem("tuple_string_operator_mode", "byte");
		SetSystem("tsp_tuple_string_operator_mode", "byte");
		SetSystem("clip_region", "false"); 
		SetSystem("max_connection", 3000);
		m_bMSA = FALSE;
		m_dCurSpeed = 0;
		//GetSystem ("version", Information)
	}catch(HException& e)
	{
		CString strMessageBox;
		strMessageBox.Format("Operating license has expired, please contact the supplier.\r\n运行许可已到期,请联系供应商\r\n%s  [ %s() ]",
			e.ErrorMessage().Text(),e.ProcName().Text());
		AfxMessageBox(strMessageBox);
		exit(1);
	}
	m_nTheme=19;//纯黑色界面
	//m_nTheme=13;//白色界面
	m_vecResolution.clear();
	m_tSystemSet.Init();
	//m_nTheme = GetIniInt("Theme","Style",18);
	::GetLocalTime(&m_tmProgramOpen);
	m_strPositionParamFile.Format("%s//Config//Public//Calibration//PositionParam.ini",GetCurrentAppPath());
	if (INVALID_FILE_ATTRIBUTES == GetFileAttributes(m_strPositionParamFile))
	{
		CString strMessageBox;
		strMessageBox.Format("%s\r\nFile not exists, program exits.\r\n标定参数文件未找到，程序将终止",m_strPositionParamFile);
		AfxMessageBox(strMessageBox);
		exit(1);
	}
	m_strCalibrationPath.Format("%s//Config//Public//Calibration//",GetCurrentAppPath());
	m_bStart = FALSE;
	m_Language.ReadParam();
	m_pCrash = NULL;
	m_bExitThread = FALSE;
}

CGlobal::~CGlobal()
{
	
}

void CGlobal::InitLoadingDlg()
{
	m_pDlgLoading = new CDlgLoading;
	m_pDlgLoading->Create(CDlgLoading::IDD);
	m_pDlgLoading->CenterWindow();
	m_pDlgLoading->ShowWindow(SW_NORMAL);
	m_pDlgLoading->UpdateWindow();
}

void CGlobal::CloseLoadingDlg()
{
	m_pDlgLoading->PostMessageA(WM_CLOSE);
	m_pDlgLoading->ShowWindow(SW_HIDE);
}

void CGlobal::SetLoadingDlgInfo(const CString& strInfoCN,const CString& strInfoEN)
{
	if(NULL == m_pDlgLoading)
		return;

	m_pDlgLoading->SetLoadingInfo(strInfoCN,strInfoEN);
}

void CGlobal::SetLoadingDlgInfo(const CString& strInfoCN,const CString& strInfoEN,double dPercent)
{
	if(NULL == m_pDlgLoading)
		return;

	m_pDlgLoading->SetLoadingInfo(strInfoCN,strInfoEN,dPercent);
}

void CGlobal::InitAlarmDlg()
{
	m_pDlgAlarmInfo = new CDlgAlarm;
	m_pDlgAlarmInfo->Create(CDlgAlarm::IDD);
	m_pDlgAlarmInfo->CenterWindow();
	m_pDlgAlarmInfo->ShowWindow(SW_HIDE);
	m_pDlgAlarmInfo->UpdateWindow();
}

void CGlobal::CloseAlarmDlg()
{
	if (NULL == m_pDlgAlarmInfo)
		return;
	m_pDlgAlarmInfo->PostMessageA(WM_CLOSE);
	m_pDlgAlarmInfo->ShowWindow(SW_HIDE);
}

void CGlobal::SetAlarmInfo(const CString& strInfo)
{
	if (NULL == m_pDlgAlarmInfo || strInfo =="")
		return;

	m_pDlgAlarmInfo->ShowAlarm(strInfo);
}

void CGlobal::SetAlarmInfo(CErrorInfo::tagErrorInfo tInfo)
{
	if (NULL == m_pDlgAlarmInfo)
		return;

	m_pDlgAlarmInfo->ShowAlarm(tInfo);
}

void CGlobal::SetAlarmInfo(const int& nErrorCode)
{
	if (NULL == m_pDlgAlarmInfo)
		return;

	m_pDlgAlarmInfo->ShowAlarm(nErrorCode);
}

void CGlobal::InitTipDlg()
{
	m_pDlgTip = new CDlgTip;
	m_pDlgTip->Create(CDlgTip::IDD);
	m_pDlgTip->CenterWindow();
	m_pDlgTip->ShowWindow(SW_HIDE);
	m_pDlgTip->UpdateWindow();
}

void CGlobal::SetTipInfo(eTipType eType)
{
	if (NULL == m_pDlgTip)
		return;

	m_pDlgTip->ShowTip(eType);
}

void CGlobal::CloseTipDlg()
{
	if (NULL == m_pDlgTip)
		return;
	m_pDlgTip->PostMessageA(WM_CLOSE);
	m_pDlgTip->ShowWindow(SW_HIDE);
}

void CGlobal::Verify()
{
	DHInterface* pVerify = LoadDHInterFaceDll("LCDVerify");
	if (NULL == pVerify)
		exit(0);

	if (TRUE != pVerify->DoSomething("Verify", (theGlobal.m_Language.IsChinese() ? "Chinese" : "English"), NULL))
	{
		if (pVerify)
			delete pVerify;
		exit(0);
	}

	if (pVerify)
		delete pVerify;
}

void CGlobal::ShowVerifyInfo()
{
	DHInterface* pVerify = LoadDHInterFaceDll("LCDVerify");
	if (NULL != pVerify)
		pVerify->DoSomething("Info", (theGlobal.m_Language.IsChinese() ? "Chinese" : "English"), NULL);

	if (pVerify)
		delete pVerify;
}

void CGlobal::CrashRecord()
{
	m_pCrash = LoadDHInterFaceDll("CrashRecord");
	m_pCrash->DoSomething("CrashRecord", NULL, NULL);

	CrashRecordInit tInit;
	tInit.pLog = theGlobal.m_Log.LogCallBack;
	m_pCrash->DoSomething("Init", &tInit, NULL);
}

double CGlobal::GetCPURate()
{
	double dRate = 0;
	if (m_pCrash == NULL) return dRate;

	m_pCrash->DoSomething("GetCPURate", NULL, &dRate);
	return dRate;
}

double CGlobal::GetRAMUseMB()
{
	double dUse = 0;
	if (m_pCrash == NULL) return dUse;

	m_pCrash->DoSomething("GetRAMUseMB", NULL, &dUse);
	return dUse;
}

BOOL CGlobal::InitAll()
{
	Init();
	m_Language.Init();
	m_Project.Init();
	m_pDlgLoading->SetLoadingInfo("日志模块加载中...","Loading log module...",5.0);
	m_Log.Init();
	CrashRecord();
	m_pDlgLoading->SetLoadingInfo("权限模块加载中...","Loading permission module...",10.0);
	m_Permission.Init();
	m_pDlgLoading->SetLoadingInfo("产品管理模块加载中...","Loading product management module...",15.0);
	m_ProductTool.Init();
	m_pDlgLoading->SetLoadingInfo("通信模块加载中...","Loading communication module...",25.0);
	m_CommunicationTool.Init();
	m_pDlgLoading->SetLoadingInfo("统计模块加载中...","Loading statistics module...",30.0);
	m_Counter.Init();
	m_pDlgLoading->SetLoadingInfo("相机模块加载中...","Loading camera module...",50.0);
	m_GrabberTool.Init();
	m_pDlgLoading->SetLoadingInfo("算法模块加载中...","Loading algorithm module...",70.0);
	m_AlgTool.Init();
	m_pDlgLoading->SetLoadingInfo("处理模块加载中...","Loading processing module...",80.0);
	m_Judge.Init();
	m_Process.Init();
	m_pDlgLoading->SetLoadingInfo("存储模块加载中...","Loading storage module...",85.0);
	m_Storage.Init();
	m_DataFilter.Init();
	m_DataCalculate.Init();
	m_Upload.Init();
	m_Speed.Init();
	m_FoolProofing.Init();
	m_Callback.Init();

	return TRUE;
}

BOOL CGlobal::Init()
{
	ReadParam();

	return TRUE;
}

BOOL CGlobal::ReadParamAll()
{
	ReadParam();

	m_Language.ReadParam();
	m_Project.ReadParam();
	m_Log.ReadParam();
	m_Permission.ReadParam();
	m_ProductTool.ReadParam();
	m_GrabberTool.ReadParam();
	m_Counter.ReadParam();
	m_AlgTool.ReadParam();
	m_Judge.ReadParam();
	m_Process.ReadParam();
	m_Storage.ReadParam();
	m_CommunicationTool.ReadParam();
	m_DataFilter.ReadParam();
	m_DataCalculate.ReadParam();
	m_Speed.ReadParam();
	m_FoolProofing.ReadParam();
	m_Callback.ReadParam();
	m_Upload.ReadParam();

	return TRUE;
}

BOOL CGlobal::ReadParam()
{
	//读取参数
	CString strHelpFile = GetIniString("HelpFile","HelpFile","help.pdf");
	m_strHelpFile.Format("%s//Help//%s",GetCurrentAppPath(),strHelpFile);

	//系统设置参数
	m_tSystemSet.eSN = (eStationSN)GetIniInt("System", "nSN", SN_Head);
	m_tSystemSet.eDevType = (eDeviceType)GetIniInt("System", "nDevType", DT_SingleLayer);
	m_tSystemSet.strWorkshopName = GetIniString("System", "WorkshopName", "M5");
	m_tSystemSet.strDeviceID = GetIniString("System", "DeviceID", "TMJI0001");

	m_tSystemSet.nHistoryCount	= GetIniInt("System","HistoryCount",10);
	m_tSystemSet.eHistoryType	= (eHistoryType)GetIniInt("System","HistoryType",AllHistory);
	m_tSystemSet.bGrabAlign		= GetIniInt("System","GrabAlign",0);
	m_tSystemSet.nCameraDistance = GetPrivateProfileInt("TranslateLocate","DelayLine", 100, m_strPositionParamFile);

	m_tSystemSet.nSpotInspectionIntervalSecond = GetIniInt("System", "SpotInspectionIntervalSecond", 300);

	m_tSystemSet.dEnableOffsetSpeed = GetIniDouble("System", "dEnableOffsetSpeed", 10.0);

	m_tSystemSet.bUseChangeBatchAuto = GetIniInt("System", "UseChangeBatchAuto", FALSE);
	m_tSystemSet.dChangeBatchDelayMeter = GetIniDouble("System", "ChangeBatchDelayMeter", 0.0);
	m_tSystemSet.bCheckProductInfoByChangeBatch = GetIniInt("System", "CheckProductInfoByChangeBatch", FALSE);
	
	int nCamCount = GetIniInt("Project", "GrabberCount", 4);
	//相机分辨率
	m_vecResolution.clear();
	for (int i = 0; i < nCamCount; i++)
	{
		CString strFile;
		strFile.Format("%s//%d//CameraParma.ini", m_strCalibrationPath, i);
		tagResolution res;
		res.dRow = GetPrivateProfileDouble("CalibrationParam", "YResolution", 0.03, strFile);
		res.dColumn = GetPrivateProfileDouble("CalibrationParam", "XResolution", 0.03, strFile);
		m_vecResolution.push_back(res);
	}

	return TRUE;
}

BOOL CGlobal::WriteParamAll()
{
	WriteParam();

	m_Project.WriteParam();
	m_ProductTool.WriteParam();
	m_Permission.WriteParam();
	m_Counter.WriteParam();
	m_GrabberTool.WriteParam();
	m_AlgTool.WriteParam();
	m_Judge.WriteParam();
	m_Process.WriteParam();
	m_Log.WriteParam();
	m_Storage.WriteParam();
	m_Language.WriteParam();
	m_CommunicationTool.WriteParam();
	m_DataFilter.WriteParam();
	m_DataCalculate.WriteParam();
	m_FoolProofing.WriteParam();
	m_Callback.WriteParam();
	m_Upload.WriteParam();

	return TRUE;
}

BOOL CGlobal::WriteParam()
{
	//写入参数
	SetFileAttributes(m_strSystemFilePath,FILE_ATTRIBUTE_NORMAL);
	WriteIniInt("Theme","Style",m_nTheme);

	WriteIniString("System", "WorkshopName", m_tSystemSet.strWorkshopName);
	WriteIniString("System", "DeviceID", m_tSystemSet.strDeviceID);
	WriteIniInt("System", "nDevType", m_tSystemSet.eDevType);
	WriteIniInt("System", "nSN", m_tSystemSet.eSN);

	WriteIniInt("System","HistoryCount",m_tSystemSet.nHistoryCount);
	WriteIniInt("System","HistoryType",(int)m_tSystemSet.eHistoryType);
	WriteIniInt("System","GrabAlign",m_tSystemSet.bGrabAlign);
	WritePrivateProfileInt("TranslateLocate","DelayLine", m_tSystemSet.nCameraDistance, m_strPositionParamFile);

	WriteIniInt("System", "SpotInspectionIntervalSecond", m_tSystemSet.nSpotInspectionIntervalSecond);

	WriteIniInt("System", "UseChangeBatchAuto", m_tSystemSet.bUseChangeBatchAuto);
	WriteIniDouble("System", "ChangeBatchDelayMeter", m_tSystemSet.dChangeBatchDelayMeter);
	WriteIniInt("System", "CheckProductInfoByChangeBatch", m_tSystemSet.bCheckProductInfoByChangeBatch);

	return TRUE;
}

BOOL CGlobal::PostMessageToMainDlg(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (m_pMainDlg->GetSafeHwnd()!=NULL
		&& IsWindow(m_pMainDlg->GetSafeHwnd()))
		return m_pMainDlg->PostMessageA(message,wParam,lParam);
	
	return FALSE;
}

BOOL CGlobal::SendMessageToMainDlg(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (m_pMainDlg->GetSafeHwnd() != NULL
		&& IsWindow(m_pMainDlg->GetSafeHwnd()))
		return m_pMainDlg->SendMessageA(message, wParam, lParam);

	return FALSE;
}