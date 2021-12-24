// PageDispList3.cpp : 实现文件
//

#include "stdafx.h"
#include "BatteriesVision.h"
#include "PageDispList3.h"
#include "afxdialogex.h"


// CPageDispList3 对话框

IMPLEMENT_DYNAMIC(CPageDispList3, CBCGPDialog)

CPageDispList3::CPageDispList3(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(CPageDispList3::IDD, pParent)
	, m_EDIT_nSpotInspectionInterval(0)
	, m_EDIT_nSpotInspDataNum(0)
	, m_EDIT_nSpotInspNoDataNum(0)
	, m_EDIT_dMaxTemp(0)
{
	theGlobal.m_pSpotInspection = this;
}

CPageDispList3::~CPageDispList3()
{
}

void CPageDispList3::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SPOTINSPECTION_INTERVAL, m_EDIT_nSpotInspectionInterval);
	DDX_Text(pDX, IDC_EDIT_SPOTINSPECTION_DATANUM, m_EDIT_nSpotInspDataNum);
	DDX_Text(pDX, IDC_EDIT_SPOTINSPECTION_NODATANUM, m_EDIT_nSpotInspNoDataNum);
	DDX_Text(pDX, IDC_EDIT_MAX_TEMP, m_EDIT_dMaxTemp);
}


BEGIN_MESSAGE_MAP(CPageDispList3, CBCGPDialog)
	ON_WM_ERASEBKGND()
	ON_EN_CHANGE(IDC_EDIT_SPOTINSPECTION_INTERVAL, &CPageDispList3::OnEnChangeEditSpotinspectionInterval)
	ON_EN_CHANGE(IDC_EDIT_SPOTINSPECTION_DATANUM, &CPageDispList3::OnEnChangeEditSpotinspectionDatanum)
	ON_EN_CHANGE(IDC_EDIT_SPOTINSPECTION_NODATANUM, &CPageDispList3::OnEnChangeEditSpotinspectionNodatanum)
	ON_BN_CLICKED(IDC_BUTTON_SPOTINSPECTION, &CPageDispList3::OnBnClickedButtonSpotinspection)
	ON_WM_PAINT()
	ON_EN_CHANGE(IDC_EDIT_MAX_TEMP, &CPageDispList3::OnEnChangeEditMaxTemp)
	ON_BN_CLICKED(IDC_CHECK_USE_TEMP, &CPageDispList3::OnBnClickedCheckUseTemp)
	ON_REGISTERED_MESSAGE(WM_SPOTINSPECTION_REFRESH, &CPageDispList3::OnSpotInspectionRefresh)
END_MESSAGE_MAP()


// CPageDispList3 消息处理程序


BOOL CPageDispList3::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	EnableVisualManagerStyle(TRUE,TRUE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


BOOL CPageDispList3::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if(pMsg->message==WM_KEYDOWN || pMsg->message == WM_SYSKEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN)//屏蔽回车
			return TRUE;
		else if (pMsg->wParam == VK_ESCAPE)//屏蔽ESC
			return TRUE;
		else if(pMsg->wParam == VK_SPACE)//
			return TRUE;
		else if (pMsg->wParam == VK_F4  && HIBYTE(::GetKeyState(VK_MENU)))//屏蔽ALT+F4
			return TRUE;
	}
	return CBCGPDialog::PreTranslateMessage(pMsg);
}


void CPageDispList3::PostNcDestroy()
{
	// TODO: 在此添加专用代码和/或调用基类
	delete this;
	CBCGPDialog::PostNcDestroy();
}

void CPageDispList3::ResetSize()
{
	InitUI();
}

