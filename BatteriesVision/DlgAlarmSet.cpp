// DlgAlarmSet.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "BatteriesVision.h"
#include "DlgAlarmSet.h"
#include "afxdialogex.h"


// CDlgAlarmSet �Ի���

IMPLEMENT_DYNAMIC(CDlgAlarmSet, CBCGPDialog)

CDlgAlarmSet::CDlgAlarmSet(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(IDD_DIALOG_ALARM_SET, pParent)
	, m_EDIT_dIngoreMeter(0)
	, m_EDIT_dMeanShiftFactor(0)
{

}

CDlgAlarmSet::~CDlgAlarmSet()
{
}

void CDlgAlarmSet::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_INGORE_METER, m_EDIT_dIngoreMeter);
	DDX_Text(pDX, IDC_EDIT_MEANSHIFT_FACTOR, m_EDIT_dMeanShiftFactor);
}


BEGIN_MESSAGE_MAP(CDlgAlarmSet, CBCGPDialog)
	ON_BN_CLICKED(IDOK, &CDlgAlarmSet::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgAlarmSet ��Ϣ�������


BOOL CDlgAlarmSet::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���

	return CBCGPDialog::PreTranslateMessage(pMsg);
}


BOOL CDlgAlarmSet::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	EnableVisualManagerStyle(TRUE, TRUE);

	InitUI();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}

void CDlgAlarmSet::InitUI()
{
	int nCount = 0;
	CBCGPGridColors theme;
	CRect rcGridBase;
	GetDlgItem(IDC_STATIC_GRID)->GetWindowRect(rcGridBase);
	ScreenToClient(rcGridBase);
	m_Grid_SizeAlarm.Create(WS_CHILD, rcGridBase, this, 1000);
	CBCGPVisualManager::GetInstance()->OnSetGridColorTheme(&m_Grid_SizeAlarm, theme);
	m_Grid_SizeAlarm.SendMessage(BCGM_ONSETCONTROLVMMODE, TRUE);
	m_Grid_SizeAlarm.SetColorTheme(theme);
	m_Grid_SizeAlarm.SetReadOnly(FALSE);//�ɱ༭
	m_Grid_SizeAlarm.EnableMarkSortedColumn(FALSE);//ʹ��������
	m_Grid_SizeAlarm.EnableHeader(TRUE, BCGP_GRID_HEADER_MOVE_ITEMS);//ʹ�ñ�ͷ
	m_Grid_SizeAlarm.EnableRowHeader(TRUE);//ʹ���б�ͷ
	m_Grid_SizeAlarm.EnableLineNumbers(TRUE);//ʹ�������
	m_Grid_SizeAlarm.SetClearInplaceEditOnEnter(FALSE);
	m_Grid_SizeAlarm.EnableInvertSelOnCtrl(FALSE);
	nCount = 0;
	m_Grid_SizeAlarm.InsertColumn(nCount++, "��������", 120);
	m_Grid_SizeAlarm.InsertColumn(nCount++, "�����ж�", 80);
	m_Grid_SizeAlarm.InsertColumn(nCount++, "NG����(����)", 100);
	m_Grid_SizeAlarm.InsertColumn(nCount++, "", 40);
	m_Grid_SizeAlarm.InsertColumn(nCount++, "ͣ���ж�", 80);
	m_Grid_SizeAlarm.InsertColumn(nCount++, "NG����(ͣ��)", 100);
	m_Grid_SizeAlarm.LoadState(_T("BasicGrid"));
	m_Grid_SizeAlarm.AdjustLayout();//�൱��������ʾ�ڱ����
	m_Grid_SizeAlarm.ShowWindow(SW_NORMAL);

	RefreshSizeList();

	GetDlgItem(IDC_STATIC_GRID2)->GetWindowRect(rcGridBase);
	ScreenToClient(rcGridBase);
	m_Grid_DefectAlarm.Create(WS_CHILD, rcGridBase, this, 1001);
	CBCGPVisualManager::GetInstance()->OnSetGridColorTheme(&m_Grid_DefectAlarm, theme);
	m_Grid_DefectAlarm.SendMessage(BCGM_ONSETCONTROLVMMODE, TRUE);
	m_Grid_DefectAlarm.SetColorTheme(theme);
	m_Grid_DefectAlarm.SetReadOnly(FALSE);//�ɱ༭
	m_Grid_DefectAlarm.EnableMarkSortedColumn(FALSE);//ʹ��������
	m_Grid_DefectAlarm.EnableHeader(TRUE, BCGP_GRID_HEADER_MOVE_ITEMS);//ʹ�ñ�ͷ
	m_Grid_DefectAlarm.EnableRowHeader(TRUE);//ʹ���б�ͷ
	m_Grid_DefectAlarm.EnableLineNumbers(TRUE);//ʹ�������
	m_Grid_DefectAlarm.SetClearInplaceEditOnEnter(FALSE);
	m_Grid_DefectAlarm.EnableInvertSelOnCtrl(FALSE);
	nCount = 0;
	m_Grid_DefectAlarm.InsertColumn(nCount++, "覴�����", 120);
	m_Grid_DefectAlarm.InsertColumn(nCount++, "�����ж�", 90);
	m_Grid_DefectAlarm.InsertColumn(nCount++, "NG���(����)", 90);
	m_Grid_DefectAlarm.InsertColumn(nCount++, "NG����(����)", 90);
	m_Grid_DefectAlarm.InsertColumn(nCount++, "NG����(����)", 90);
	m_Grid_DefectAlarm.InsertColumn(nCount++, "", 40);
	m_Grid_DefectAlarm.InsertColumn(nCount++, "ͣ���ж�", 90);
	m_Grid_DefectAlarm.InsertColumn(nCount++, "NG���(ͣ��)", 90);
	m_Grid_DefectAlarm.InsertColumn(nCount++, "NG����(ͣ��)", 90);
	m_Grid_DefectAlarm.InsertColumn(nCount++, "NG����(ͣ��)", 90);
	m_Grid_DefectAlarm.LoadState(_T("BasicGrid"));
	m_Grid_DefectAlarm.AdjustLayout();//�൱��������ʾ�ڱ����
	m_Grid_DefectAlarm.ShowWindow(SW_NORMAL);

	RefreshDefectList();

	((CButton*)GetDlgItem(IDC_CHECK_USE_ALARM))->SetCheck(theGlobal.m_Judge.m_tParam.bUseAlarm);
	((CButton*)GetDlgItem(IDC_CHECK_USE_STOP_MACHINE))->SetCheck(theGlobal.m_Judge.m_tParam.bUseStopMachine);
	((CButton*)GetDlgItem(IDC_CHECK_USE_NG_MACHINE))->SetCheck(theGlobal.m_Judge.m_tParam.bUseNGAlarm);
	m_EDIT_dIngoreMeter = theGlobal.m_Judge.m_tParam.dIngoreMeter;
	m_EDIT_dMeanShiftFactor = theGlobal.m_Judge.m_tParam.dMeanShiftLimitFactor;

	UpdateData(FALSE);
}

