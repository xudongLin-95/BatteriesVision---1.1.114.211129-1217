// DlgParamSet.cpp : 实现文件
//

#include "stdafx.h"
#include "SCIPLC.h"
#include "DlgParamSet.h"
#include "afxdialogex.h"
#include <string>


// CDlgParamSet 对话框

IMPLEMENT_DYNAMIC(CDlgParamSet, CBCGPDialog)

CDlgParamSet::CDlgParamSet(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(CDlgParamSet::IDD, pParent)
	, m_EDIT_strIP(_T(""))
	, m_EDIT_nBatchAddress(0)
	, m_EDT_nGuidFinishAddr(0)
	, m_EDT_nDoCorrectAddr(0)
	, m_EDT_nCCDEnableAddress(0)
	, m_EDT_nPLCRunDirectionAddress(0)
	, m_EDIT_dCorrectIntervalLong(0)
	, m_EDIT_dCamDistance(0)
	, m_EDT_nPLCDieHeadDirectionAddress(0)
	, m_EDIT_nBatchAddressVerify(0)
	, m_EDT_nPLCStartCoatAddress(0)
	, m_EDT_nPLCDieHeadPositionAddress(0)
	, m_EDT_nPLCAlarmAddress(0)
	, m_EDIT_dKnifeAlertDiff(0)
	, m_EDIT_dKnifeUnit(0)
	, m_EDIT_dKnifeCorrectPercent(0)
	, m_EDIT_nLeftKnifeCorrectAddr(0)
	, m_EDIT_nRightKnifeCorrectAddr(0)
	, m_EDIT_nKnifeCorrectDoAddr(0)
	, m_EDIT_nKnifeCorrectFinishAddr(0)
	, m_EDIT_nLeftKnifePosAddr(0)
	, m_EDIT_nRightKnifePosAddr(0)
	, m_EDIT_dKnifeAlertMinDiff(0)
	, m_EDIT_dCamKnifeDistance(0)
	, m_EDIT_dKnifeCorrectInterval(0)
	, m_EDIT_dKnifeStartIngoreMeter(0)
	, m_EDIT_dBateRayDistance(0)
	, m_EDIT_dBetaRayMaxWeightDiff(0)
	, m_Edit_nProductTypeAddr(0)
	, m_Edit_nPlcSoftwareVersionAddr(0)
	, m_Edit_nReadProductType_Addr(0)
{
	m_EDIT_nPort = 0;
	m_EDIT_nSingleCorrectAddress = 0;
	m_EDIT_nDoubleCorrectAddress = 0;
	m_EDIT_nPlcHeartBeatAddress = 0;
	m_EDIT_nDecimal = 0;
	m_EDIT_dAlertDiff = 0;
	m_EDIT_nFilterSize = 0;
	m_EDIT_dCorrectPercent = 0;
	m_EDIT_dCorrectInterval = 0;
	m_EDT_nPLCDieHeadPositionAddress = 0;
}

CDlgParamSet::~CDlgParamSet()
{
}

void CDlgParamSet::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_PORT, m_EDIT_nPort);
	DDX_Text(pDX, IDC_IPADDRESS_IP, m_EDIT_strIP);
	DDX_Text(pDX, IDC_EDIT_SINGLE_CORRECT_ADDRESS, m_EDIT_nSingleCorrectAddress);
	DDX_Text(pDX, IDC_EDIT_DOUBLE_CORRECT_ADDRESS, m_EDIT_nDoubleCorrectAddress);
	DDX_Text(pDX, IDC_EDIT_PLC_HEARTBEAT_ADDRESS, m_EDIT_nPlcHeartBeatAddress);
	DDX_Text(pDX, IDC_EDIT_ALERT_DIFF, m_EDIT_dAlertDiff);
	DDX_Text(pDX, IDC_EDIT_FILTER_SIZE, m_EDIT_nFilterSize);
	DDX_Text(pDX, IDC_EDIT_CORRECT_PERCENT, m_EDIT_dCorrectPercent);
	DDX_Text(pDX, IDC_EDIT_CORRECT_INTERVAL, m_EDIT_dCorrectInterval);
	DDX_Control(pDX, IDC_COMBO_COMMU_TYPE, m_combCommuType);
	DDX_Control(pDX, IDC_COMO_CORRECT_TYPE, m_combCorrectType);
	DDX_Control(pDX, IDC_COMBO_FILTER_TYPE, m_combFitterType);
	DDX_Control(pDX, IDC_COMO_CORRECT_LOGIC_TYPE, m_combCorrectLogicType);
	DDX_Control(pDX, IDC_COMBO_CORRECT_DIRECTION, m_Combo_CoorectDirection);
	DDX_Text(pDX, IDC_EDIT_PLC_BATCH_ADDRESS, m_EDIT_nBatchAddress);
	DDX_Text(pDX, IDC_EDIT_GUID_FINISH_ADDR, m_EDT_nGuidFinishAddr);
	DDX_Text(pDX, IDC_EDIT_PLC_CCD_DO_CORRECT_ADDR, m_EDT_nDoCorrectAddr);
	DDX_Text(pDX, IDC_EDIT_PLC_CCD_ENABLE_ADDRESS, m_EDT_nCCDEnableAddress);
	DDX_Text(pDX, IDC_EDIT_PLC_RUN_DIRECTION_ADDR, m_EDT_nPLCRunDirectionAddress);
	DDX_Text(pDX, IDC_EDIT_CORRECT_INTERVAL_LONG, m_EDIT_dCorrectIntervalLong);
	DDX_Text(pDX, IDC_EDIT_CAM_DISTANCE, m_EDIT_dCamDistance);
	DDX_Text(pDX, IDC_EDIT_PLC_DIEHEAD_DIRECTION_ADDR, m_EDT_nPLCDieHeadDirectionAddress);
	DDX_Text(pDX, IDC_EDIT_PLC_BATCH_ADDRESS_VERIFY, m_EDIT_nBatchAddressVerify);
	DDX_Text(pDX, IDC_EDIT_PLC_START_COAT_ADDR, m_EDT_nPLCStartCoatAddress);
	DDX_Text(pDX, IDC_EDIT_PLC_DIEHEAD_POS_ADDR, m_EDT_nPLCDieHeadPositionAddress);
	DDX_Text(pDX, IDC_EDIT_PLC_ALARM_ADDR, m_EDT_nPLCAlarmAddress);
	DDX_Text(pDX, IDC_EDIT_KNIFE_ALERT_DIFF, m_EDIT_dKnifeAlertDiff);
	DDX_Text(pDX, IDC_EDIT_KNIFE_UNIT, m_EDIT_dKnifeUnit);
	DDX_Text(pDX, IDC_EDIT_KNIFECORRECT_PERCENT, m_EDIT_dKnifeCorrectPercent);
	DDX_Text(pDX, IDC_EDIT_LEFT_KNIFE_ADDR, m_EDIT_nLeftKnifeCorrectAddr);
	DDX_Text(pDX, IDC_EDIT_RIGHT_KNIFE_ADDR2, m_EDIT_nRightKnifeCorrectAddr);
	DDX_Text(pDX, IDC_EDIT_RIGHT_KNIFE_DO_ADDR, m_EDIT_nKnifeCorrectDoAddr);
	DDX_Text(pDX, IDC_EDIT_GUID_FINISH_ADDR2, m_EDIT_nKnifeCorrectFinishAddr);
	DDX_Text(pDX, IDC_EDIT_LEFT_KNIFE_POS_ADDR, m_EDIT_nLeftKnifePosAddr);
	DDX_Text(pDX, IDC_EDIT_RIGHT_KNIFE_POS_ADDR, m_EDIT_nRightKnifePosAddr);
	DDX_Text(pDX, IDC_EDIT_KNIFE_MIN_ALERT_DIFF, m_EDIT_dKnifeAlertMinDiff);
	DDX_Text(pDX, IDC_EDIT_CAM_KNIFE_DISTANCE, m_EDIT_dCamKnifeDistance);
	DDX_Text(pDX, IDC_EDIT_KNIFE_CORRECT_INTERVAL, m_EDIT_dKnifeCorrectInterval);
	DDX_Text(pDX, IDC_EDIT_KNIFE_START_IGNORE_METER, m_EDIT_dKnifeStartIngoreMeter);
	DDX_Text(pDX, IDC_EDIT_BETARAY_DISTANCE, m_EDIT_dBateRayDistance);
	DDX_Text(pDX, IDC_EDIT_BETARAY_MAX_WEIGHT_DIFF, m_EDIT_dBetaRayMaxWeightDiff);
	DDX_Text(pDX, IDC_EDIT_PLC_PRODUCT_TYPE_ADDR, m_Edit_nProductTypeAddr);
	DDX_Text(pDX, IDC_EDIT_PLC_SOFTWARE_VERSION_ADDR, m_Edit_nPlcSoftwareVersionAddr);
	DDX_Text(pDX, IDC_EDIT_PLC_REDPRODUCTTYPE_ADDR, m_Edit_nReadProductType_Addr);
}