void CPageDispList3::InitUI()
{
	CBCGPGridColors theme;
	CRect rcGridBase, rcDlg, rcButton;
	GetWindowRect(rcDlg);
	ScreenToClient(rcDlg);
	GetDlgItem(IDC_BUTTON_SPOTINSPECTION)->GetWindowRect(rcButton);
	ScreenToClient(rcButton);
	rcGridBase = rcDlg;
	rcGridBase.top = rcButton.bottom + 15;
	m_LST_Grid_List.Create (WS_CHILD, rcGridBase, this, 1000);
	CBCGPVisualManager::GetInstance ()->OnSetGridColorTheme(&m_LST_Grid_List, theme);
	m_LST_Grid_List.SendMessage(BCGM_ONSETCONTROLVMMODE, TRUE);
	m_LST_Grid_List.SetColorTheme(theme);
	ResetList();

	m_EDIT_nSpotInspectionInterval = theGlobal.m_Judge.m_tSpotInspParam.nSpotInspInterval;
	m_EDIT_nSpotInspDataNum = theGlobal.m_Judge.m_tSpotInspParam.nCountinousDataNum;
	m_EDIT_nSpotInspNoDataNum = theGlobal.m_Judge.m_tSpotInspParam.nNoDataLimit;
	m_EDIT_dMaxTemp = theGlobal.m_Judge.m_tSpotInspParam.dMaxTemperature;
	((CButton*)GetDlgItem(IDC_CHECK_USE_TEMP))->SetCheck(theGlobal.m_Judge.m_tSpotInspParam.bUseTemp);
	UpdateData(FALSE);
}

void CPageDispList3::ResetList()
{
	BOOL bChinese = theGlobal.m_Language.IsChinese();
	m_LST_Grid_List.RemoveAll();
	m_LST_Grid_List.DeleteAllColumns();
	m_LST_Grid_List.SetReadOnly();//可编辑
	m_LST_Grid_List.EnableMarkSortedColumn (FALSE);//使用列排序
	m_LST_Grid_List.EnableHeader (TRUE, BCGP_GRID_HEADER_MOVE_ITEMS);//使用表头
	m_LST_Grid_List.EnableRowHeader (FALSE);//使用行表头
	m_LST_Grid_List.EnableLineNumbers (FALSE);//使用行序号
	m_LST_Grid_List.SetClearInplaceEditOnEnter (FALSE);
	m_LST_Grid_List.EnableInvertSelOnCtrl (FALSE);
	//m_LST_Grid_List.SetScalingRange (0.1, 2.0);//设置缩放比例，按Ctrl键和鼠标滚轮可缩放
	m_LST_Grid_List.InsertColumn(0, "点检/时间", 150);
	//点检
	int nCount = 1;
	//for (int i = 0; i < theGlobal.m_Project.GetProjectConfig().nSideCount; i++)
	{
		CString strName;
		strName.Format("工位");
		m_LST_Grid_List.InsertColumn(nCount++, strName, 70);
		strName.Format("点检结果");
		m_LST_Grid_List.InsertColumn(nCount++, strName, 70);
		strName.Format("圆心距1实测与标准差值(mm)");
		m_LST_Grid_List.InsertColumn(nCount++, strName, 170);
		strName.Format("圆心距2实测与标准差值(mm)");
		m_LST_Grid_List.InsertColumn(nCount++, strName, 170);
		strName.Format("圆心距3实测与标准差值(mm)");
		m_LST_Grid_List.InsertColumn(nCount++, strName, 170);
		strName.Format("温度(℃)");
		m_LST_Grid_List.InsertColumn(nCount++, strName, 80);
	}
	m_LST_Grid_List.LoadState(_T("BasicGrid"));
	m_LST_Grid_List.AdjustLayout();//相当于数据显示在表格上
	m_LST_Grid_List.ShowWindow(SW_NORMAL);
}

