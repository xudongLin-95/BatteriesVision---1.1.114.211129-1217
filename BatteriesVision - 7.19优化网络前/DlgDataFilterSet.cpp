// DlgDataFilterSet.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "BatteriesVision.h"
#include "DlgDataFilterSet.h"
#include "afxdialogex.h"


// CDlgDataFilterSet �Ի���

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


// CDlgDataFilterSet ��Ϣ�������


BOOL CDlgDataFilterSet::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	EnableVisualManagerStyle(TRUE, TRUE);
	InitUI();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
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
	m_Grid_Set.SetReadOnly(FALSE);//�ɱ༭
	m_Grid_Set.EnableMarkSortedColumn(FALSE);//ʹ��������
	m_Grid_Set.EnableHeader(TRUE, BCGP_GRID_HEADER_MOVE_ITEMS);//ʹ�ñ�ͷ
	m_Grid_Set.EnableRowHeader(TRUE);//ʹ���б�ͷ
	m_Grid_Set.EnableLineNumbers(TRUE);//ʹ�������
	m_Grid_Set.SetClearInplaceEditOnEnter(TRUE);
	m_Grid_Set.EnableInvertSelOnCtrl(FALSE);
	m_Grid_Set.InsertColumn(0, "��Ŀ", 120);
	m_Grid_Set.InsertColumn(1, "����/0��ʹ��;1��ֵ;2��ֵ;3��ֵƽ��;", 360);
	m_Grid_Set.InsertColumn(2, "ƫ������", 160);
	m_Grid_Set.InsertColumn(3, "�����Ѽ�����", 160);
	m_Grid_Set.LoadState(_T("BasicGrid"));
	m_Grid_Set.AdjustLayout();//�൱��������ʾ�ڱ����
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
