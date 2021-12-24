// DlgUserPower.cpp : 实现文件
//

#include "stdafx.h"
#include "PowerLimit.h"
#include "DlgUserPower.h"


// DlgUserPower 对话框

IMPLEMENT_DYNAMIC(DlgUserPower, CBCGPDialog)

DlgUserPower::DlgUserPower(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(IDD_DIALOG_USERPOWER, pParent)
{
	m_pOwner = NULL;
}

DlgUserPower::~DlgUserPower()
{
}

void DlgUserPower::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
}

BOOL DlgUserPower::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();
	EnableVisualManagerStyle(TRUE, TRUE);
	InitList();
	LoadList();
	return TRUE;
}


BEGIN_MESSAGE_MAP(DlgUserPower, CBCGPDialog)
	ON_BN_CLICKED(IDOK, &DlgUserPower::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &DlgUserPower::OnBnClickedCancel)
END_MESSAGE_MAP()


// DlgUserPower 消息处理程序


void DlgUserPower::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	//int nCount = m_LST_Grid_List.GetColumnCount();
	CBCGPGridRow* pRow;
	CString strValue;
	for (size_t i = 0; i < PM_COUNT; i++)
	{
		pRow = m_LST_Grid_List.GetRow(i);
		if (!pRow) continue;
		for (int nPT = 0; nPT < m_pOwner->GetPowerTypeList().size() - 1; nPT++)
		{
			BOOL bEnable(FALSE);
			strValue = pRow->GetItem(nPT + 1)->GetValue();
			if (strValue == "是")
				bEnable = TRUE;

			m_pOwner->GetPowerTypeList().at(nPT).SetModulePower((ePOWERLIMITMODULE)i, bEnable);
		}
	}
	m_pOwner->Write();

	OnOK();
}


void DlgUserPower::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码

	OnCancel();
}

void DlgUserPower::InitList()
{
	CBCGPGridColors theme;
	CRect rcGridBase;
	GetDlgItem(IDC_STATIC_GRID)->GetWindowRect(rcGridBase);
	ScreenToClient(rcGridBase);
	m_LST_Grid_List.Create(WS_CHILD, rcGridBase, this, 1000);
	CBCGPVisualManager::GetInstance()->OnSetGridColorTheme(&m_LST_Grid_List, theme);
	m_LST_Grid_List.SendMessage(BCGM_ONSETCONTROLVMMODE, TRUE);
	m_LST_Grid_List.SetColorTheme(theme);
	m_LST_Grid_List.SetReadOnly(FALSE);//可编辑
	m_LST_Grid_List.SetWholeRowSel(FALSE);
	m_LST_Grid_List.EnableMarkSortedColumn(FALSE);//使用列排序
	m_LST_Grid_List.EnableHeader(TRUE, BCGP_GRID_HEADER_MOVE_ITEMS);//使用表头
	m_LST_Grid_List.EnableCheckBoxes(FALSE);
	m_LST_Grid_List.EnableRowHeader(TRUE);//使用行表头
	m_LST_Grid_List.EnableLineNumbers(TRUE);//使用行序号
	m_LST_Grid_List.SetClearInplaceEditOnEnter(FALSE);
	m_LST_Grid_List.EnableInvertSelOnCtrl(FALSE);
	m_LST_Grid_List.EnableDragHeaderItems(FALSE);
	m_LST_Grid_List.SetScalingRange(0.1, 2.0);//设置缩放比例，按Ctrl键和鼠标滚轮可缩放

	m_LST_Grid_List.DeleteAllColumns();
	int nCount = 0;
	m_LST_Grid_List.InsertColumn(nCount++, "权限类型", 100);
	m_LST_Grid_List.InsertColumn(nCount++, "操作员", 70);
	m_LST_Grid_List.InsertColumn(nCount++, "技术员", 70);
	m_LST_Grid_List.InsertColumn(nCount++, "工程师", 70);
	m_LST_Grid_List.InsertColumn(nCount++, "管理员", 70);
	m_LST_Grid_List.InsertColumn(nCount++, "超级管理员", 70);
	m_LST_Grid_List.LoadState(_T("BasicGrid"));
	m_LST_Grid_List.AdjustLayout();//相当于数据显示在表格上
	m_LST_Grid_List.ShowWindow(SW_NORMAL);
}

void DlgUserPower::LoadList()
{
	m_LST_Grid_List.RemoveAll();

	CBCGPGridRow* pRow;
	for (int i = 0; i<PM_COUNT; i++)
	{
		pRow = m_LST_Grid_List.CreateRow(m_LST_Grid_List.GetColumnCount());
		pRow->GetItem(0)->SetValue(_variant_t(g_strPowerModuleName[i]), FALSE);
		for (int nPT = 0; nPT < m_pOwner->GetPowerTypeList().size(); nPT++)
		{
			pRow->GetItem(1 + nPT)->AddOption("是");
			pRow->GetItem(1 + nPT)->AddOption("否");
			pRow->GetItem(1 + nPT)->SetValue(_variant_t(m_pOwner->GetPowerTypeList().at(nPT).GetModulePower((ePOWERLIMITMODULE)i) ? "是" : "否"), FALSE);
		}
		m_LST_Grid_List.AddRow(pRow, FALSE);
	}
	m_LST_Grid_List.AdjustLayout();
}