void CDlgAlarmSet::RefreshSizeList()
{
	CBCGPGridRow* pRow;
	m_Grid_SizeAlarm.RemoveAll();
	for (int i = 0; i < NG_ABMisalign+1; i++)
	{
		pRow = m_Grid_SizeAlarm.CreateRow(m_Grid_SizeAlarm.GetColumnCount());
		CString strName = theGlobal.m_Counter.GetNGTypeName(NG_CoatWidth + i);

		pRow->GetItem(0)->SetValue(_variant_t(strName), FALSE);
		pRow->GetItem(1)->AddOption("�ر�");
		pRow->GetItem(1)->AddOption("����");
		pRow->GetItem(1)->SetValue(_variant_t(theGlobal.m_Judge.m_tParam.vecAlarm[i].bUse ? "����" : "�ر�"), FALSE);
		pRow->GetItem(2)->SetValue(_variant_t(theGlobal.m_Judge.m_tParam.vecAlarm[i].hvMeters.D()), FALSE);

		pRow->GetItem(4)->AddOption("�ر�");
		pRow->GetItem(4)->AddOption("����");
		pRow->GetItem(4)->SetValue(_variant_t(theGlobal.m_Judge.m_tParam.vecStopMachine[i].bUse ? "����" : "�ر�"),FALSE);
		pRow->GetItem(5)->SetValue(_variant_t(theGlobal.m_Judge.m_tParam.vecStopMachine[i].hvMeters.D()), FALSE);
		m_Grid_SizeAlarm.AddRow(pRow, FALSE);
	}
	//AB����Ļ�У׼
	pRow = m_Grid_SizeAlarm.CreateRow(m_Grid_SizeAlarm.GetColumnCount());
	CString strName = "AB����Ŀ�У";
	pRow->GetItem(0)->SetValue(_variant_t(strName), FALSE);
	pRow->GetItem(1)->AddOption("�ر�");
	pRow->GetItem(1)->AddOption("����");
	pRow->GetItem(1)->SetValue(_variant_t(theGlobal.m_Judge.m_tParam.vecAlarm[NG_Count + 0].bUse ? "����" : "�ر�"), FALSE);
	pRow->GetItem(2)->SetValue(_variant_t(theGlobal.m_Judge.m_tParam.vecAlarm[NG_Count + 0].hvMeters.D()), FALSE);
	pRow->GetItem(4)->AddOption("�ر�");
	pRow->GetItem(4)->AddOption("����");
	pRow->GetItem(4)->SetValue(_variant_t(theGlobal.m_Judge.m_tParam.vecStopMachine[NG_Count + 0].bUse ? "����" : "�ر�"), FALSE);
	pRow->GetItem(5)->SetValue(_variant_t(theGlobal.m_Judge.m_tParam.vecStopMachine[NG_Count + 0].hvMeters.D()), FALSE);
	m_Grid_SizeAlarm.AddRow(pRow, FALSE);

	//Mean Shift
	for (int i = 0; i < NG_ABMisalign + 1; i++)
	{
		pRow = m_Grid_SizeAlarm.CreateRow(m_Grid_SizeAlarm.GetColumnCount());
		CString strName = "Mean Shift " + theGlobal.m_Counter.GetNGTypeName(NG_CoatWidth + i);

		pRow->GetItem(0)->SetValue(_variant_t(strName), FALSE);
		pRow->GetItem(1)->AddOption("�ر�");
		pRow->GetItem(1)->AddOption("����");
		pRow->GetItem(1)->SetValue(_variant_t(theGlobal.m_Judge.m_tParam.vecMeanShiftAlarmParam[i].bUse ? "����" : "�ر�"), FALSE);
		pRow->GetItem(2)->SetValue(_variant_t(theGlobal.m_Judge.m_tParam.vecMeanShiftAlarmParam[i].hvMeters.D()), FALSE);

		pRow->GetItem(4)->AddOption("�ر�");
		pRow->GetItem(4)->AddOption("����");
		pRow->GetItem(4)->SetValue(_variant_t(theGlobal.m_Judge.m_tParam.vecMeanShiftStopMachineParam[i].bUse ? "����" : "�ر�"), FALSE);
		pRow->GetItem(5)->SetValue(_variant_t(theGlobal.m_Judge.m_tParam.vecMeanShiftStopMachineParam[i].hvMeters.D()), FALSE);
		m_Grid_SizeAlarm.AddRow(pRow, FALSE);
	}


	m_Grid_SizeAlarm.AdjustLayout();
}