BEGIN_MESSAGE_MAP(CDlgParamSet, CBCGPDialog)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_ACTIVATE()
	ON_WM_CTLCOLOR()
	ON_WM_NCHITTEST()
	ON_BN_CLICKED(IDOK, &CDlgParamSet::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgParamSet::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CDlgParamSet::OnBnClickedButtonClose)
	ON_BN_CLICKED(IDC_BTN_WRITETEST, &CDlgParamSet::OnBnClickedBtnWritetest)
	ON_BN_CLICKED(IDC_BTN_READ, &CDlgParamSet::OnBnClickedBtnRead)
	ON_BN_CLICKED(IDC_BUTTON_CONNECT, &CDlgParamSet::OnBnClickedButtonConnect)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CDlgParamSet 消息处理程序


BOOL CDlgParamSet::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	EnableVisualManagerStyle(TRUE, TRUE);
	InitUI();
	SetTimer(UM_REFRESH_LIST, 500, NULL);
	SetTimer(UM_HEART_BEAT, 2000, NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


BOOL CDlgParamSet::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (pMsg->message == WM_KEYDOWN || pMsg->message == WM_SYSKEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN)//屏蔽回车
		{
			return TRUE;
		}
		else if (pMsg->wParam == VK_ESCAPE)//屏蔽ESC
		{
			return TRUE;
		}
	}
	return CBCGPDialog::PreTranslateMessage(pMsg);
}


void CDlgParamSet::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CBCGPDialog::OnPaint()
}


BOOL CDlgParamSet::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//return TRUE;
	return CBCGPDialog::OnEraseBkgnd(pDC);
}


void CDlgParamSet::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CBCGPDialog::OnActivate(nState, pWndOther, bMinimized);

	if (WA_INACTIVE == nState)
	{
		::SetWindowLong(this->GetSafeHwnd(), GWL_EXSTYLE
			, ::GetWindowLong(this->GetSafeHwnd(), GWL_EXSTYLE) | WS_EX_LAYERED);
		this->SetLayeredWindowAttributes(0, (255 * 90) / 100, LWA_ALPHA);
	}
	else if (WA_CLICKACTIVE == nState)
	{
		::SetWindowLong(this->GetSafeHwnd(), GWL_EXSTYLE
			, ::GetWindowLong(this->GetSafeHwnd(), GWL_EXSTYLE)&~WS_EX_LAYERED);
		this->RedrawWindow();
	}
	else if (WA_ACTIVE == nState)
	{
		::SetWindowLong(this->GetSafeHwnd(), GWL_EXSTYLE
			, ::GetWindowLong(this->GetSafeHwnd(), GWL_EXSTYLE)&~WS_EX_LAYERED);
		this->RedrawWindow();
	}
}


