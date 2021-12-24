// DlgIPSet.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "BatteriesVision.h"
#include "DlgIPSet.h"
#include "afxdialogex.h"


// CDlgIPSet �Ի���

IMPLEMENT_DYNAMIC(CDlgIPSet, CBCGPDialog)

CDlgIPSet::CDlgIPSet(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(IDD_DIALOG_IP_SET, pParent)
{

}

CDlgIPSet::~CDlgIPSet()
{
}

void CDlgIPSet::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgIPSet, CBCGPDialog)
	ON_BN_CLICKED(IDOK, &CDlgIPSet::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgIPSet ��Ϣ�������


BOOL CDlgIPSet::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();
	EnableVisualManagerStyle(TRUE, TRUE);
	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	InitUI();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}

void CDlgIPSet::InitUI()
{
	CBCGPGridColors theme;
	CRect rcGridBase;
	GetDlgItem(IDC_STATIC_GRID)->GetWindowRect(rcGridBase);
	ScreenToClient(rcGridBase);

	m_Grid_Base.Create(WS_CHILD, rcGridBase, this, 1000);
	CBCGPVisualManager::GetInstance()->OnSetGridColorTheme(&m_Grid_Base, theme);
	m_Grid_Base.SendMessage(BCGM_ONSETCONTROLVMMODE, TRUE);
	m_Grid_Base.SetColorTheme(theme);
	m_Grid_Base.SetReadOnly(FALSE);//���ɱ༭
	m_Grid_Base.SetWholeRowSel(FALSE);
	m_Grid_Base.EnableMarkSortedColumn(FALSE);//ʹ��������
	m_Grid_Base.EnableHeader(TRUE, BCGP_GRID_HEADER_MOVE_ITEMS);//ʹ�ñ�ͷ
	m_Grid_Base.EnableCheckBoxes(FALSE);
	m_Grid_Base.EnableRowHeader(TRUE);//ʹ���б�ͷ
	m_Grid_Base.EnableLineNumbers(TRUE);//ʹ�������
	m_Grid_Base.SetClearInplaceEditOnEnter(FALSE);
	m_Grid_Base.EnableInvertSelOnCtrl(FALSE);
	m_Grid_Base.EnableDragHeaderItems(FALSE);
	m_Grid_Base.LoadState(_T("BasicGrid"));
	m_Grid_Base.AdjustLayout();//�൱��������ʾ�ڱ����
	m_Grid_Base.ShowWindow(SW_NORMAL);

	//IP�Ͷ˿�����
	CBCGPGridRow* pRow;
	m_Grid_Base.RemoveAll();
	m_Grid_Base.DeleteAllColumns();

	if (theGlobal.m_tSystemSet.eSN == CGlobal::SN_Head)
	{
		m_Grid_Base.InsertColumn(0, "�ͻ�������", 130);
		m_Grid_Base.InsertColumn(1, "�����˿�", 80);
		m_Grid_Base.InsertColumn(2, "�Ƿ�����", 80);
		pRow = m_Grid_Base.CreateRow(m_Grid_Base.GetColumnCount());
		pRow->GetItem(0)->SetValue(_variant_t("��λ2"), FALSE);
		pRow->GetItem(1)->SetValue(_variant_t(theGlobal.m_Upload.m_tParam.nPortListen[0]), FALSE);
		pRow->GetItem(2)->AddOption("�ر�");
		pRow->GetItem(2)->AddOption("����");
		pRow->GetItem(2)->SetValue(_variant_t(theGlobal.m_Upload.m_tParam.bListenClient[0] ? "����" : "�ر�"), FALSE);
		m_Grid_Base.AddRow(pRow, FALSE);
		pRow = m_Grid_Base.CreateRow(m_Grid_Base.GetColumnCount());
		pRow->GetItem(0)->SetValue(_variant_t("��λ3"), FALSE);
		pRow->GetItem(1)->SetValue(_variant_t(theGlobal.m_Upload.m_tParam.nPortListen[1]), FALSE);
		pRow->GetItem(2)->AddOption("�ر�");
		pRow->GetItem(2)->AddOption("����");
		pRow->GetItem(2)->SetValue(_variant_t(theGlobal.m_Upload.m_tParam.bListenClient[1] ? "����" : "�ر�"), FALSE);
		m_Grid_Base.AddRow(pRow, FALSE);
	}
	else
	{
		m_Grid_Base.InsertColumn(0, "����������", 130);
		m_Grid_Base.InsertColumn(1, "������IP", 160);
		m_Grid_Base.InsertColumn(2, "�������˿�", 80);
		pRow = m_Grid_Base.CreateRow(m_Grid_Base.GetColumnCount());
		pRow->GetItem(0)->SetValue(_variant_t("��λ1"), FALSE);
		pRow->GetItem(1)->SetValue(_variant_t(theGlobal.m_Upload.m_tParam.strAddresConnect), FALSE);
		pRow->GetItem(2)->SetValue(_variant_t(theGlobal.m_Upload.m_tParam.nPortConnect), FALSE);
		m_Grid_Base.AddRow(pRow, FALSE);
	}
	m_Grid_Base.AdjustLayout();
}


void CDlgIPSet::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CBCGPGridRow* pRow;
	if (theGlobal.m_tSystemSet.eSN == CGlobal::SN_Head)
	{
		//��λ2
		pRow = m_Grid_Base.GetRow(0);
		theGlobal.m_Upload.m_tParam.nPortListen[0] = (_variant_t)pRow->GetItem(1)->GetValue();
		theGlobal.m_Upload.m_tParam.bListenClient[0] = ((CString)pRow->GetItem(2)->GetValue()) == "����" ? TRUE : FALSE;
		//��λ3
		pRow = m_Grid_Base.GetRow(1);
		theGlobal.m_Upload.m_tParam.nPortListen[1] = (_variant_t)pRow->GetItem(1)->GetValue();
		theGlobal.m_Upload.m_tParam.bListenClient[1] = ((CString)pRow->GetItem(2)->GetValue()) == "����" ? TRUE : FALSE;
	}
	else
	{
		//��λ1
		pRow = m_Grid_Base.GetRow(0);
		theGlobal.m_Upload.m_tParam.strAddresConnect = (_variant_t)pRow->GetItem(1)->GetValue();
		theGlobal.m_Upload.m_tParam.nPortConnect = (_variant_t)pRow->GetItem(2)->GetValue();
	}

	theGlobal.m_Upload.WriteParam();

	OnOK();
}
