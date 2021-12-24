// PageMeasureSet.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "PageMeasureSet.h"
#include "afxdialogex.h"


// CPageMeasureSet �Ի���

IMPLEMENT_DYNAMIC(CPageMeasureSet, CBCGPDialog)

CPageMeasureSet::CPageMeasureSet(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(CPageMeasureSet::IDD, pParent)
	, m_nMeasureLineSN(0)
	, m_dSearchLineLength(0)
	, m_nThreshold(0)
	, m_dSlide(0)
	, m_nSearchDir(0)
	, m_nJumpDir(0)
	, m_nMeasurePointSelect(0)
	, m_EDT_dUncoatAreaRate(0)
	, m_EDT_nUncoatThresLight(0)
	, m_EDT_nUncoatThresDark(0)
	, m_Combo_nFoilSelect(0)
	, m_Combo_nMisalignmentSelect(0)
	, m_Combo_nCoatSelect(0)
	, m_EDIT_nMeasureLeanDiff(0)
{
}

CPageMeasureSet::~CPageMeasureSet()
{
}

void CPageMeasureSet::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_MEASURE_SN, m_nMeasureLineSN);
	DDX_Text(pDX, IDC_EDIT_MEASURE_LENGTH, m_dSearchLineLength);
	DDX_Text(pDX, IDC_EDIT_MEASURE_THRES, m_nThreshold);
	DDX_Text(pDX, IDC_EDIT_MEASURE_SMOOTH, m_dSlide);
	DDX_CBIndex(pDX, IDC_COMBO_MEASURE_DIRECTION, m_nSearchDir);
	DDX_CBIndex(pDX, IDC_COMBO_MEASURE_TRANSITION, m_nJumpDir);
	DDX_CBIndex(pDX, IDC_COMBO_MEASURE_SORT, m_nMeasurePointSelect);
	DDX_Text(pDX, IDC_EDIT_UNCOAT_AREA_RATE, m_EDT_dUncoatAreaRate);
	DDX_Text(pDX, IDC_EDIT_UNCOAT_THRES_LIGHT, m_EDT_nUncoatThresLight);
	DDX_Text(pDX, IDC_EDIT_UNCOAT_THRES_DARK, m_EDT_nUncoatThresDark);
	DDX_CBIndex(pDX, IDC_COMBO_FOIL_SELECT, m_Combo_nFoilSelect);
	DDX_CBIndex(pDX, IDC_COMBO_MISALIGNMENT_SELECT, m_Combo_nMisalignmentSelect);
	DDX_CBIndex(pDX, IDC_COMBO_COAT_SELECT, m_Combo_nCoatSelect);
	DDX_Text(pDX, IDC_EDIT_MEASURE_LEAN_DIFF, m_EDIT_nMeasureLeanDiff);
}

