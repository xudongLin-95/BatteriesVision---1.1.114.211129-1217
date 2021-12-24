// DlgUserManage.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "PowerLimit.h"
#include "DlgUserManage.h"
#include "afxdialogex.h"
#include "DlgUser.h"
const CString strPowerLimit[] = {
	_T("��"),
	_T("����Ա"),
	_T("����Ա"),
	_T("����ʦ"),
	_T("����Ա"),
	_T("��������Ա"),
};

// DlgUserManage �Ի���

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
	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	initList();
	loadList();

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
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
	m_listUser.SetReadOnly();//�ɱ༭
	m_listUser.EnableMarkSortedColumn(FALSE);//ʹ��������
	m_listUser.EnableHeader(TRUE, BCGP_GRID_HEADER_MOVE_ITEMS);//ʹ�ñ�ͷ
	m_listUser.EnableRowHeader(TRUE);//ʹ���б�ͷ
	m_listUser.EnableLineNumbers(TRUE);//ʹ�������
	m_listUser.SetWholeRowSel(TRUE);
	m_listUser.SetClearInplaceEditOnEnter(FALSE);
	m_listUser.EnableInvertSelOnCtrl(FALSE);
	m_listUser.InsertColumn(0, "�˻�Ȩ��", 100);
	m_listUser.InsertColumn(1, "����", 150);	
	m_listUser.InsertColumn(2, "����", 150);	
	m_listUser.InsertColumn(3, "����", 150); //BY YS
	m_listUser.LoadState(_T("BasicGrid"));
	m_listUser.AdjustLayout();//�൱��������ʾ�ڱ����
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
		pRow->GetItem(3)->SetValue(_variant_t(data.getCardNumber()), FALSE);//����ͨ�����Ż�ȡ
		m_listUser.AddRow(pRow);
	}
	m_listUser.AdjustLayout();
}


// DlgUserManage ��Ϣ�������


void DlgUserManage::OnBnClickedButtonAdd()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CList<CBCGPGridItem*, CBCGPGridItem*> sel;
	m_listUser.GetSelectedItems(sel);
	int nCount = sel.GetCount();
	if (0 >= nCount || nCount > 2)
	{
		BCGPMessageBox("��ѡ��Ҫɾ�����û�!");
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CList<CBCGPGridItem*, CBCGPGridItem*> sel;
	m_listUser.GetSelectedItems(sel);
	int nCount = sel.GetCount();
	if (0 >= nCount || nCount > 2)
	{
		BCGPMessageBox("��ѡ��Ҫ�޸ĵ��û�!");
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_pOwner->Write();

	OnOK();
}


void DlgUserManage::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_pOwner->Read();

	OnCancel();
}