void CDlgAlarmSet::RefreshDefectList()
{
	CBCGPGridRow* pRow;
	m_Grid_DefectAlarm.RemoveAll();
	for (int i = NG_ABMisalign + 1; i < NG_Count; i++)
	{
		pRow = m_Grid_DefectAlarm.CreateRow(m_Grid_DefectAlarm.GetColumnCount());
		CString strName = theGlobal.m_Counter.GetNGTypeName(NG_CoatWidth + i);

		pRow->GetItem(0)->SetValue(_variant_t(strName), FALSE);
		pRow->GetItem(1)->AddOption("�ر�");
		pRow->GetItem(1)->AddOption("����");
		pRow->GetItem(1)->SetValue(_variant_t(theGlobal.m_Judge.m_tParam.vecAlarm[i].bUse ? "����" : "�ر�"), FALSE);
		pRow->GetItem(2)->SetValue(_variant_t(theGlobal.m_Judge.m_tParam.vecAlarm[i].hvArea.D()), FALSE);
		pRow->GetItem(3)->SetValue(_variant_t(theGlobal.m_Judge.m_tParam.vecAlarm[i].hvNum.I()), FALSE);
		pRow->GetItem(4)->SetValue(_variant_t(theGlobal.m_Judge.m_tParam.vecAlarm[i].hvMeters.D()), FALSE);

		pRow->GetItem(6)->AddOption("�ر�");
		pRow->GetItem(6)->AddOption("����");
		pRow->GetItem(6)->SetValue(_variant_t(theGlobal.m_Judge.m_tParam.vecStopMachine[i].bUse ? "����" : "�ر�"), FALSE);
		pRow->GetItem(7)->SetValue(_variant_t(theGlobal.m_Judge.m_tParam.vecStopMachine[i].hvArea.D()), FALSE);
		pRow->GetItem(8)->SetValue(_variant_t(theGlobal.m_Judge.m_tParam.vecStopMachine[i].hvNum.I()), FALSE);
		pRow->GetItem(9)->SetValue(_variant_t(theGlobal.m_Judge.m_tParam.vecStopMachine[i].hvMeters.D()), FALSE);
		m_Grid_DefectAlarm.AddRow(pRow, FALSE);
	}

	m_Grid_DefectAlarm.AdjustLayout();
}

