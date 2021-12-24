// PageDispList4.cpp : 实现文件
//

#include "stdafx.h"
#include "BatteriesVision.h"
#include "PageDispList4.h"
#include "afxdialogex.h"


// CPageDispList4 对话框

IMPLEMENT_DYNAMIC(CPageDispList4, CBCGPDialog)

CPageDispList4::CPageDispList4(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(CPageDispList4::IDD, pParent)
	, m_EDIT_dFirstArticleMeter(0)
{

}

CPageDispList4::~CPageDispList4()
{
}

void CPageDispList4::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SPOTINSPECTION_TIME, m_EDIT_dFirstArticleMeter);
}


BEGIN_MESSAGE_MAP(CPageDispList4, CBCGPDialog)
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_BUTTON_FAI, &CPageDispList4::OnBnClickedButtonFai)
	ON_EN_CHANGE(IDC_EDIT_SPOTINSPECTION_TIME, &CPageDispList4::OnEnChangeEditSpotinspectionTime)
END_MESSAGE_MAP()


// CPageDispList4 消息处理程序


BOOL CPageDispList4::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	EnableVisualManagerStyle(TRUE,TRUE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


BOOL CPageDispList4::PreTranslateMessage(MSG* pMsg)
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


void CPageDispList4::PostNcDestroy()
{
	// TODO: 在此添加专用代码和/或调用基类
	delete this;
	CBCGPDialog::PostNcDestroy();
}

void CPageDispList4::ResetSize()
{
	InitUI();
}

void CPageDispList4::InitUI()
{
	/*GetDlgItem(IDC_BUTTON_FAI)->ShowWindow(SW_NORMAL);
	GetDlgItem(IDC_STATIC_CH_PRODUCTPARAM)->ShowWindow(SW_NORMAL);
	GetDlgItem(IDC_STATIC_CH_SPOTINSPECTION_TIME)->ShowWindow(SW_NORMAL);
	GetDlgItem(IDC_EDIT_SPOTINSPECTION_TIME)->ShowWindow(SW_NORMAL);*/

	CBCGPGridColors theme;
	CRect rcGridBase,rcDlg,rcButton;
	GetWindowRect(rcDlg);
	ScreenToClient(rcDlg);
	GetDlgItem(IDC_BUTTON_FAI)->GetWindowRect(rcButton);
	ScreenToClient(rcButton);
	rcGridBase = rcDlg;
	rcGridBase.top = rcButton.bottom + 15;
	m_LST_Grid_List.Create (WS_CHILD, rcGridBase, this, 1000);
	CBCGPVisualManager::GetInstance ()->OnSetGridColorTheme(&m_LST_Grid_List, theme);
	m_LST_Grid_List.SendMessage(BCGM_ONSETCONTROLVMMODE, TRUE);
	m_LST_Grid_List.SetColorTheme(theme);
	ResetList();

	m_EDIT_dFirstArticleMeter = theGlobal.m_Judge.m_tParam.dFirstArticleInspMeter;
	UpdateData(FALSE);
}

void CPageDispList4::ResetList()
{
	tagPDInfo tPdInfo = theGlobal.m_ProductTool.GetCurrentProductInfo();
	//正面测量值
	m_LST_Grid_List.RemoveAll();
	m_LST_Grid_List.DeleteAllColumns();
	m_LST_Grid_List.SetReadOnly();//可编辑
	m_LST_Grid_List.EnableMarkSortedColumn(FALSE);//使用列排序
	m_LST_Grid_List.EnableHeader(TRUE, BCGP_GRID_HEADER_MOVE_ITEMS);//使用表头
	m_LST_Grid_List.EnableRowHeader(FALSE);//使用行表头
	m_LST_Grid_List.EnableLineNumbers(FALSE);//使用行序号
	m_LST_Grid_List.SetClearInplaceEditOnEnter(FALSE);
	m_LST_Grid_List.EnableInvertSelOnCtrl(FALSE);
	//m_LST_Grid_List.SetScalingRange(0.1, 2.0);//设置缩放比例，按Ctrl键和鼠标滚轮可缩放
	int nCount = 0;
	m_LST_Grid_List.InsertColumn(nCount++, "时间", 140);
	m_LST_Grid_List.InsertColumn(nCount++, "图像序号", 70);

	for (int i = 0; i < tPdInfo.vecNameList.size(); i++)
	{
		m_LST_Grid_List.InsertColumn(nCount++, CString("正面" + tPdInfo.vecNameList[i]), 70);
	}
	if (tPdInfo.nCoatingType == TwoSide)
	{
		for (int i = 0; i < tPdInfo.vecNameList.size(); i++)
		{
			m_LST_Grid_List.InsertColumn(nCount++, CString("反面" + tPdInfo.vecNameList[i]), 70);
		}
		for (int i = 0; i < tPdInfo.tParam.vecMisalignment.size(); i++)
		{
			CString strName;
			strName.Format("错位值%d", i + 1);
			m_LST_Grid_List.InsertColumn(nCount++, strName, 70);
		}
	}

	m_LST_Grid_List.LoadState(_T("BasicGrid"));
	m_LST_Grid_List.AdjustLayout();//相当于数据显示在表格上
	m_LST_Grid_List.ShowWindow(SW_NORMAL);
}

void CPageDispList4::UpdateList(C_MergeWork& tOutput)
{
	if (!theGlobal.m_Judge.m_tParam.bUseFirstArticleInsp) return;

	/*if (theGlobal.m_Project.GetProjectConfig().nCamCount == 4 && theGlobal.m_Project.GetProjectConfig().nSideCount == 2)
		return;
	if (theGlobal.m_Project.GetProjectConfig().nCamCount == 2 && theGlobal.m_Project.GetProjectConfig().nSideCount == 1)
		return;*/

	tagPDInfo tPdInfo = theGlobal.m_ProductTool.GetCurrentProductInfo();
	SYSTEMTIME tTm;
	GetLocalTime(&tTm);
	CString strTime, strVal;
	strTime.Format("%2d时%2d分%2d秒%3d毫秒", tTm.wHour, tTm.wMinute, tTm.wSecond, tTm.wMilliseconds);

	if (theGlobal.m_Speed.GetSpeed() > 39 && theGlobal.m_Speed.GetSpeed() <= 50 && tOutput.vecB[0].vecA[0].hvImageSN.I() % 2 != 0) return;
	if (theGlobal.m_Speed.GetSpeed() > 50 && theGlobal.m_Speed.GetSpeed() <= 70 && tOutput.vecB[0].vecA[0].hvImageSN.I() % 3 != 0) return;
	if (theGlobal.m_Speed.GetSpeed() > 70 && tOutput.vecB[0].vecA[0].hvImageSN.I() % 4 != 0) return;
	if (!IsWindowVisible()) return;


	/*HWND hwndList = m_LST_Grid_List.GetSafeHwnd();
	SNDMSG(hwndList, WM_SETREDRAW, FALSE, 0);*/
	COLORREF colTmp[3];
	colTmp[OK] = RGB(255, 255, 255);
	colTmp[NG] = RGB(255, 0, 0);
	colTmp[WARN] = RGB(248, 182, 43);
	int nCount = 0;

	CBCGPGridRow* pRow = m_LST_Grid_List.CreateRow(m_LST_Grid_List.GetColumnCount());
	pRow->GetItem(nCount++)->SetValue(_variant_t(strTime), FALSE);
	//序号
	pRow->GetItem(nCount++)->SetValue(_variant_t(tOutput.vecB[0].vecA[0].hvImageSN.I()), FALSE);

	//正面尺寸
	if (tOutput.vecB[0].tMeasure.hv_HasCoat == TRUE)
	{

		for (int i = 0; i < tOutput.vecB[0].tMeasure.tSize.hv_Width.Length(); i++)
		{
			if (tOutput.vecB[0].tMeasure.tSize.hv_Width[i] != NO_DATA)
			{
				strVal.Format("%0.4f", tOutput.vecB[0].tMeasure.tSize.hv_Width[i].D());
				pRow->GetItem(nCount)->SetValue(_variant_t((strVal)), FALSE);
				if (tOutput.tMerge.vecSideResult[0].hv_AllWidthFlagList[i].I() == NG)
					pRow->GetItem(nCount)->SetBackgroundColor(colTmp[NG], FALSE);
				nCount++;
			}
			else
			{
				pRow->GetItem(nCount++)->SetValue(_variant_t("-"), FALSE);
			}
		}
	}

	if (tPdInfo.nCoatingType == TwoSide)
	{
		//反面尺寸
		if (tOutput.vecB[1].tMeasure.hv_HasCoat == TRUE && theGlobal.m_Project.GetProjectConfig().nSideCount == 2)
		{
			for (int i = 0; i < tOutput.vecB[1].tMeasure.tSize.hv_Width.Length(); i++)
			{
				if (tOutput.vecB[1].tMeasure.tSize.hv_Width[i] != NO_DATA)
				{
					strVal.Format("%0.4f", tOutput.vecB[1].tMeasure.tSize.hv_Width[i].D());
					pRow->GetItem(nCount)->SetValue(_variant_t(strVal), FALSE);
					if (tOutput.tMerge.vecSideResult[1].hv_AllWidthFlagList[i].I() == NG)
						pRow->GetItem(nCount)->SetBackgroundColor(colTmp[NG], FALSE);
					nCount++;
				}
				else
				{
					pRow->GetItem(nCount++)->SetValue(_variant_t("-"), FALSE);
				}
			}
		}

		//A/B面错位
		if (tOutput.tMerge.hv_Misalignment.Length() > 0 && theGlobal.m_Project.GetProjectConfig().nSideCount == 2
			&& tOutput.vecB[0].tMeasure.hv_HasCoat == TRUE && tOutput.vecB[1].tMeasure.hv_HasCoat == TRUE)
		{
			for (int i = 0; i < tOutput.tMerge.hv_Misalignment.Length(); i++)
			{
				if (tOutput.tMerge.hv_Misalignment[i] != NO_DATA)
				{
					strVal.Format("%0.4f", tOutput.tMerge.hv_Misalignment[i].D());
					pRow->GetItem(nCount)->SetValue(_variant_t(strVal), FALSE);
					if (tOutput.tMerge.hv_MisalignmentFlagList[i].I() == NG)
						pRow->GetItem(nCount)->SetBackgroundColor(colTmp[NG], FALSE);
					nCount++;
				}
				else
				{
					pRow->GetItem(nCount++)->SetValue(_variant_t("-"), FALSE);
				}
			}
		}
	}

	m_LST_Grid_List.InsertRowBefore(0, pRow, FALSE);

	if (m_LST_Grid_List.GetRowCount() > MAX_LIST_COUNT)
		m_LST_Grid_List.RemoveRow(m_LST_Grid_List.GetRowCount() - 1, FALSE);


	m_LST_Grid_List.AdjustLayout();
}

BOOL CPageDispList4::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//return FALSE;
	return CBCGPDialog::OnEraseBkgnd(pDC);
}


void CPageDispList4::OnBnClickedButtonFai()
{
	// TODO: 在此添加控件通知处理程序代码
	theGlobal.m_Judge.m_tParam.bUseFirstArticleInsp = TRUE;

	while (m_LST_Grid_List.GetRowCount() > 0)
	{
		m_LST_Grid_List.RemoveRow(0, FALSE);
	}
	m_LST_Grid_List.AdjustLayout();
}


void CPageDispList4::OnEnChangeEditSpotinspectionTime()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CBCGPDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	UpdateData(TRUE);
	theGlobal.m_Judge.m_tParam.dFirstArticleInspMeter = m_EDIT_dFirstArticleMeter;
	theGlobal.m_Judge.WriteParam();
}