BEGIN_MESSAGE_MAP(CPageMeasureSet, CBCGPDialog)
	ON_BN_CLICKED(IDC_BUTTON_MEASURELINE_RESET, &CPageMeasureSet::OnBnClickedButtonMeasurelineReset)
	ON_BN_CLICKED(IDC_CHECK_SHOW_MEASURELINE, &CPageMeasureSet::OnBnClickedCheckShowMeasureline)
	ON_EN_CHANGE(IDC_EDIT_MEASURE_LENGTH, &CPageMeasureSet::OnEnChangeEditMeasureLength)
	ON_EN_CHANGE(IDC_EDIT_MEASURE_THRES, &CPageMeasureSet::OnEnChangeEditMeasureThres)
	ON_EN_CHANGE(IDC_EDIT_MEASURE_SMOOTH, &CPageMeasureSet::OnEnChangeEditMeasureSmooth)
	ON_CBN_SELCHANGE(IDC_COMBO_MEASURE_DIRECTION, &CPageMeasureSet::OnCbnSelchangeComboMeasureDirection)
	ON_CBN_SELCHANGE(IDC_COMBO_MEASURE_TRANSITION, &CPageMeasureSet::OnCbnSelchangeComboMeasureTransition)
	ON_CBN_SELCHANGE(IDC_COMBO_MEASURE_SORT, &CPageMeasureSet::OnCbnSelchangeComboMeasureSort)
	ON_BN_CLICKED(IDC_BUTTON_SINGLE_LINE_TEST, &CPageMeasureSet::OnBnClickedButtonSingleLineTest)
	ON_BN_CLICKED(IDC_BUTTON_SINGLE_LINE_MOVE, &CPageMeasureSet::OnBnClickedButtonSingleLineMove)
	ON_BN_CLICKED(IDC_CHECK_JUDGE_UNCOAT, &CPageMeasureSet::OnBnClickedCheckJudgeUncoat)
	ON_EN_CHANGE(IDC_EDIT_UNCOAT_THRES, &CPageMeasureSet::OnEnChangeEditUncoatThresLight)
	ON_EN_CHANGE(IDC_EDIT_UNCOAT_AREA_RATE, &CPageMeasureSet::OnEnChangeEditUncoatAreaRate)
	ON_BN_CLICKED(IDC_BUTTON_LEFT, &CPageMeasureSet::OnBnClickedButtonLeft)
	ON_BN_CLICKED(IDC_BUTTON_RIGHT, &CPageMeasureSet::OnBnClickedButtonRight)
	ON_BN_CLICKED(IDC_BUTTON_MOVE_LEFT, &CPageMeasureSet::OnBnClickedButtonMoveLeft)
	ON_BN_CLICKED(IDC_BUTTON_MOVE_RIGHT, &CPageMeasureSet::OnBnClickedButtonMoveRight)
	ON_CBN_SELCHANGE(IDC_COMBO_FOIL_SELECT, &CPageMeasureSet::OnCbnSelchangeComboFoilSelect)
	ON_CBN_SELCHANGE(IDC_COMBO_MISALIGNMENT_SELECT, &CPageMeasureSet::OnCbnSelchangeComboMisalignmentSelect)
	ON_CBN_SELCHANGE(IDC_COMBO_COAT_SELECT, &CPageMeasureSet::OnCbnSelchangeComboCoatSelect)
	ON_BN_CLICKED(IDC_BUTTON_MEASURELINE_RESET2, &CPageMeasureSet::OnBnClickedButtonMeasurelineReset2)
	ON_EN_CHANGE(IDC_EDIT_MEASURE_LEAN_DIFF, &CPageMeasureSet::OnEnChangeEditMeasureLeanDiff)
	ON_EN_CHANGE(IDC_EDIT_UNCOAT_THRES_DARK, &CPageMeasureSet::OnEnChangeEditUncoatThresDark)
END_MESSAGE_MAP()

// CPageMeasureSet ��Ϣ�������

BOOL CPageMeasureSet::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();
	EnableVisualManagerStyle(TRUE, TRUE);
	// TODO:  �ڴ���Ӷ���ĳ�ʼ��	
	InitUI();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}

BOOL CPageMeasureSet::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���
	if (pMsg->message == WM_KEYDOWN || pMsg->message == WM_SYSKEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN)//���λس�
			return TRUE;
		else if (pMsg->wParam == VK_ESCAPE)//����ESC
			return TRUE;
		else if (pMsg->wParam == VK_SPACE)//
			return TRUE;
		else if (pMsg->wParam == VK_F4  && HIBYTE(::GetKeyState(VK_MENU)))//����ALT+F4
			return TRUE;
	}
	return CBCGPDialog::PreTranslateMessage(pMsg);
}

void CPageMeasureSet::PostNcDestroy()
{
	// TODO: �ڴ����ר�ô����/����û���
	delete this;
	CBCGPDialog::PostNcDestroy();
}

void CPageMeasureSet::InitUI()
{
	DispSearchLineParam(0);

	m_Combo_nFoilSelect = m_pOwners->m_tMeasureAdvanceParam.eFoilSelect;
	m_Combo_nMisalignmentSelect = m_pOwners->m_tMeasureAdvanceParam.eMisalignmentSelect;
	m_Combo_nCoatSelect = m_pOwners->m_tMeasureAdvanceParam.eCoatSelect;
	//((CBCGPComboBox*)GetDlgItem(IDC_COMBO_FOIL_SELECT))->SetCurSel();
	//((CBCGPComboBox*)GetDlgItem(IDC_COMBO_MISALIGNMENT_SELECT))->SetCurSel();
	
	m_EDIT_nMeasureLeanDiff = m_pOwners->m_LineMeasure.m_tMeasureParam.dMaxLeanDiff;
	((CBCGPButton*)GetDlgItem(IDC_CHECK_JUDGE_UNCOAT))->SetCheck(m_pOwners->m_tPreprocParam.bUseJudgeUncoat);
	m_EDT_dUncoatAreaRate = m_pOwners->m_tPreprocParam.dUncoatAreaRate;
	m_EDT_nUncoatThresLight = m_pOwners->m_tPreprocParam.nUncoatThresLight;
	m_EDT_nUncoatThresDark = m_pOwners->m_tPreprocParam.nUncoatThresDark;

	UpdateData(FALSE);

	BOOL bCheck = FALSE;
	GetDlgItem(IDC_BUTTON_LEFT)->EnableWindow(bCheck);
	GetDlgItem(IDC_BUTTON_RIGHT)->EnableWindow(bCheck);
	GetDlgItem(IDC_BUTTON_MOVE_LEFT)->EnableWindow(bCheck);
	GetDlgItem(IDC_BUTTON_MOVE_RIGHT)->EnableWindow(bCheck);
	GetDlgItem(IDC_BUTTON_SINGLE_LINE_MOVE)->EnableWindow(bCheck);
	GetDlgItem(IDC_BUTTON_SINGLE_LINE_TEST)->EnableWindow(bCheck);
}

