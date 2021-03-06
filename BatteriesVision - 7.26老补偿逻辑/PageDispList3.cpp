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
{

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
}


BEGIN_MESSAGE_MAP(CPageDispList3, CBCGPDialog)
	ON_WM_ERASEBKGND()
	ON_EN_CHANGE(IDC_EDIT_SPOTINSPECTION_INTERVAL, &CPageDispList3::OnEnChangeEditSpotinspectionInterval)
	ON_EN_CHANGE(IDC_EDIT_SPOTINSPECTION_DATANUM, &CPageDispList3::OnEnChangeEditSpotinspectionDatanum)
	ON_EN_CHANGE(IDC_EDIT_SPOTINSPECTION_NODATANUM, &CPageDispList3::OnEnChangeEditSpotinspectionNodatanum)
	ON_BN_CLICKED(IDC_BUTTON_SPOTINSPECTION, &CPageDispList3::OnBnClickedButtonSpotinspection)
	ON_WM_PAINT()
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
	m_LST_Grid_List.InsertColumn(0, "点检/时间", 120);
	//点检
	int nCount = 1;
	for (int i = 0; i < theGlobal.m_Project.GetProjectConfig().nSideCount; i++)
	{
		CString strName;
		strName.Format("%s左宽(mm)", i == 0 ? "正" : "反");
		m_LST_Grid_List.InsertColumn(nCount++, strName, 80);
		strName.Format("%s左高(mm)", i == 0 ? "正" : "反");
		m_LST_Grid_List.InsertColumn(nCount++, strName, 80);
		strName.Format("%s右宽(mm)", i == 0 ? "正" : "反");
		m_LST_Grid_List.InsertColumn(nCount++, strName, 80);
		strName.Format("%s右高(mm)", i == 0 ? "正" : "反");
		m_LST_Grid_List.InsertColumn(nCount++, strName, 80);
		strName.Format("%s间距(mm)", i == 0 ? "正" : "反");
		m_LST_Grid_List.InsertColumn(nCount++, strName, 80);
	}
	m_LST_Grid_List.LoadState(_T("BasicGrid"));
	m_LST_Grid_List.AdjustLayout();//相当于数据显示在表格上
	m_LST_Grid_List.ShowWindow(SW_NORMAL);
}

void CPageDispList3::UpdateList(C_MergeWork& tOutput)
{
	tagPDInfo tPdInfo = theGlobal.m_ProductTool.GetCurrentProductInfo();
	if (!tPdInfo.tParam.bUseChecking || theGlobal.m_tSystemSet.nActionMode != RunMode) return;
	if (!IsWindowVisible()) return;

	SYSTEMTIME tTm;
	GetLocalTime(&tTm);
	CString strTime, strVal;
	strTime.Format("%2d时%2d分%2d秒%3d毫秒", tTm.wHour, tTm.wMinute, tTm.wSecond, tTm.wMilliseconds);

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
	if ((hvInc.Length() >= 1 && hvInc != -1) || hvInc.Length() == 0 || hvSize.Length() != 10)
	{
		return;
	}

	//点检
	if (tOutput.vecB[0].tMeasure.tSpotInspection.hv_Width.Length() == 5)
	{
		CBCGPGridRow* pRow = m_LST_Grid_List.CreateRow(m_LST_Grid_List.GetColumnCount());
		pRow->GetItem(0)->SetValue(_variant_t(strTime), FALSE);
		int nCount = 1;
		for (int nBWorkSN = 0; nBWorkSN < tOutput.vecB.size(); nBWorkSN++)
		{
			HTuple hv = tOutput.vecB[nBWorkSN].tMeasure.tSpotInspection.hv_Width;
			for (int i = 0; i < 5; i++)
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
		}
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
}

void CPageDispList3::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 在此处添加消息处理程序代码
					   // 不为绘图消息调用 CBCGPDialog::OnPaint()

	UIPowerLimit();
}
