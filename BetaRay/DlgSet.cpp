// DlgTemperature.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Temperature.h"
#include "afxdialogex.h"
#include "DlgSet.h"
#include "BetaRay.in.h"

// CDlgSet �Ի���

IMPLEMENT_DYNAMIC(CDlgSet, CBCGPDialog)

CDlgSet::CDlgSet(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(IDD_DIALOG_TEMPERATURE, pParent)
	, m_EDIT_nPort(0)
	, m_EDIT_strIP(_T(""))
	, m_EDIT_dWeightStd(0)
	, m_EDIT_dWeightTol(0)
{

}

CDlgSet::~CDlgSet()
{
}

void CDlgSet::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_BETARAY_TYPE, m_Combo_BetaRayType);
	DDX_Text(pDX, IDC_EDIT_PORT, m_EDIT_nPort);
	DDX_Text(pDX, IDC_EDIT_IP, m_EDIT_strIP);
	DDX_Text(pDX, IDC_EDIT_WEIDHT_STD, m_EDIT_dWeightStd);
	DDX_Text(pDX, IDC_EDIT_WEIGHT_TOL, m_EDIT_dWeightTol);
}


BEGIN_MESSAGE_MAP(CDlgSet, CBCGPDialog)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_CONNECT, &CDlgSet::OnBnClickedButtonConnect)
	ON_BN_CLICKED(IDC_BUTTON_EXIT, &CDlgSet::OnBnClickedButtonExit)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_EXIT, &CDlgSet::OnBnClickedButtonSaveExit)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CDlgSet ��Ϣ�������

BOOL CDlgSet::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	EnableVisualManagerStyle(TRUE, TRUE);

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	InitUI();
	SetTimer(NULL, 1000, NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}

void CDlgSet::InitUI()
{
	((CButton*)GetDlgItem(IDC_CHECK_USE_COMM))->SetCheck((bool)g_This->m_tParam.bUseComm);

	m_Combo_BetaRayType.AddString("������");
	m_Combo_BetaRayType.AddString("���");
	m_Combo_BetaRayType.SetCurSel(g_This->m_tParam.eType);

	m_EDIT_strIP = g_This->m_tParam.strIP;
	m_EDIT_nPort = g_This->m_tParam.nPort;

	tagBetaRayPdInfo tBetaRayInfo = g_This->m_tBetaRayPdInfo;
	if (g_This->m_sInit.tPdInfo.nCoatingType == TwoSide)
	{
		m_EDIT_dWeightStd = g_This->m_tBetaRayPdInfo.fBaseWeight + g_This->m_tBetaRayPdInfo.fNetWeightA + g_This->m_tBetaRayPdInfo.fNetWeightB;
		m_EDIT_dWeightTol = g_This->m_tBetaRayPdInfo.fUpperTol;
	}
	else
	{
		m_EDIT_dWeightStd = g_This->m_tBetaRayPdInfo.fBaseWeight + g_This->m_tBetaRayPdInfo.fNetWeightA;
		m_EDIT_dWeightTol = g_This->m_tBetaRayPdInfo.fUpperTol;
	}

	BOOL bRet = 0;
	m_pOwner->DoSomething("GetConnection", NULL, &bRet);
	if (bRet)
	{
		((CBCGPButton*)GetDlgItem(IDC_BUTTON_CONNECT))->SetWindowTextA("Connected");
		((CBCGPButton*)GetDlgItem(IDC_BUTTON_CONNECT))->SetFaceColor(RGB(0, 255, 0));
	}
	else
	{
		((CBCGPButton*)GetDlgItem(IDC_BUTTON_CONNECT))->SetWindowTextA("Unconnected");
		((CBCGPButton*)GetDlgItem(IDC_BUTTON_CONNECT))->SetFaceColor(RGB(255, 0, 0));
	}

	CRect rcGridBase;
	CBCGPGridColors theme;
	GetDlgItem(IDC_STATIC_GRID)->GetWindowRect(rcGridBase);
	ScreenToClient(rcGridBase);
	m_Grid_Data.Create(WS_CHILD, rcGridBase, this, 1000);
	CBCGPVisualManager::GetInstance()->OnSetGridColorTheme(&m_Grid_Data, theme);
	m_Grid_Data.SendMessage(BCGM_ONSETCONTROLVMMODE, TRUE);
	m_Grid_Data.SetColorTheme(theme);
	m_Grid_Data.SetReadOnly(TRUE);//���ɱ༭
	m_Grid_Data.SetWholeRowSel(TRUE);
	m_Grid_Data.EnableMarkSortedColumn(FALSE);//ʹ��������
	m_Grid_Data.EnableHeader(TRUE, BCGP_GRID_HEADER_MOVE_ITEMS);//ʹ�ñ�ͷ
	m_Grid_Data.EnableRowHeader(TRUE);//ʹ���б�ͷ
	m_Grid_Data.EnableLineNumbers(TRUE);//ʹ�������
	m_Grid_Data.SetClearInplaceEditOnEnter(FALSE);
	m_Grid_Data.EnableInvertSelOnCtrl(FALSE);
	m_Grid_Data.InsertColumn(0, "����/����", 100);
	m_Grid_Data.LoadState(_T("BasicGrid"));
	m_Grid_Data.AdjustLayout();//�൱��������ʾ�ڱ����
	m_Grid_Data.ShowWindow(SW_NORMAL);

	UpdateData(FALSE);//�������ؼ�
}