void CPageMeasureSet::RefreshUIData()
{
	if (m_pOwners->m_LineMeasure.m_tMeasureParam.vecLineParam.size() < m_nMeasureLineSN + 1) return;

	UpdateData(TRUE);

	CLineMeasure::tagLineParam tParam = m_pOwners->m_LineMeasure.m_tMeasureParam.vecLineParam[m_nMeasureLineSN];
	//m_nMeasureLineSN = nSN;
	tParam.dWidth = m_dSearchLineLength;
	tParam.nThr = m_nThreshold;
	tParam.dSigma = m_dSlide;
	tParam.nDirection = m_nSearchDir;
	tParam.nTransition = m_nJumpDir;
	tParam.nSelect = m_nMeasurePointSelect;

	m_pOwners->m_LineMeasure.m_tMeasureParam.vecLineParam[m_nMeasureLineSN] = tParam;

	m_pParentDlg->m_nSearchLineSN = m_nMeasureLineSN;
	m_pParentDlg->GenSearchRegion();
	BOOL bCheck = ((CBCGPButton*)GetDlgItem(IDC_CHECK_SHOW_MEASURELINE))->GetCheck();
	m_pParentDlg->DispSearchLine(bCheck);

	m_pOwners->m_tPreprocParam.bUseJudgeUncoat = ((CBCGPButton*)GetDlgItem(IDC_CHECK_JUDGE_UNCOAT))->GetCheck();
	m_pOwners->m_tPreprocParam.dUncoatAreaRate = m_EDT_dUncoatAreaRate;
	m_pOwners->m_tPreprocParam.nUncoatThresLight = m_EDT_nUncoatThresLight;
	m_pOwners->m_tPreprocParam.nUncoatThresDark = m_EDT_nUncoatThresDark;

	//m_pOwners->m_tMeasureAdvanceParam.eFoilSelect = (CMeasure::eFoilSelect)((CBCGPComboBox*)GetDlgItem(IDC_COMBO_FOIL_SELECT))->GetCurSel();
	//m_pOwners->m_tMeasureAdvanceParam.eMisalignmentSelect = (CMeasure::eMisalignmentSelect)((CBCGPComboBox*)GetDlgItem(IDC_COMBO_MISALIGNMENT_SELECT))->GetCurSel();

	m_pOwners->m_tMeasureAdvanceParam.eFoilSelect = (CMeasure::eFoilSelect)m_Combo_nFoilSelect;
	m_pOwners->m_tMeasureAdvanceParam.eMisalignmentSelect = (CMeasure::eMisalignmentSelect)m_Combo_nMisalignmentSelect;
	m_pOwners->m_tMeasureAdvanceParam.eCoatSelect = (CMeasure::eCoatSelect)m_Combo_nCoatSelect;
	m_pOwners->m_LineMeasure.m_tMeasureParam.dMaxLeanDiff = m_EDIT_nMeasureLeanDiff;
}

void CPageMeasureSet::OnBnClickedButtonMeasurelineReset()
{
	CLineMeasure::GenLineInput tIn;
	CLineMeasure::GenLineOutput tOut;
	tIn.hoImage = m_pParentDlg->m_tInput.hoGrabImage;

	if (!m_pOwners->DoSomething("GenMeasureLine", &tIn, &tOut))
	{
		BCGPMessageBox("һ���������ɲ�����ʧ��");
		return;
	}

	m_pParentDlg->m_Wnd_Image.ClearMessage();
	m_pParentDlg->m_Wnd_Image.DispImage(tIn.hoImage,FALSE);
	m_pParentDlg->m_Wnd_Image.DispRegion(tOut.hoDisp, "blue", "margin", 2, TRUE);
	m_pParentDlg->m_Wnd_Image.DispRegion(tOut.hoLine, "green", "margin",2, FALSE);

	BCGPMessageBox("һ���������ɲ��������");
}

