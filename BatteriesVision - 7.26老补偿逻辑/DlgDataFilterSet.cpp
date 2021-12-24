// DlgDataFilterSet.cpp : 实现文件
//

#include "stdafx.h"
#include "BatteriesVision.h"
#include "DlgDataFilterSet.h"
#include "afxdialogex.h"


// CDlgDataFilterSet 对话框

IMPLEMENT_DYNAMIC(CDlgDataFilterSet, CBCGPDialog)

CDlgDataFilterSet::CDlgDataFilterSet(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(IDD_DIALOG_DATA_FILTER_SET, pParent)
{

}

CDlgDataFilterSet::~CDlgDataFilterSet()
{
}

void CDlgDataFilterSet::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgDataFilterSet, CBCGPDialog)
	ON_BN_CLICKED(IDOK, &CDlgDataFilterSet::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgDataFilterSet 消息处理程序


BOOL CDlgDataFilterSet::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	EnableVisualManagerStyle(TRUE, TRUE);
	InitUI();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void CDlgDataFilterSet::InitUI()
{
	CBCGPGridColors theme;
	CRect rcGridBase;
	GetDlgItem(IDC_STATIC_GRID)->GetWindowRect(rcGridBase);
	ScreenToClient(rcGridBase);
	m_Grid_Set.Create(WS_CHILD, rcGridBase, this, 1000);
	CBCGPVisualManager::GetInstance()->OnSetGridColorTheme(&m_Grid_Set, theme);
	m_Grid_Set.SendMessage(BCGM_ONSETCONTROLVMMODE, TRUE);
	m_Grid_Set.SetColorTheme(theme);
	m_Grid_Set.SetReadOnly(FALSE);//可编辑
	m_Grid_Set.EnableMarkSortedColumn(FALSE);//使用列排序
	m_Grid_Set.EnableHeader(TRUE, BCGP_GRID_HEADER_MOVE_ITEMS);//使用表头
	m_Grid_Set.EnableRowHeader(TRUE);//使用行表头
	m_Grid_Set.EnableLineNumbers(TRUE);//使用行序号
	m_Grid_Set.SetClearInplaceEditOnEnter(TRUE);
	m_Grid_Set.EnableInvertSelOnCtrl(FALSE);
	m_Grid_Set.InsertColumn(0, "项目", 120);
	m_Grid_Set.InsertColumn(1, "类型/0不使用;1均值;2中值;3中值平均;", 360);
	m_Grid_Set.InsertColumn(2, "偏差上限", 160);
	m_Grid_Set.InsertColumn(3, "数据搜集长度", 160);
	m_Grid_Set.LoadState(_T("BasicGrid"));
	m_Grid_Set.AdjustLayout();//相当于数据显示在表格上
	m_Grid_Set.ShowWindow(SW_NORMAL);

	RefreshList(theGlobal.m_DataFilter.GetFilterParam());
	((CButton*)GetDlgItem(IDC_CHECK_USE_FILTER))->SetCheck(theGlobal.m_DataFilter.IsUse());

	UpdateData(FALSE);
}
void CDlgDataFilterSet::RefreshList(CDataFilter::tagTFilterParam &tParam)
{
	HWND hwndList = m_Grid_Set.GetSafeHwnd();
	SNDMSG(hwndList, WM_SETREDRAW, FALSE, 0);//SetWindowRedraw

	m_Grid_Set.RemoveAll();
	for (int i = 0; i < tParam.nFilterNum; i++)
	{
		CBCGPGridRow* pRow = m_Grid_Set.CreateRow(m_Grid_Set.GetColumnCount());
		CString strName;
		strName = theGlobal.m_DataFilter.s_strFilterName[i];
		pRow->GetItem(0)->SetValue(_variant_t(strName), FALSE);
		HTuple hvTmp;
		if (i > tParam.hvFilterType.Length() - 1)
			hvTmp = CDataFilter::None;
		else
			hvTmp = tParam.hvFilterType[i];
		pRow->GetItem(1)->SetValue(_variant_t(hvTmp.I()), FALSE);
		if (i > tParam.hvOffset.Length() - 1)
			hvTmp = 100.0;
		else
			hvTmp = tParam.hvOffset[i];
		pRow->GetItem(2)->SetValue(_variant_t(hvTmp.D()), FALSE);
		if (i > tParam.hvMarkSize.Length() - 1)
			hvTmp = 5;
		else
			hvTmp = tParam.hvMarkSize[i];
		pRow->GetItem(3)->SetValue(_variant_t(hvTmp.I()), FALSE);
		if (i % 2 == 1)
		{
			pRow->GetItem(0)->SetBackgroundColor(RGB(58, 58, 58), FALSE);
			pRow->GetItem(1)->SetBackgroundColor(RGB(58, 58, 58), FALSE);
			pRow->GetItem(2)->SetBackgroundColor(RGB(58, 58, 58), FALSE);
			pRow->GetItem(3)->SetBackgroundColor(RGB(58, 58, 58), FALSE);
		}

		m_Grid_Set.InsertRowAfter(i, pRow, FALSE);
	}

	SNDMSG(hwndList, WM_SETREDRAW, TRUE, 0);
	m_Grid_Set.AdjustLayout();
}

void CDlgDataFilterSet::GetList()
{
	int nColumns = m_Grid_Set.GetColumnCount();
	int nRows = m_Grid_Set.GetRowCount();
	CDataFilter::tagTFilterParam tParam;
	tParam.nFilterNum = nRows;

	for (int i=0;i<nRows;i++)
	{
		CBCGPGridRow* pRow = m_Grid_Set.GetRow(i);
		tParam.hvFilterType.Append((int)pRow->GetItem(1)->GetValue());
		tParam.hvOffset.Append((double)pRow->GetItem(2)->GetValue());
		tParam.hvMarkSize.Append((int)pRow->GetItem(3)->GetValue());
	}
	theGlobal.m_DataFilter.SetFilterParam(tParam);
	theGlobal.m_DataFilter.WriteParam();
}

void CDlgDataFilterSet::OnBnClickedOk()
{
	GetList();
	theGlobal.m_DataFilter.SetUse(((CButton*)GetDlgItem(IDC_CHECK_USE_FILTER))->GetCheck());
	CBCGPDialog::OnOK();
}