HBRUSH CDlgParamSet::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CBCGPDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何特性
	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}


LRESULT CDlgParamSet::OnNcHitTest(CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	return CBCGPDialog::OnNcHitTest(point);
}

void CDlgParamSet::InitUI()
{
	m_combCommuType.AddString(_T("None"));
	m_combCommuType.AddString(_T("Fins"));
	m_combCommuType.AddString(_T("Modbus"));
	m_combCommuType.AddString(_T("Ads"));

#pragma region 单双面下拉选项
	m_combCorrectType.AddString(_T("双面"));
	m_combCorrectType.AddString(_T("单面"));
	m_combCorrectType.SetCurSel((int)m_pOwner->m_tPLCSet.nCorrectType);
#pragma endregion
#pragma region 闭环逻辑下拉选项
	m_combCorrectLogicType.AddString(_T("标准差"));
	m_combCorrectLogicType.AddString(_T("均值"));
	m_combCorrectLogicType.SetCurSel((int)m_pOwner->m_tPLCSet.nCorrectLogicType);
#pragma endregion
#pragma region 数据滤波下拉选项
	m_combFitterType.AddString(_T("不滤波"));
	m_combFitterType.AddString(_T("均值滤波"));
	m_combFitterType.AddString(_T("中值滤波"));
	m_combFitterType.SetCurSel((int)m_pOwner->m_tPLCSet.nFilterType);
#pragma endregion

	m_Combo_CoorectDirection.AddString(_T("左正右负"));
	m_Combo_CoorectDirection.AddString(_T("左负右正"));
	m_Combo_CoorectDirection.SetCurSel((int)m_pOwner->m_tPLCSet.nCorrectDirection);

	((CButton*)GetDlgItem(IDC_CHECK_USE_PLC))->SetCheck((bool)m_pOwner->m_tPLCSet.nUsePlc);
	((CButton*)GetDlgItem(IDC_CHECK_USE_CORRECT))->SetCheck((bool)m_pOwner->m_tPLCSet.nUseCorrect);

	((CIPAddressCtrl*)GetDlgItem(IDC_IPADDRESS_IP))->SetWindowTextA(m_pOwner->m_tPLCSet.strPLCIP);

	m_EDIT_nPort = m_pOwner->m_tPLCSet.nPLCPort;
	m_EDIT_strIP = m_pOwner->m_tPLCSet.strPLCIP;
	m_EDIT_nSingleCorrectAddress = m_pOwner->m_tPLCSet.nSingleCorrectAddress;
	m_EDIT_nDoubleCorrectAddress = m_pOwner->m_tPLCSet.nDoubleCorrectAddress;
	m_EDT_nDoCorrectAddr = m_pOwner->m_tPLCSet.nDoCorrectAddress;
	m_EDIT_nPlcHeartBeatAddress = m_pOwner->m_tPLCSet.nPlcHeartBeatAddress;
	m_EDT_nCCDEnableAddress = m_pOwner->m_tPLCSet.nPlcCCDEnableAddress;
	m_EDIT_nBatchAddress = m_pOwner->m_tPLCSet.nPlcBatchNameAddress;
	m_EDIT_nBatchAddressVerify = m_pOwner->m_tPLCSet.nPlcBatchNameVerifyAddress;
	m_EDT_nPLCRunDirectionAddress = m_pOwner->m_tPLCSet.nPlcRunDirectionAddress;
	m_EDT_nPLCDieHeadDirectionAddress = m_pOwner->m_tPLCSet.nPlcDieHeadDirectionAddress;
	m_EDT_nPLCStartCoatAddress = m_pOwner->m_tPLCSet.nPlcStartCoatAddress;
	m_EDT_nPLCDieHeadPositionAddress = m_pOwner->m_tPLCSet.nPlcDieHeadPosAddress;
	m_EDT_nPLCAlarmAddress = m_pOwner->m_tPLCSet.nPlcAlarmAddress;
	m_Edit_nProductTypeAddr = m_pOwner->m_tPLCSet.nProductTypeAddress;
	m_Edit_nPlcSoftwareVersionAddr = m_pOwner->m_tPLCSet.nPlcSoftwareVersionAddress;
	m_Edit_nReadProductType_Addr = m_pOwner->m_tPLCSet.nReadProductTypeAddress;

	m_EDIT_dAlertDiff = m_pOwner->m_tPLCSet.dAlertDiff;
	m_EDIT_nFilterSize = m_pOwner->m_tPLCSet.nFilterSize;
	m_EDIT_dCorrectPercent = m_pOwner->m_tPLCSet.dCorrectPercent;
	m_EDIT_dCorrectInterval = m_pOwner->m_tPLCSet.dCorrectInterval;
	m_EDIT_dCorrectIntervalLong = m_pOwner->m_tPLCSet.dCorrectIntervalLong;
	m_EDIT_dCamDistance = m_pOwner->m_tPLCSet.dCamDistance;

	((CButton*)GetDlgItem(IDC_CHECK_JUDGE_GUID_FINSH))->SetCheck((bool)m_pOwner->m_tPLCSet.bJudgeGuidFinish);
	m_EDT_nGuidFinishAddr = m_pOwner->m_tPLCSet.nGuidFinishAddr;

	if (m_pOwner->m_tPLCSet.nUsePlc == 0)
	{
		m_combCommuType.SetCurSel(0);
	}
	else if (m_pOwner->m_tPLCSet.nUsePlc == 1)
	{
		m_combCommuType.SetCurSel(1);
	}
	else if (m_pOwner->m_tPLCSet.nUsePlc == 2)
	{
		m_combCommuType.SetCurSel(2);
	}
	else if (m_pOwner->m_tPLCSet.nUsePlc == 3)
	{
		m_combCommuType.SetCurSel(3);
	}

	//刀距闭环
	((CButton*)GetDlgItem(IDC_CHECK_USE_KNIFECORRECT))->SetCheck((bool)m_pOwner->m_tPLCSet.nUseKnifeCorrect);
	((CButton*)GetDlgItem(IDC_CHECK_USE_WEIGHT_DATA))->SetCheck((bool)m_pOwner->m_tPLCSet.nUseWeightData);
	m_EDIT_dKnifeAlertDiff = m_pOwner->m_tPLCSet.dKnifeAlertMaxDiff;
	m_EDIT_dKnifeAlertMinDiff = m_pOwner->m_tPLCSet.dKnifeAlertMinDiff;
	m_EDIT_dKnifeUnit = m_pOwner->m_tPLCSet.dUnitKnifeCoatWidth;
	m_EDIT_dKnifeCorrectPercent = m_pOwner->m_tPLCSet.dKnifeCorrectPercent;
	m_EDIT_dCamKnifeDistance = m_pOwner->m_tPLCSet.dCamKnifeDistance;
	m_EDIT_dKnifeCorrectInterval = m_pOwner->m_tPLCSet.dKnifeCorrectInterval;
	m_EDIT_dKnifeStartIngoreMeter = m_pOwner->m_tPLCSet.dKnifeStartIngoreDistance;
	m_EDIT_dBetaRayMaxWeightDiff = m_pOwner->m_tPLCSet.dBetaRayWeightDiffLimit;
	m_EDIT_dBateRayDistance = m_pOwner->m_tPLCSet.dBetaRayDistance;

	((CButton*)GetDlgItem(IDC_CHECK_JUDGE_KNIFE_FINSH))->SetCheck((bool)m_pOwner->m_tPLCSet.bJudgeKnifeDoFinish);
	m_EDIT_nLeftKnifeCorrectAddr = m_pOwner->m_tPLCSet.nPlcLeftKnifeCorrectAddress;
	m_EDIT_nRightKnifeCorrectAddr = m_pOwner->m_tPLCSet.nPlcRightKnifeCorrectAddress;
	m_EDIT_nKnifeCorrectDoAddr = m_pOwner->m_tPLCSet.nPlcKnifeDoCorrectAddress;
	m_EDIT_nKnifeCorrectFinishAddr = m_pOwner->m_tPLCSet.nPlcKnifeDoFinishAddress;
	m_EDIT_nLeftKnifePosAddr = m_pOwner->m_tPLCSet.nPlcLeftKnifePosAddress;
	m_EDIT_nRightKnifePosAddr = m_pOwner->m_tPLCSet.nPlcRightKnifePosAddress;

	RefreshButton();

	CBCGPGridColors theme;
	CRect rcGridBase;
	GetDlgItem(IDC_STATIC_GRID)->GetWindowRect(rcGridBase);
	ScreenToClient(rcGridBase);
	m_LST_Grid_List.Create(WS_CHILD, rcGridBase, this, 1000);
	CBCGPVisualManager::GetInstance()->OnSetGridColorTheme(&m_LST_Grid_List, theme);
	m_LST_Grid_List.SendMessage(BCGM_ONSETCONTROLVMMODE, TRUE);
	m_LST_Grid_List.SetColorTheme(theme);
	m_LST_Grid_List.SetReadOnly();//可编辑
	m_LST_Grid_List.EnableMarkSortedColumn(FALSE);//使用列排序
	m_LST_Grid_List.EnableHeader(TRUE, BCGP_GRID_HEADER_MOVE_ITEMS);//使用表头
	m_LST_Grid_List.EnableRowHeader(TRUE);//使用行表头
	m_LST_Grid_List.EnableLineNumbers(TRUE);//使用行序号
	m_LST_Grid_List.SetClearInplaceEditOnEnter(FALSE);
	m_LST_Grid_List.EnableInvertSelOnCtrl(FALSE);
	m_LST_Grid_List.SetScalingRange(0.1, 2.0);//设置缩放比例，按Ctrl键和鼠标滚轮可缩放
	int nCount = 0;
	BOOL bChinese = m_pOwner->m_Lang.IsChinese();
	m_LST_Grid_List.InsertColumn(nCount++, bChinese ? "项目" : "Item", 140);
	m_LST_Grid_List.InsertColumn(nCount++, bChinese ? "地址(DM)" : "Address(DM)", 80);
	m_LST_Grid_List.InsertColumn(nCount++, bChinese ? "值" : "Value", 80);
	m_LST_Grid_List.LoadState(_T("BasicGrid"));
	m_LST_Grid_List.AdjustLayout();//相当于数据显示在表格上
	m_LST_Grid_List.ShowWindow(SW_NORMAL);
	RefreshAddressValueList();

	GetDlgItem(IDC_EDIT_ADD)->SetWindowTextA("");

	m_pOwner->m_Lang.SetDlgLanguage(GetSafeHwnd(), "IDD_DIALOG_PLC_PARAMSET");

	((CComboBox*)GetDlgItem(IDC_COMBO_VALUE_TYPE))->SetCurSel(2);

	UpdateData(FALSE);

	ManageCorrectPowerLimit();
}