void CPageMeasureSet::OnBnClickedCheckShowMeasureline()
{
	BOOL bCheck = ((CBCGPButton*)GetDlgItem(IDC_CHECK_SHOW_MEASURELINE))->GetCheck();

	m_pParentDlg->m_nSearchLineSN = min(max(0, m_pParentDlg->m_nSearchLineSN - 1), m_pOwners->m_tInit.tProductInfo.nMeasureLineNum - 1);
	m_pParentDlg->DispSearchLine(bCheck);

	GetDlgItem(IDC_BUTTON_LEFT)->EnableWindow(bCheck);
	GetDlgItem(IDC_BUTTON_RIGHT)->EnableWindow(bCheck);
	GetDlgItem(IDC_BUTTON_MOVE_LEFT)->EnableWindow(bCheck);
	GetDlgItem(IDC_BUTTON_MOVE_RIGHT)->EnableWindow(bCheck);
	GetDlgItem(IDC_BUTTON_SINGLE_LINE_MOVE)->EnableWindow(bCheck);
	GetDlgItem(IDC_BUTTON_SINGLE_LINE_TEST)->EnableWindow(bCheck);
}

void CPageMeasureSet::DispSearchLineParam(int nSN)
{
	if (nSN < 0) return;
	if (m_pOwners->m_LineMeasure.m_tMeasureParam.vecLineParam.size() < nSN + 1) return;

	CLineMeasure::tagLineParam tParam = m_pOwners->m_LineMeasure.m_tMeasureParam.vecLineParam[nSN];
	m_nMeasureLineSN = nSN;
	m_dSearchLineLength = tParam.dWidth;
	m_nThreshold = tParam.nThr;
	m_dSlide = tParam.dSigma;
	m_nSearchDir = tParam.nDirection;
	m_nJumpDir = tParam.nTransition;
	m_nMeasurePointSelect = tParam.nSelect;

	((CBCGPButton*)GetDlgItem(IDC_CHECK_JUDGE_UNCOAT))->SetCheck(m_pOwners->m_tPreprocParam.bUseJudgeUncoat);
	m_EDT_dUncoatAreaRate = m_pOwners->m_tPreprocParam.dUncoatAreaRate;
	m_EDT_nUncoatThresLight = m_pOwners->m_tPreprocParam.nUncoatThresLight;
	m_EDT_nUncoatThresDark = m_pOwners->m_tPreprocParam.nUncoatThresDark;

	UpdateData(FALSE);

	BOOL bCheck = ((CBCGPButton*)GetDlgItem(IDC_CHECK_SHOW_MEASURELINE))->GetCheck();
	m_pParentDlg->DispSearchLine(bCheck);
	if (bCheck)
	{
		CLineMeasure::tagLineMeasureInput tIn;
		CLineMeasure::tagLineMeasureOutput tOut;
		tIn.hoImage = m_pParentDlg->m_Wnd_Image.GetDispImage();
		tIn.nMode = DebugMode;
		m_pOwners->m_LineMeasure.FindSingleLine(m_nMeasureLineSN, tIn, tOut, TRUE);

		m_pParentDlg->m_Wnd_Image.DispRegion(tOut.hoDisp, "green", "margin", 2, FALSE);
	}
}

void CPageMeasureSet::OnEnChangeEditMeasureLength()
{
	RefreshUIData();
}


void CPageMeasureSet::OnEnChangeEditMeasureThres()
{
	RefreshUIData();
}


void CPageMeasureSet::OnEnChangeEditMeasureSmooth()
{
	RefreshUIData();
}


void CPageMeasureSet::OnCbnSelchangeComboMeasureDirection()
{
	RefreshUIData();
}


void CPageMeasureSet::OnCbnSelchangeComboMeasureTransition()
{
	RefreshUIData();
}


void CPageMeasureSet::OnCbnSelchangeComboMeasureSort()
{
	RefreshUIData();
	//m_pOwners->m_LineMeasure.FindSingleLine(m_nSearchLineSN, &tIn, &tOut);
}