void CDlgSet::OnBnClickedButtonConnect()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	BOOL bRet = FALSE;
	{
		m_pOwner->Close();
		m_pOwner->Open();
		m_pOwner->DoSomething("GetConnection", NULL, &bRet);
		if (bRet)
		{
			((CBCGPButton*)GetDlgItem(IDC_BUTTON_CONNECT))->SetWindowTextA("Connected");
			((CBCGPButton*)GetDlgItem(IDC_BUTTON_CONNECT))->SetFaceColor(RGB(0, 255, 0));
		}
		else
		{
			((CBCGPButton*)GetDlgItem(IDC_BUTTON_CONNECT))->SetWindowTextA("Unconnected");
			((CBCGPButton*)GetDlgItem(IDC_BUTTON_CONNECT))->SetFaceColor(RGB(255, 0, 0));
		}
	}
}

void CDlgSet::OnBnClickedButtonExit()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CBCGPDialog::OnCancel();
}

void CDlgSet::OnBnClickedButtonSaveExit()
{
	UpdateData(TRUE);//�ؼ�������

	m_pOwner->m_tParam.bUseComm = ((CButton*)GetDlgItem(IDC_CHECK_USE_COMM))->GetCheck();

	m_pOwner->m_tParam.eType = (eBetaRayType)m_Combo_BetaRayType.GetCurSel();

	m_pOwner->m_tParam.strIP = m_EDIT_strIP;
	m_pOwner->m_tParam.nPort = m_EDIT_nPort;
	m_pOwner->SaveParam();

	CBCGPDialog::OnOK();
}

void CDlgSet::RefreshData()
{
	//��������
	if (g_This->m_hvecBetaRayDatas.Length() == 0 ||
		g_This->m_hvecBetaRayDatasX.Length() == 0)
	{
		return;
	}

	static HTupleVector s_hvecDataX(1), s_hvecData(1);

	if (g_This->m_hvecBetaRayDatas[0].T().Length() == s_hvecData.Length())
	{
		//�����������ݲ�0.001���ϣ�����Ϊ���µ�һ������
		HTuple hvSub, hvGreater, hvInc;
		TupleSub(g_This->m_hvecBetaRayDatas[0].T(), s_hvecData[0].T(), &hvSub);
		TupleGreaterElem(hvSub.TupleAbs(), 0.001, &hvGreater);
		TupleFind(hvGreater, 1, &hvInc);
		if (hvInc == -1 || hvInc.Length() == 0)
		{
			return;
		}
	}

	s_hvecDataX = g_This->m_hvecBetaRayDatasX;
	s_hvecData = g_This->m_hvecBetaRayDatas;

	m_Grid_Data.RemoveAll();
	m_Grid_Data.DeleteAllColumns();

	m_Grid_Data.InsertColumn(0, "����/����", 100);
	int nCount = 1;
	for (int i=0;i<s_hvecDataX[0].T().Length();i++)
	{
		if(i % 10 != 0) continue;
		CString strName;
		strName.Format("%d", s_hvecDataX[0].T()[i].I());
		m_Grid_Data.InsertColumn(nCount++, strName, 50);
	}

	CBCGPGridRow* pRow = m_Grid_Data.CreateRow(m_Grid_Data.GetColumnCount());

	nCount = 1;
	for (int i = 0; i < s_hvecData[0].T().Length(); i++)
	{
		if (i % 10 != 0) continue;
		CString strName;
		strName.Format("%0.2f", s_hvecData[0].T()[i].D());
		pRow->GetItem(nCount++)->SetValue(_variant_t(strName), FALSE);
	}
	m_Grid_Data.AddRow(pRow, FALSE);
	m_Grid_Data.AdjustLayout();
}

void CDlgSet::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (!this->IsWindowVisible()) return;
	RefreshData();
	CBCGPDialog::OnTimer(nIDEvent);
}