//纠偏权限管理
void CDlgParamSet::ManageCorrectPowerLimit()
{
	try
	{
		//纠偏权限管理
		USERINFO  tUserInfo;
		m_pOwner->GetPlcInit().Main(MT_PLC, "GetUserInfo", NULL, &tUserInfo, NULL);
		if (tUserInfo.ePowerType > PT_TECHNICIAN)
		{
			EnableCorrectPowerLimit(true);
		}
		else
		{
			EnableCorrectPowerLimit(false);
		}
	}
	catch (const std::exception&)
	{
	}
	catch (HException&e)
	{
	}
	catch (...)
	{
	}
}

void CDlgParamSet::EnableCorrectPowerLimit(BOOL Enable)
{
	try
	{
		//GetDlgItem(IDC_COMO_CORRECT_TYPE)->EnableWindow(Enable);
		GetDlgItem(IDC_COMO_CORRECT_LOGIC_TYPE)->EnableWindow(Enable);
		GetDlgItem(IDC_COMBO_CORRECT_DIRECTION)->EnableWindow(Enable);
		GetDlgItem(IDC_COMBO_FILTER_TYPE)->EnableWindow(Enable);
		GetDlgItem(IDC_EDIT_ALERT_DIFF)->EnableWindow(Enable);
		GetDlgItem(IDC_EDIT_FILTER_SIZE)->EnableWindow(Enable);
		GetDlgItem(IDC_EDIT_CORRECT_PERCENT)->EnableWindow(Enable);
		GetDlgItem(IDC_EDIT_CORRECT_INTERVAL)->EnableWindow(Enable);
		GetDlgItem(IDC_CHECK_USE_CORRECT)->EnableWindow(Enable);


		GetDlgItem(IDC_CHECK_USE_PLC)->EnableWindow(Enable);
		GetDlgItem(IDC_COMBO_COMMU_TYPE)->EnableWindow(Enable);
		GetDlgItem(IDC_IPADDRESS_IP)->EnableWindow(Enable);
		GetDlgItem(IDC_BUTTON_CONNECT)->EnableWindow(Enable);
		GetDlgItem(IDC_EDIT_PORT)->EnableWindow(Enable);
		GetDlgItem(IDC_EDIT_SINGLE_CORRECT_ADDRESS)->EnableWindow(Enable);
		GetDlgItem(IDC_EDIT_DOUBLE_CORRECT_ADDRESS)->EnableWindow(Enable);
		GetDlgItem(IDC_EDIT_PLC_HEARTBEAT_ADDRESS)->EnableWindow(Enable);

		GetDlgItem(IDC_COMBO_VALUE_TYPE)->EnableWindow(Enable);
		GetDlgItem(IDC_EDIT_ADD)->EnableWindow(Enable);
		GetDlgItem(IDC_BTN_WRITETEST)->EnableWindow(Enable);
		GetDlgItem(IDC_EDIT_VALUE)->EnableWindow(Enable);
		GetDlgItem(IDC_BTN_READ)->EnableWindow(Enable);
		GetDlgItem(IDOK)->EnableWindow(Enable);

	}
	catch (const std::exception&)
	{
	}
	catch (HException&e)
	{
	}
	catch (...)
	{
	}
}

