// DlgDetectSet.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "SCISeparate.h"
#include "DlgDetectSet.h"
#include "afxdialogex.h"


// CDlgDetectSet �Ի���

IMPLEMENT_DYNAMIC(CDlgDetectSet, CDialogEx)

CDlgDetectSet::CDlgDetectSet(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgDetectSet::IDD, pParent)
{

	m_dCoatShrink = 0.0;
}

CDlgDetectSet::~CDlgDetectSet()
{
}

void CDlgDetectSet::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_COAT, m_LST_Coat);
	DDX_Text(pDX, IDC_EDIT_COAT_SHRINK, m_dCoatShrink);
	DDV_MinMaxDouble(pDX, m_dCoatShrink, 0, 999);
}


BEGIN_MESSAGE_MAP(CDlgDetectSet, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, &CDlgDetectSet::OnBnClickedButtonCancel)
	ON_BN_CLICKED(IDC_BUTTON_OK, &CDlgDetectSet::OnBnClickedButtonOk)
END_MESSAGE_MAP()


// CDlgDetectSet ��Ϣ�������
void CDlgDetectSet::InitUI()
{
	m_LST_Coat.SetColorHeader(TRUE);
	m_LST_Coat.SetHeaderBKColor(255, 255, 255, 1);
	//m_LST_Coat.SetHeaderFontHW(18, 0);

	m_LST_Coat.SetGridLines();
	m_LST_Coat.SetCheckboxeStyle();
	m_LST_Coat.SetEditable(TRUE);

	CString strVal;

	m_LST_Coat.DeleteAllItems();
	m_LST_Coat.DeleteAllColumns();
	m_LST_Coat.InsertColumn(m_LST_Coat.GetColumnCount(), "�����Ŀ", LVCFMT_CENTER, 100);
	m_LST_Coat.InsertColumn(m_LST_Coat.GetColumnCount(), "�Ҷ���ֵ", LVCFMT_CENTER, 100);
	m_LST_Coat.InsertColumn(m_LST_Coat.GetColumnCount(), "���(mm)", LVCFMT_CENTER, 100);
	m_LST_Coat.InsertColumn(m_LST_Coat.GetColumnCount(), "�߶�(mm)", LVCFMT_CENTER, 100);
	m_LST_Coat.InsertColumn(m_LST_Coat.GetColumnCount(), "���(mm2)", LVCFMT_CENTER, 100);
	m_LST_Coat.InsertItem(1,"Ϳ��©��");
	m_LST_Coat.SetCheck(0,m_pOwner->m_Surface.m_SurfaceParam.CoatParam.FoilLeak.bDetect);
	strVal.Format("%d",m_pOwner->m_Surface.m_SurfaceParam.CoatParam.FoilLeak.nGray);
	m_LST_Coat.SetItemText(0,1,strVal);
	strVal.Format("%f",m_pOwner->m_Surface.m_SurfaceParam.CoatParam.FoilLeak.dWidth);
	m_LST_Coat.SetItemText(0,2,strVal);
	strVal.Format("%f",m_pOwner->m_Surface.m_SurfaceParam.CoatParam.FoilLeak.dHeight);
	m_LST_Coat.SetItemText(0,3,strVal);
	strVal.Format("%f",m_pOwner->m_Surface.m_SurfaceParam.CoatParam.FoilLeak.dArea);
	m_LST_Coat.SetItemText(0,4,strVal);

	m_dCoatShrink = m_pOwner->m_Surface.m_SurfaceParam.CoatParam.dShrinkWidth;

	strVal.Format("%f",m_pOwner->m_Surface.m_SurfaceParam.Reslution.dColumnReslution);
	GetDlgItem(IDC_EDIT_RESLUTION)->SetWindowTextA(strVal);

	UpdateData(FALSE);
}

BOOL CDlgDetectSet::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	InitUI();
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


void CDlgDetectSet::OnBnClickedButtonCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	OnCancel();
}


void CDlgDetectSet::OnBnClickedButtonOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

}