void CPageDispList3::UpdateList(C_MergeWork& tOutput)
{
	return;
	tagPDInfo tPdInfo = theGlobal.m_ProductTool.GetCurrentProductInfo();
	if (!tPdInfo.tParam.bUseChecking || theGlobal.m_tSystemSet.nActionMode != RunMode) return;
	if (!IsWindowVisible()) return;

	SYSTEMTIME tTm;
	GetLocalTime(&tTm);
	CString strTime, strVal;
	strTime.Format("%4d/%2d/%2d %2d:%2d:%2d", tTm.wYear, tTm.wMonth, tTm.wDay, tTm.wHour, tTm.wMinute, tTm.wSecond);

	/*HWND hwndList = m_LST_Grid_List.GetSafeHwnd();
	SNDMSG(hwndList, WM_SETREDRAW, FALSE, 0);*/
	COLORREF colTmp[3];
	colTmp[OK] = RGB(255, 255, 255);
	colTmp[NG] = RGB(255, 0, 0);
	colTmp[WARN] = RGB(248, 182, 43);

	//判定数据是否有效
	HTuple hvSize;
	for (int nSideSN = 0; nSideSN < tOutput.vecB.size(); nSideSN++)
	{
		hvSize.Append(tOutput.vecB[nSideSN].tMeasure.tSpotInspection.hv_Width);
	}
	HTuple hvInc;
	TupleFind(hvSize, NO_DATA, &hvInc);
	if ((hvInc.Length() >= 1 && hvInc != -1) || hvInc.Length() == 0 || hvSize.Length() != tOutput.vecB.size() * 7)
	{
		return;
	}

	//获取温度
	BOOL bConnect = TRUE;
	theGlobal.m_CommunicationTool.DoSomething(CCommunicationTool::Temperature, "GetConnection", &bConnect, NULL);
	std::vector<unsigned short> vTempList;
	vTempList.resize(theGlobal.m_CommunicationTool.m_nTempertatureNums, 0);
	theGlobal.m_CommunicationTool.DoSomething(CCommunicationTool::Temperature, "GetCurrentTemperature", &vTempList, NULL);

	//点检
	if (tOutput.vecB[0].tMeasure.tSpotInspection.hv_Width.Length() == 7)
	{
		CBCGPGridRow* pRow = m_LST_Grid_List.CreateRow(m_LST_Grid_List.GetColumnCount());
		pRow->GetItem(0)->SetValue(_variant_t(strTime), FALSE);
		int nCount = 1;
		for (int nBWorkSN = 0; nBWorkSN < tOutput.vecB.size(); nBWorkSN++)
		{
			HTuple hv = tOutput.vecB[nBWorkSN].tMeasure.tSpotInspection.hv_Width;
			for (int i = 0; i < 7; i++)
			{
				if (tOutput.vecB[nBWorkSN].tMeasure.tSpotInspection.hv_Width.Length() > i
					&& tOutput.vecB[nBWorkSN].tMeasure.tSpotInspection.hv_Width[i] != NO_DATA)
				{
					pRow->GetItem(nCount)->SetValue(_variant_t((tOutput.vecB[nBWorkSN].tMeasure.tSpotInspection.hv_Width[i].D())), FALSE);
					if (tOutput.tMerge.vecSideResult[nBWorkSN].hv_SpotInspectionFlagList[i].I() == NG)
						pRow->GetItem(nCount)->SetBackgroundColor(colTmp[NG], FALSE);
				}
				else
				{
					pRow->GetItem(nCount)->SetValue(_variant_t("-"), FALSE);
				}
				nCount++;
			}
			if (theGlobal.m_Judge.m_tSpotInspParam.bUseTemp && bConnect)
			{
				if (vTempList.size() > nBWorkSN)
					pRow->GetItem(nCount)->SetValue(_variant_t(vTempList[nBWorkSN]), FALSE);
				else
					pRow->GetItem(nCount)->SetValue(_variant_t("-"), FALSE);
			}
			else
			{
				pRow->GetItem(nCount)->SetValue(_variant_t("-"), FALSE);
			}
			nCount++;
		}
		
		m_LST_Grid_List.InsertRowBefore(0, pRow, FALSE);
	}

	if (m_LST_Grid_List.GetRowCount() > MAX_LIST_COUNT)
		m_LST_Grid_List.RemoveRow(m_LST_Grid_List.GetRowCount() - 1, FALSE);

	m_LST_Grid_List.AdjustLayout();
}