void CDlgParamSet::RefreshButton()
{
	BOOL bRet = FALSE;
	BOOL bChinese = m_pOwner->m_Lang.IsChinese();
	m_pOwner->DoSomething("GetConnection", NULL, &bRet);
	if (bRet)
	{
		((CBCGPButton*)GetDlgItem(IDC_BUTTON_CONNECT))->SetWindowTextA(bChinese ? "已连接" : "Connected");
		((CBCGPButton*)GetDlgItem(IDC_BUTTON_CONNECT))->SetFaceColor(RGB(0, 255, 0));
	}
	else
	{
		((CBCGPButton*)GetDlgItem(IDC_BUTTON_CONNECT))->SetWindowTextA(bChinese ? "未连接" : "Unconnected");
		((CBCGPButton*)GetDlgItem(IDC_BUTTON_CONNECT))->SetFaceColor(RGB(255, 0, 0));
	}
}

void CDlgParamSet::RefreshAddressValueList()
{
	return;
	if (m_pOwner->m_tPLCSet.nUsePlc != CPLC::FinsTCP) return;
	BOOL bRet = FALSE;
	m_pOwner->DoSomething("GetConnection", NULL, &bRet);
	if (bRet == FALSE) return;

	int nNum = (UPS_ALARM - HEARTBEAT) / 2 + 1;
	int nAddr = HEARTBEAT;
	std::vector<int> vecVal;
	vecVal.insert(vecVal.begin(), nNum, 0);
	m_pOwner->DoSomething("ReadInt32Batch", &nAddr, &vecVal);

	CStringArray strArr;
	strArr.Add("心跳信号");
	strArr.Add("报警信号");
	strArr.Add("PLC状态信号");
	strArr.Add("涂布机状态信号");
	strArr.Add("涂布机报警信号");
	strArr.Add("报警复位信号");
	strArr.Add("UPS报警信号");
	//strArr.Add("卷号");

	HWND hwndList = m_LST_Grid_List.GetSafeHwnd();
	SNDMSG(hwndList, WM_SETREDRAW, FALSE, 0);
	m_LST_Grid_List.RemoveAll();

	CBCGPGridRow* pRow;
	for (int i = 0; i < strArr.GetSize(); i++)
	{
		pRow = m_LST_Grid_List.CreateRow(m_LST_Grid_List.GetColumnCount());
		//名称
		pRow->GetItem(0)->SetValue(_variant_t(strArr.GetAt(i)), FALSE);
		CString strName;
		strName.Format("%d", HEARTBEAT + i * 2);
		pRow->GetItem(1)->SetValue(_variant_t(strName), FALSE);
		strName.Format("%d", vecVal[i]);
		pRow->GetItem(2)->SetValue(_variant_t(strName), FALSE);
		m_LST_Grid_List.AddRow(pRow, FALSE);
	}

	//CString strBatch;
	//nAddr = CCD_Batch_Num;
	//m_pOwner->DoSomething("ReadString", &nAddr, &strBatch);
	//pRow = m_LST_Grid_List.CreateRow(m_LST_Grid_List.GetColumnCount());
	////名称
	//pRow->GetItem(0)->SetValue(_variant_t("卷号"), FALSE);
	//CString strName;
	//strName.Format("%d", CCD_Batch_Num);
	//pRow->GetItem(1)->SetValue(_variant_t(strName), FALSE);
	//pRow->GetItem(2)->SetValue(_variant_t(strBatch), FALSE);
	//m_LST_Grid_List.AddRow(pRow, FALSE);

	SNDMSG(hwndList, WM_SETREDRAW, TRUE, 0);
	m_LST_Grid_List.AdjustLayout();

}