void CPageMeasureSet::OnBnClickedButtonSingleLineTest()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	CLineMeasure::tagLineMeasureInput tIn;
	CLineMeasure::tagLineMeasureOutput tOut;
	tIn.hoImage = m_pParentDlg->m_Wnd_Image.GetDispImage();
	tIn.nMode = DebugMode;
	m_pOwners->m_LineMeasure.FindSingleLine(m_nMeasureLineSN, tIn, tOut, TRUE);

	m_pParentDlg->m_Wnd_Image.DispRegion(tOut.hoDisp, "green", "margin", 2, TRUE);
}


void CPageMeasureSet::OnBnClickedButtonSingleLineMove()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_pParentDlg->MoveSearchReion();
}


void CPageMeasureSet::OnBnClickedCheckJudgeUncoat()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	RefreshUIData();
}


void CPageMeasureSet::OnEnChangeEditUncoatThresLight()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CBCGPDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	RefreshUIData();
}


void CPageMeasureSet::OnEnChangeEditUncoatAreaRate()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CBCGPDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	RefreshUIData();
}

void CPageMeasureSet::OnBnClickedButtonLeft()
{
	m_pParentDlg->m_nSearchLineSN = min(max(0, m_pParentDlg->m_nSearchLineSN - 1), m_pOwners->m_tInit.tProductInfo.nMeasureLineNum - 1);
	DispSearchLineParam(m_pParentDlg->m_nSearchLineSN);
}

void CPageMeasureSet::OnBnClickedButtonRight()
{
	m_pParentDlg->m_nSearchLineSN = min(max(0, m_pParentDlg->m_nSearchLineSN + 1), m_pOwners->m_tInit.tProductInfo.nMeasureLineNum - 1);
	DispSearchLineParam(m_pParentDlg->m_nSearchLineSN);
}

void CPageMeasureSet::OnBnClickedButtonMoveLeft()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_pParentDlg->MoveSearchRegionStep(TRUE);
}

void CPageMeasureSet::OnBnClickedButtonMoveRight()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_pParentDlg->MoveSearchRegionStep(FALSE);
}

void CPageMeasureSet::OnCbnSelchangeComboFoilSelect()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	RefreshUIData();
}

void CPageMeasureSet::OnCbnSelchangeComboMisalignmentSelect()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	RefreshUIData();
}


void CPageMeasureSet::OnCbnSelchangeComboCoatSelect()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	RefreshUIData();
}


void CPageMeasureSet::OnBnClickedButtonMeasurelineReset2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CLineMeasure::GenLineInput tIn;
	CLineMeasure::GenLineOutput tOut;
	tIn.hoImage = m_pParentDlg->m_tInput.hoGrabImage;

	/*m_pParentDlg->m_Wnd_Image.SetDrawing(TRUE);
	CString strMsg;
	strMsg.Format("����ѡ�������׼�ߵ�λ�ã������Ҽ��������");
	m_pParentDlg->m_Wnd_Image.DispMessage(strMsg, XDL_LeftTop, "green");

	try
	{
		HTuple hvCol;
		DrawPoint(m_pParentDlg->m_Wnd_Image.GetDispWnd(), &HTuple(), &tIn.hvBaseLineColumn);
	}
	catch (HException& e)
	{
		BCGPMessageBox("ѡ�������׼��λ��ʧ��");
		return;
	}

	m_pParentDlg->m_Wnd_Image.SetDrawing(FALSE);*/

	if (!m_pOwners->DoSomething("GenMeasureLineWithStdVal", &tIn, &tOut))
	{
		BCGPMessageBox("һ���������ɲ�����ʧ��");
		return;
	}

	m_pParentDlg->m_Wnd_Image.ClearMessage();
	m_pParentDlg->m_Wnd_Image.DispImage(tIn.hoImage, FALSE);
	m_pParentDlg->m_Wnd_Image.DispRegion(tOut.hoDisp, "blue", "margin", 2, TRUE);
	m_pParentDlg->m_Wnd_Image.DispRegion(tOut.hoLine, "green", "margin", 2, FALSE);

	BCGPMessageBox("һ���������ɲ��������");
}


void CPageMeasureSet::OnEnChangeEditMeasureLeanDiff()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CBCGPDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	RefreshUIData();
}


void CPageMeasureSet::OnEnChangeEditUncoatThresDark()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CBCGPDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	RefreshUIData();
}