void CPageDispList3::RefreshList()
{
	tagPDInfo tPdInfo = theGlobal.m_ProductTool.GetCurrentProductInfo();
	if (!tPdInfo.tParam.bUseChecking || theGlobal.m_tSystemSet.nActionMode != RunMode) return;

	SYSTEMTIME tTm;
	GetLocalTime(&tTm);
	CString strTime, strVal;
	strTime.Format("%04d/%02d/%02d %2d:%2d:%2d", tTm.wYear, tTm.wMonth, tTm.wDay, tTm.wHour, tTm.wMinute, tTm.wSecond);

	/*HWND hwndList = m_LST_Grid_List.GetSafeHwnd();
	SNDMSG(hwndList, WM_SETREDRAW, FALSE, 0);*/
	COLORREF colTmp[3];
	colTmp[OK] = RGB(0, 255, 0);
	colTmp[NG] = RGB(255, 0, 0);
	colTmp[WARN] = RGB(248, 182, 43);

	//判定数据是否有效
	HTuple hvSize;
	for (int nSideSN = 0; nSideSN < (tPdInfo.nCoatingType+1); nSideSN++)
	{
		CBCGPGridRow* pRow = m_LST_Grid_List.CreateRow(m_LST_Grid_List.GetColumnCount());
		
		CString strValue;
		double dValue=-1;
		//时间
		pRow->GetItem(0)->SetValue(_variant_t(strTime), FALSE);
		//工位
		strValue.Format("%s", nSideSN == 0 ? "正面" : "反面");
		pRow->GetItem(1)->SetValue(_variant_t(strValue), FALSE);
		//点检结果
		strValue.Format("%s", m_tSpotInspCount.hvFlagSingle[nSideSN] == OK ? "OK" : "NG");
		pRow->GetItem(2)->SetValue(_variant_t(strValue), FALSE);
		pRow->GetItem(2)->SetTextColor(m_tSpotInspCount.hvFlagSingle[nSideSN] == OK ? colTmp[OK] : colTmp[NG], FALSE);
		//圆心距1实测与标准差值
		if (m_tSpotInspCount.hvDataMedian[nSideSN][2].D() < 0)
		{
			strValue = " ";
		}
		else
		{
			dValue = m_tSpotInspCount.hvDataMedian[nSideSN][2].D() - 
				(nSideSN == 0 ? tPdInfo.tParam.vecChecking[2].dStandard : tPdInfo.tParam.vecChecking[2].dStandard2);
			strValue.Format("%0.3f", dValue);
		}
		pRow->GetItem(3)->SetValue(_variant_t(strValue), FALSE);
		//圆心距2实测与标准差值
		if (m_tSpotInspCount.hvDataMedian[nSideSN][5].D() < 0)
		{
			strValue = " ";
		}
		else
		{
			dValue = m_tSpotInspCount.hvDataMedian[nSideSN][5].D() -
				(nSideSN == 0 ? tPdInfo.tParam.vecChecking[5].dStandard : tPdInfo.tParam.vecChecking[5].dStandard2);
			strValue.Format("%0.3f", dValue);
		}
		pRow->GetItem(4)->SetValue(_variant_t(strValue), FALSE);
		//圆心距3实测与标准差值
		if (m_tSpotInspCount.hvDataMedian[nSideSN][6].D() < 0)
		{
			strValue = " ";
		}
		else
		{
			dValue = m_tSpotInspCount.hvDataMedian[nSideSN][6].D() -
				(nSideSN == 0 ? tPdInfo.tParam.vecChecking[6].dStandard : tPdInfo.tParam.vecChecking[6].dStandard2);
			strValue.Format("%0.3f", dValue);
		}
		pRow->GetItem(5)->SetValue(_variant_t(strValue), FALSE);
		//温度
		if (theGlobal.m_Judge.m_tSpotInspParam.bUseTemp && m_tSpotInspCount.hvTempMedian[nSideSN].Length() > 0 && m_tSpotInspCount.hvTempMedian[nSideSN].D() > 0)
		{
			strValue.Format("%0.3f", m_tSpotInspCount.hvTempMedian[nSideSN].D());
		}
		else
		{
			strValue = " ";
		}
		pRow->GetItem(6)->SetValue(_variant_t(strValue), FALSE);
		m_LST_Grid_List.InsertRowBefore(0, pRow, FALSE);
	}

	if (m_LST_Grid_List.GetRowCount() > MAX_LIST_COUNT)
		m_LST_Grid_List.RemoveRow(m_LST_Grid_List.GetRowCount() - 1, FALSE);

	m_LST_Grid_List.AdjustLayout();
}

BOOL CPageDispList3::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//return TRUE;
	return CBCGPDialog::OnEraseBkgnd(pDC);
}


void CPageDispList3::OnEnChangeEditSpotinspectionInterval()
{
	UpdateData(TRUE);
	theGlobal.m_Log.Operate(theGlobal.m_Judge.m_tSpotInspParam.nSpotInspInterval, m_EDIT_nSpotInspectionInterval, "自动点检间隔时间(s)");
	theGlobal.m_Judge.m_tSpotInspParam.nSpotInspInterval = m_EDIT_nSpotInspectionInterval;
	theGlobal.m_Judge.WriteParam();
}