void CDlgParamSet::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	m_pOwner->m_tPLCSet.nUsePlc = ((CButton*)GetDlgItem(IDC_CHECK_USE_PLC))->GetCheck() == TRUE ? CPLC::FinsTCP : CPLC::None;
	m_pOwner->m_tPLCSet.nUseCorrect = ((CButton*)GetDlgItem(IDC_CHECK_USE_CORRECT))->GetCheck();
	m_pOwner->m_tPLCSet.nCorrectType = (CPLC::eUseCorrectType)((CBCGPComboBox*)GetDlgItem(IDC_COMO_CORRECT_TYPE))->GetCurSel();
	m_pOwner->m_tPLCSet.nFilterType = (CPLC::eFilterType)((CBCGPComboBox*)GetDlgItem(IDC_COMBO_FILTER_TYPE))->GetCurSel();
	m_pOwner->m_tPLCSet.nCorrectLogicType = (CPLC::eCorrectLogicType)((CBCGPComboBox*)GetDlgItem(IDC_COMO_CORRECT_LOGIC_TYPE))->GetCurSel();
	m_pOwner->m_tPLCSet.nCorrectDirection = (CPLC::eCorrectLogicType)((CBCGPComboBox*)GetDlgItem(IDC_COMBO_CORRECT_DIRECTION))->GetCurSel();
	m_pOwner->m_tPLCSet.nPLCPort = m_EDIT_nPort;
	m_pOwner->m_tPLCSet.strPLCIP = m_EDIT_strIP;
	m_pOwner->m_tPLCSet.nSingleCorrectAddress = m_EDIT_nSingleCorrectAddress;
	m_pOwner->m_tPLCSet.nDoubleCorrectAddress = m_EDIT_nDoubleCorrectAddress;
	m_pOwner->m_tPLCSet.nDoCorrectAddress = m_EDT_nDoCorrectAddr;
	m_pOwner->m_tPLCSet.nPlcCCDEnableAddress = m_EDT_nCCDEnableAddress;
	m_pOwner->m_tPLCSet.nPlcHeartBeatAddress = m_EDIT_nPlcHeartBeatAddress;
	m_pOwner->m_tPLCSet.nPlcBatchNameAddress = m_EDIT_nBatchAddress;
	m_pOwner->m_tPLCSet.nPlcBatchNameVerifyAddress = m_EDIT_nBatchAddressVerify;
	m_pOwner->m_tPLCSet.nPlcRunDirectionAddress = m_EDT_nPLCRunDirectionAddress;
	m_pOwner->m_tPLCSet.nPlcDieHeadDirectionAddress = m_EDT_nPLCDieHeadDirectionAddress;
	m_pOwner->m_tPLCSet.nPlcStartCoatAddress = m_EDT_nPLCStartCoatAddress;
	m_pOwner->m_tPLCSet.nPlcDieHeadPosAddress = m_EDT_nPLCDieHeadPositionAddress;
	m_pOwner->m_tPLCSet.nPlcAlarmAddress = m_EDT_nPLCAlarmAddress;
	m_pOwner->m_tPLCSet.nProductTypeAddress = m_Edit_nProductTypeAddr;
	m_pOwner->m_tPLCSet.nPlcSoftwareVersionAddress = m_Edit_nPlcSoftwareVersionAddr;
	m_pOwner->m_tPLCSet.nReadProductTypeAddress = m_Edit_nReadProductType_Addr;
	

	m_pOwner->m_tPLCSet.dAlertDiff = m_EDIT_dAlertDiff;
	m_pOwner->m_tPLCSet.nFilterSize = m_EDIT_nFilterSize;
	m_pOwner->m_tPLCSet.dCorrectPercent = m_EDIT_dCorrectPercent;
	m_pOwner->m_tPLCSet.dCorrectInterval = m_EDIT_dCorrectInterval;
	m_pOwner->m_tPLCSet.dCorrectIntervalLong = m_EDIT_dCorrectIntervalLong;
	m_pOwner->m_tPLCSet.dCamDistance = m_EDIT_dCamDistance;

	m_pOwner->m_tPLCSet.bJudgeGuidFinish = ((CButton*)GetDlgItem(IDC_CHECK_JUDGE_GUID_FINSH))->GetCheck();
	m_pOwner->m_tPLCSet.nGuidFinishAddr = m_EDT_nGuidFinishAddr;

	//刀距闭环
	m_pOwner->m_tPLCSet.nUseKnifeCorrect = ((CButton*)GetDlgItem(IDC_CHECK_USE_KNIFECORRECT))->GetCheck();
	m_pOwner->m_tPLCSet.nUseWeightData = ((CButton*)GetDlgItem(IDC_CHECK_USE_WEIGHT_DATA))->GetCheck();
	m_pOwner->m_tPLCSet.dKnifeAlertMaxDiff = m_EDIT_dKnifeAlertDiff;
	m_pOwner->m_tPLCSet.dKnifeAlertMinDiff = m_EDIT_dKnifeAlertMinDiff;
	m_pOwner->m_tPLCSet.dUnitKnifeCoatWidth = m_EDIT_dKnifeUnit;
	m_pOwner->m_tPLCSet.dKnifeCorrectPercent = m_EDIT_dKnifeCorrectPercent;
	m_pOwner->m_tPLCSet.dCamKnifeDistance = m_EDIT_dCamKnifeDistance;
	m_pOwner->m_tPLCSet.dKnifeCorrectInterval = m_EDIT_dKnifeCorrectInterval;
	m_pOwner->m_tPLCSet.dKnifeStartIngoreDistance = m_EDIT_dKnifeStartIngoreMeter;
	m_pOwner->m_tPLCSet.dBetaRayDistance = m_EDIT_dBateRayDistance;
	m_pOwner->m_tPLCSet.dBetaRayWeightDiffLimit = m_EDIT_dBetaRayMaxWeightDiff;

	m_pOwner->m_tPLCSet.bJudgeKnifeDoFinish = ((CButton*)GetDlgItem(IDC_CHECK_JUDGE_KNIFE_FINSH))->GetCheck();
	m_pOwner->m_tPLCSet.nPlcLeftKnifeCorrectAddress = m_EDIT_nLeftKnifeCorrectAddr;
	m_pOwner->m_tPLCSet.nPlcRightKnifeCorrectAddress = m_EDIT_nRightKnifeCorrectAddr;
	m_pOwner->m_tPLCSet.nPlcKnifeDoCorrectAddress = m_EDIT_nKnifeCorrectDoAddr;
	m_pOwner->m_tPLCSet.nPlcKnifeDoFinishAddress = m_EDIT_nKnifeCorrectFinishAddr;
	m_pOwner->m_tPLCSet.nPlcLeftKnifePosAddress = m_EDIT_nLeftKnifePosAddr;
	m_pOwner->m_tPLCSet.nPlcRightKnifePosAddress = m_EDIT_nRightKnifePosAddr;

	((CIPAddressCtrl*)GetDlgItem(IDC_IPADDRESS_IP))->GetWindowTextA(m_pOwner->m_tPLCSet.strPLCIP);
	m_pOwner->m_tPLCSet.nUsePlc = (CPLC::eUsePlcType)((CBCGPComboBox*)GetDlgItem(IDC_COMBO_COMMU_TYPE))->GetCurSel();
	m_pOwner->WriteParam();

	CBCGPDialog::OnOK();
}