void CDlgAlarmSet::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	CBCGPGridRow* pRow;
	theGlobal.m_Judge.m_tParam.bUseAlarm = ((CButton*)GetDlgItem(IDC_CHECK_USE_ALARM))->GetCheck();
	theGlobal.m_Judge.m_tParam.bUseStopMachine = ((CButton*)GetDlgItem(IDC_CHECK_USE_STOP_MACHINE))->GetCheck();
	theGlobal.m_Judge.m_tParam.bUseNGAlarm = ((CButton*)GetDlgItem(IDC_CHECK_USE_NG_MACHINE))->GetCheck();
	theGlobal.m_Judge.m_tParam.dIngoreMeter = m_EDIT_dIngoreMeter;
	theGlobal.m_Judge.m_tParam.dMeanShiftLimitFactor = m_EDIT_dMeanShiftFactor;

	//�ߴ�
	for (int i = 0; i < NG_ABMisalign + 1; i++)
	{
		pRow = m_Grid_SizeAlarm.GetRow(i);

		theGlobal.m_Judge.m_tParam.vecAlarm[i].bUse = (CString)pRow->GetItem(1)->GetValue() == "����" ? 1 : 0;
		theGlobal.m_Judge.m_tParam.vecAlarm[i].hvMeters = (double)pRow->GetItem(2)->GetValue();

		theGlobal.m_Judge.m_tParam.vecStopMachine[i].bUse = (CString)pRow->GetItem(4)->GetValue() == "����" ? 1 : 0;
		theGlobal.m_Judge.m_tParam.vecStopMachine[i].hvMeters = (double)pRow->GetItem(5)->GetValue();
	}
	//AB�ߴ绥У
	pRow = m_Grid_SizeAlarm.GetRow(NG_ABMisalign+1);
	theGlobal.m_Judge.m_tParam.vecAlarm[NG_Count + 0].bUse = (CString)pRow->GetItem(1)->GetValue() == "����" ? 1 : 0;
	theGlobal.m_Judge.m_tParam.vecAlarm[NG_Count + 0].hvMeters = (double)pRow->GetItem(2)->GetValue();

	theGlobal.m_Judge.m_tParam.vecStopMachine[NG_Count + 0].bUse = (CString)pRow->GetItem(4)->GetValue() == "����" ? 1 : 0;
	theGlobal.m_Judge.m_tParam.vecStopMachine[NG_Count + 0].hvMeters = (double)pRow->GetItem(5)->GetValue();
	//Mean Shift
	for (int i = 0; i < NG_ABMisalign + 1; i++)
	{
		pRow = m_Grid_SizeAlarm.GetRow(NG_ABMisalign + 2 + i);

		theGlobal.m_Judge.m_tParam.vecMeanShiftAlarmParam[i].bUse = (CString)pRow->GetItem(1)->GetValue() == "����" ? 1 : 0;
		theGlobal.m_Judge.m_tParam.vecMeanShiftAlarmParam[i].hvMeters = (double)pRow->GetItem(2)->GetValue();

		theGlobal.m_Judge.m_tParam.vecMeanShiftStopMachineParam[i].bUse = (CString)pRow->GetItem(4)->GetValue() == "����" ? 1 : 0;
		theGlobal.m_Judge.m_tParam.vecMeanShiftStopMachineParam[i].hvMeters = (double)pRow->GetItem(5)->GetValue();
	}

	//覴�
	for (int i = NG_ExposeFoil; i < NG_Count; i++)
	{
		pRow = m_Grid_DefectAlarm.GetRow(i - NG_ExposeFoil);

		theGlobal.m_Judge.m_tParam.vecAlarm[i].bUse = (CString)pRow->GetItem(1)->GetValue() == "����" ? 1 : 0;
		theGlobal.m_Judge.m_tParam.vecAlarm[i].hvArea = (double)pRow->GetItem(2)->GetValue();
		theGlobal.m_Judge.m_tParam.vecAlarm[i].hvNum = (int)pRow->GetItem(3)->GetValue();
		theGlobal.m_Judge.m_tParam.vecAlarm[i].hvMeters = (double)pRow->GetItem(4)->GetValue();


		theGlobal.m_Judge.m_tParam.vecStopMachine[i].bUse = (CString)pRow->GetItem(6)->GetValue() == "����" ? 1 : 0;
		theGlobal.m_Judge.m_tParam.vecStopMachine[i].hvArea = (double)pRow->GetItem(7)->GetValue();
		theGlobal.m_Judge.m_tParam.vecStopMachine[i].hvNum = (int)pRow->GetItem(8)->GetValue();
		theGlobal.m_Judge.m_tParam.vecStopMachine[i].hvMeters = (double)pRow->GetItem(9)->GetValue();
	}

	theGlobal.m_Judge.WriteParam();

	OnOK();
}