void CPageDispList3::OnEnChangeEditSpotinspectionDatanum()
{
	UpdateData(TRUE);
	theGlobal.m_Log.Operate(theGlobal.m_Judge.m_tSpotInspParam.nCountinousDataNum, m_EDIT_nSpotInspDataNum, "自动点检连续采集数据量(组)");
	theGlobal.m_Judge.m_tSpotInspParam.nCountinousDataNum = m_EDIT_nSpotInspDataNum;
	theGlobal.m_Judge.WriteParam();
}


void CPageDispList3::OnEnChangeEditSpotinspectionNodatanum()
{
	UpdateData(TRUE);
	theGlobal.m_Log.Operate(theGlobal.m_Judge.m_tSpotInspParam.nNoDataLimit, m_EDIT_nSpotInspNoDataNum, "自动点检最大无数据量(报警)");
	theGlobal.m_Judge.m_tSpotInspParam.nNoDataLimit = m_EDIT_nSpotInspNoDataNum;
	theGlobal.m_Judge.WriteParam();
}


void CPageDispList3::OnBnClickedButtonSpotinspection()
{
	if (IDYES != theGlobal.m_Language.MessageboxQuestion("是否进行手动点检?", "Whether to spot inspection?", ""))
		return;
	while (m_LST_Grid_List.GetRowCount() > 0)
	{
		m_LST_Grid_List.RemoveRow(0, FALSE);
	}
	m_LST_Grid_List.AdjustLayout();

	theGlobal.m_Judge.m_hvLastSpotInspTime = 0;
	theGlobal.m_Judge.m_tSpotInspParam.bStart = TRUE;
}

void CPageDispList3::UIPowerLimit()
{
	BOOL bEnable = theGlobal.m_Permission.CheckPermission(PM_Detect);
	GetDlgItem(IDC_EDIT_SPOTINSPECTION_INTERVAL)->EnableWindow(bEnable);
	GetDlgItem(IDC_EDIT_SPOTINSPECTION_DATANUM)->EnableWindow(bEnable);
	GetDlgItem(IDC_EDIT_SPOTINSPECTION_NODATANUM)->EnableWindow(bEnable);
	GetDlgItem(IDC_CHECK_USE_TEMP)->EnableWindow(bEnable);
	GetDlgItem(IDC_EDIT_MAX_TEMP)->EnableWindow(bEnable);
}

void CPageDispList3::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 在此处添加消息处理程序代码
					   // 不为绘图消息调用 CBCGPDialog::OnPaint()

	UIPowerLimit();
}

void CPageDispList3::OnEnChangeEditMaxTemp()
{
	UpdateData(TRUE);
	theGlobal.m_Log.Operate(theGlobal.m_Judge.m_tSpotInspParam.dMaxTemperature, m_EDIT_dMaxTemp, "点检最小温度限制(℃)");
	theGlobal.m_Judge.m_tSpotInspParam.dMaxTemperature = m_EDIT_dMaxTemp;
	theGlobal.m_Judge.WriteParam();
}


void CPageDispList3::OnBnClickedCheckUseTemp()
{
	UpdateData(TRUE);
	theGlobal.m_Log.Operate(theGlobal.m_Judge.m_tSpotInspParam.bUseTemp, ((CButton*)GetDlgItem(IDC_CHECK_USE_TEMP))->GetCheck(), "使用温度限制");
	theGlobal.m_Judge.m_tSpotInspParam.bUseTemp = ((CButton*)GetDlgItem(IDC_CHECK_USE_TEMP))->GetCheck();
	theGlobal.m_Judge.WriteParam();
}

LRESULT CPageDispList3::OnSpotInspectionRefresh(WPARAM wParam, LPARAM lParam)
{
	try
	{
		RefreshList();
	}
	catch (HException&e)
	{
		CString strException;
		strException.Format("CPageDispList3_OnSpotInspectionRefresh: 刷新点检列表异常:%s  [ %s() ]", e.ErrorMessage().Text()
			, e.ProcName().Text());
		theGlobal.m_Log.Exception(strException);
	}
	
	return 0L;
}