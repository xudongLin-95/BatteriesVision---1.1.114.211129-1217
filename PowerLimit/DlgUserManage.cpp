// DlgUserManage.cpp : 实现文件
//

#include "stdafx.h"
#include "PowerLimit.h"
#include "DlgUserManage.h"
#include "afxdialogex.h"
#include "DlgUser.h"
const CString strPowerLimit[] = {
	_T("无"),
	_T("操作员"),
	_T("技术员"),
	_T("工程师"),
	_T("管理员"),
	_T("超级管理员"),
};

// DlgUserManage 对话框

IMPLEMENT_DYNAMIC(DlgUserManage, CBCGPDialog)

DlgUserManage::DlgUserManage(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(DlgUserManage::IDD, pParent)
{
	m_pOwner = NULL;
	m_ListFont.CreatePointFont(80, "Microsoft YaHei");
}

DlgUserManage::~DlgUserManage()
{
	m_ListFont.DeleteObject();
}

void DlgUserManage::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(DlgUserManage, CBCGPDialog)
	ON_BN_CLICKED(IDC_BUTTON_ADD, &DlgUserManage::OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, &DlgUserManage::OnBnClickedButtonDelete)
	ON_BN_CLICKED(IDC_BUTTON_CHANGE, &DlgUserManage::OnBnClickedButtonChange)
	ON_BN_CLICKED(IDOK, &DlgUserManage::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &DlgUserManage::OnBnClickedCancel)
END_MESSAGE_MAP()

BOOL DlgUserManage::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();
	EnableVisualManagerStyle(TRUE, TRUE);
	// TODO:  在此添加额外的初始化
	initList();
	loadList();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void DlgUserManage::initList()
{
	CBCGPGridColors theme;
	CRect rcGridBase;
	GetDlgItem(IDC_STATIC_GRID_BASE)->GetWindowRect(rcGridBase);
	ScreenToClient(rcGridBase);
	m_listUser.Create(WS_CHILD, rcGridBase, this, 1000);
	CBCGPVisualManager::GetInstance()->OnSetGridColorTheme(&m_listUser, theme);
	m_listUser.SendMessage(BCGM_ONSETCONTROLVMMODE, TRUE);
	m_listUser.SetColorTheme(theme);
	m_listUser.SetReadOnly();//可编辑
	m_listUser.EnableMarkSortedColumn(FALSE);//使用列排序
	m_listUser.EnableHeader(TRUE, BCGP_GRID_HEADER_MOVE_ITEMS);//使用表头
	m_listUser.EnableRowHeader(TRUE);//使用行表头
	m_listUser.EnableLineNumbers(TRUE);//使用行序号
	m_listUser.SetWholeRowSel(TRUE);
	m_listUser.SetClearInplaceEditOnEnter(FALSE);
	m_listUser.EnableInvertSelOnCtrl(FALSE);
	m_listUser.InsertColumn(0, "账户权限", 100);
	m_listUser.InsertColumn(1, "工号", 150);	
	m_listUser.InsertColumn(2, "密码", 150);	
	m_listUser.InsertColumn(3, "卡号", 150); //BY YS
	m_listUser.LoadState(_T("BasicGrid"));
	m_listUser.AdjustLayout();//相当于数据显示在表格上
	m_listUser.ShowWindow(SW_NORMAL);
	m_listUser.SetFont(&m_ListFont);
}

void DlgUserManage::loadList()
{
	CBCGPGridRow* pRow;
	m_listUser.RemoveAll();
	int nPLCount = m_pOwner->getPowerLimitList().getCount();
	for (size_t i = 0; i < nPLCount; i++)
	{
		PowerLimit data = m_pOwner->getPowerLimitList().getPowerLimit(i);
		pRow = m_listUser.CreateRow(m_listUser.GetColumnCount());
		pRow->GetItem(0)->SetValue(_variant_t(strPowerLimit[data.getPowerType()]), FALSE);
		pRow->GetItem(1)->SetValue(_variant_t(data.getJobNumber()), FALSE);
		pRow->GetItem(2)->SetValue(_variant_t(data.getPassword()), FALSE);
		pRow->GetItem(3)->SetValue(_variant_t(data.getCardNumber()), FALSE);//卡号通过工号获取
		m_listUser.AddRow(pRow);
	}
	m_listUser.AdjustLayout();
}


// DlgUserManage 消息处理程序


void DlgUserManage::OnBnClickedButtonAdd()
{
	// TODO: 在此添加控件通知处理程序代码
	DlgUser dlg;
	dlg.m_pOwner = m_pOwner;
	dlg.m_eDlgType = DlgUser::DLGTYPE::ADD_TYPE;
	if ( IDOK == dlg.DoModal() )
	{
		int nPLCount = m_pOwner->getPowerLimitList().getCount();
		if (nPLCount == 0)
			return;

		loadList();
	}
}


void DlgUserManage::OnBnClickedButtonDelete()
{
	// TODO: 在此添加控件通知处理程序代码
	CList<CBCGPGridItem*, CBCGPGridItem*> sel;
	m_listUser.GetSelectedItems(sel);
	int nCount = sel.GetCount();
	if (0 >= nCount || nCount > 2)
	{
		BCGPMessageBox("请选择要删除的用户!");
		return;
	}

	CBCGPGridItem* pItem = sel.GetHead();
	CBCGPGridRow* pRow = pItem->GetParentRow();
	int nRow = pRow->GetRowId();

	PowerLimit data = m_pOwner->getPowerLimitList().getPowerLimit(nRow);
	m_pOwner->getPowerLimitList().delData(data);
	loadList();
}


void DlgUserManage::OnBnClickedButtonChange()
{
	// TODO: 在此添加控件通知处理程序代码
	CList<CBCGPGridItem*, CBCGPGridItem*> sel;
	m_listUser.GetSelectedItems(sel);
	int nCount = sel.GetCount();
	if (0 >= nCount || nCount > 2)
	{
		BCGPMessageBox("请选择要修改的用户!");
		return;
	}

	CBCGPGridItem* pItem = sel.GetHead();
	CBCGPGridRow* pRow = pItem->GetParentRow();
	int nRow = pRow->GetRowId();

	PowerLimit data = m_pOwner->getPowerLimitList().getPowerLimit(nRow);
	DlgUser dlg;
	dlg.m_pOwner = m_pOwner;
	dlg.m_eDlgType = DlgUser::DLGTYPE::CHANGE_TYPE;
	dlg.m_changePL = data;
	if ( IDOK == dlg.DoModal() )
	{
		loadList();
	}
}


void DlgUserManage::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	m_pOwner->Write();

	OnOK();
}


void DlgUserManage::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	m_pOwner->Read();

	OnCancel();
}