void CDlgParamSet::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	OnBnClickedButtonClose();
}


void CDlgParamSet::OnBnClickedButtonClose()
{
	// TODO: 在此添加控件通知处理程序代码
	KillTimer(UM_REFRESH_LIST);
	KillTimer(UM_HEART_BEAT);
	CBCGPDialog::OnCancel();
}

/************************************
*@Method:    OnBnClickedBtnWritetest
*@Access:    public
*@Returns:   void
*@Author: 	 Fowindy
*@Created:   2021/02/27 11:59
*@Describe:	 PLC设置界面_写入测试按钮
*************************************/
void CDlgParamSet::OnBnClickedBtnWritetest()
{
	BOOL bRet = FALSE;
	m_pOwner->DoSomething("GetConnection", NULL, &bRet);
	if (!bRet) return;
	CString strAddr, strVal, strMess = NULL;
	GetDlgItem(IDC_EDIT_ADD)->GetWindowTextA(strAddr);
	GetDlgItem(IDC_EDIT_VALUE)->GetWindowTextA(strVal);

	int nSel = ((CComboBox*)GetDlgItem(IDC_COMBO_VALUE_TYPE))->GetCurSel();

	if (CPLC::FinsTCP == m_pOwner->m_tPLCSet.nUsePlc)
	{
		int nAddr = atoi(strAddr);
		int nValue = atoi(strVal);
		if (VT_string == nSel)
		{
			bRet = m_pOwner->m_pFinsTcp->WriteStringBatch(nAddr, strVal);
		}
		else if (VT_float == nSel)
		{
			float fVal = atof(strVal);
			bRet = m_pOwner->m_pFinsTcp->WriteFloat32(nAddr, fVal);
		}
		else
		{
			bRet = m_pOwner->m_pFinsTcp->WriteInt32(nAddr, nValue);
		}
		
		if (!bRet)
		{
			strMess.Format("写入失败，请检查通信!");
			AfxMessageBox(strMess);
		}
	}
	else if (CPLC::ModbusTCP == m_pOwner->m_tPLCSet.nUsePlc)
	{
		int nAddr = atoi(strAddr);
		float fValue = atof(strVal);
		m_pOwner->m_pMdbsTcp->WriteFloat(nAddr, fValue);
	}
	else if (CPLC::AdsTCP == m_pOwner->m_tPLCSet.nUsePlc &&
		m_pOwner->m_pAdsTcp->IsConnected())
	{
		if (VT_bool == nSel)
		{
			std::vector<bool> vec;
			vec.push_back((bool)atoi(strVal));
			m_pOwner->m_pAdsTcp->WriteBoolBatch(strAddr, vec);
		}
		else if (VT_short == nSel)
		{
			std::vector<short> vec;
			vec.push_back((short)atoi(strVal));
			m_pOwner->m_pAdsTcp->WriteShortBatch(strAddr, vec);
		}
		else if (VT_int32 == nSel)
		{
			std::vector<int> vec;
			vec.push_back(atoi(strVal));
			m_pOwner->m_pAdsTcp->WriteInt32Batch(strAddr, vec);
		}
		else if (VT_float == nSel)
		{
			std::vector<float> vec;
			vec.push_back((float)atof(strVal));
			m_pOwner->m_pAdsTcp->WriteFloatBatch(strAddr, vec);
		}
		else if (VT_double == nSel)
		{
			std::vector<double> vec;
			vec.push_back(atof(strVal));
			m_pOwner->m_pAdsTcp->WriteDoubleBatch(strAddr, vec);
		}
		else if (VT_string == nSel)
		{
			m_pOwner->m_pAdsTcp->WriteString(strAddr, strVal);
		}
	}
}


void CDlgParamSet::OnBnClickedBtnRead()
{
	BOOL bRet = FALSE;
	m_pOwner->DoSomething("GetConnection", NULL, &bRet);
	if (!bRet) return;
	CString strAddr, strVal = "", strMess = NULL;
	GetDlgItem(IDC_EDIT_ADD)->GetWindowTextA(strAddr);
	int nSel = ((CComboBox*)GetDlgItem(IDC_COMBO_VALUE_TYPE))->GetCurSel();//确认下拉列表选中的项
	if (CPLC::FinsTCP == m_pOwner->m_tPLCSet.nUsePlc)
	{
		int nAddr = atoi(strAddr);
		if (VT_string == nSel)
		{
			bRet = m_pOwner->m_pFinsTcp->ReadStringBatchInverse(nAddr, 18, strVal);
		}
		else if (VT_float == nSel)
		{
			float fVal = 0;
			bRet = m_pOwner->m_pFinsTcp->ReadFloat32(nAddr, fVal);
			strVal.Format("%f", fVal);
		}
		else
		{
			int nValue;
			bRet = m_pOwner->m_pFinsTcp->ReadInt32(nAddr, nValue);
			strVal.Format("%d", nValue);
		}
		if (!bRet)
		{
			strMess.Format("读取失败，请检查通信!");
			AfxMessageBox(strMess);
		}
	}
	else if (CPLC::ModbusTCP == m_pOwner->m_tPLCSet.nUsePlc)
	{
		int nAddr = atoi(strAddr);
		float fVal;
		m_pOwner->m_pMdbsTcp->ReadFloat(nAddr, fVal);
		strVal.Format("%0.3f", fVal);
	}
	else if (CPLC::AdsTCP == m_pOwner->m_tPLCSet.nUsePlc &&
		m_pOwner->m_pAdsTcp->IsConnected())
	{
		if (VT_bool == nSel)
		{
			std::vector<bool> vec;
			BOOL bRet = m_pOwner->m_pAdsTcp->ReadBoolBatch(strAddr, 1, vec);
			if (bRet)
				strVal.Format("%d", (int)vec[0]);
			else
				strVal = "";

		}
		else if (VT_short == nSel)
		{
			std::vector<short> vec;
			BOOL bRet = m_pOwner->m_pAdsTcp->ReadShortBatch(strAddr, 1, vec);
			if (bRet)
				strVal.Format("%d", (int)vec[0]);
			else
				strVal = "";
		}
		else if (VT_int32 == nSel)
		{
			std::vector<int> vec;
			BOOL bRet = m_pOwner->m_pAdsTcp->ReadInt32Batch(strAddr, 1, vec);
			if (bRet)
				strVal.Format("%d", (int)vec[0]);
			else
				strVal = "";
		}
		else if (VT_float == nSel)
		{
			std::vector<float> vec;
			BOOL bRet = m_pOwner->m_pAdsTcp->ReadFloatBatch(strAddr, 1, vec);
			if (bRet)
				strVal.Format("%0.3f", (int)vec[0]);
			else
				strVal = "";
		}
		else if (VT_double == nSel)
		{
			std::vector<double> vec;
			BOOL bRet = m_pOwner->m_pAdsTcp->ReadDoubleBatch(strAddr, 1, vec);
			if (bRet)
				strVal.Format("%0.3f", vec[0]);
			else
				strVal = "";
		}
		else if (VT_string == nSel)
		{
			m_pOwner->m_pAdsTcp->ReadString(strAddr, 40, strVal);
		}
	}
	GetDlgItem(IDC_EDIT_VALUE)->SetWindowTextA(strVal);
}


/************************************
*@Method:    OnBnClickedButtonConnect
*@Access:    public
*@Returns:   void
*@Author: 	 Fowindy
*@Created:   2021/03/07 14:01
*@Describe:	 PLC界面连接按钮
*************************************/
void CDlgParamSet::OnBnClickedButtonConnect()
{
	UpdateData(TRUE);
	m_pOwner->m_tPLCSet.nUsePlc = ((CButton*)GetDlgItem(IDC_CHECK_USE_PLC))->GetCheck() == TRUE ? CPLC::FinsTCP : CPLC::None;
	m_pOwner->m_tPLCSet.nPLCPort = m_EDIT_nPort;
	m_pOwner->m_tPLCSet.strPLCIP = m_EDIT_strIP;
	//((CIPAddressCtrl*)GetDlgItem(IDC_IPADDRESS_IP))->GetWindowTextA(m_pOwner->m_tPLCSet.strPLCIP);

	BOOL bRet = FALSE;
	{
		m_pOwner->m_pFinsTcp->Disconnect();
		m_pOwner->m_pFinsTcp->Connect(m_pOwner->m_tPLCSet.strPLCIP, m_pOwner->m_tPLCSet.nPLCPort, CT_CLIENT);
		m_pOwner->DoSomething("GetConnection", NULL, &bRet);
		if (bRet)
		{
			((CBCGPButton*)GetDlgItem(IDC_BUTTON_CONNECT))->SetWindowTextA("Connected");
			((CBCGPButton*)GetDlgItem(IDC_BUTTON_CONNECT))->SetFaceColor(RGB(0, 255, 0));
		}
		else
		{
			((CBCGPButton*)GetDlgItem(IDC_BUTTON_CONNECT))->SetWindowTextA("Unconnected");
			((CBCGPButton*)GetDlgItem(IDC_BUTTON_CONNECT))->SetFaceColor(RGB(255, 0, 0));
		}
	}
}

void CDlgParamSet::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (UM_REFRESH_LIST == nIDEvent)
	{
		RefreshButton();
		RefreshAddressValueList();
	}
	/*if (UM_HEART_BEAT == nIDEvent)
	{
		int nAddr = HEARTBEAT;
		int nVal = 1;
		m_pOwner->DoSomething("WRITEINT32", &nAddr, &nVal);
	}*/
	CBCGPDialog::OnTimer(nIDEvent);
}
