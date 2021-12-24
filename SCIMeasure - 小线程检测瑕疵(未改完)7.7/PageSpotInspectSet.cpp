// PageSpotInspectSet.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "PageSpotInspectSet.h"
#include "afxdialogex.h"


// CPageSpotInspectSet �Ի���

IMPLEMENT_DYNAMIC(CPageSpotInspectSet, CBCGPDialog)

CPageSpotInspectSet::CPageSpotInspectSet(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(IDD_DIALOG_SPOTINSPECT_SET, pParent)
	, m_edit_dMinSizePixel(0)
	, m_edit_nThreshold(0)
	, m_edit_dMaxSizePixel(0)
{

}

CPageSpotInspectSet::~CPageSpotInspectSet()
{
}

void CPageSpotInspectSet::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_SPOTINSPTYPE, m_combType);
	DDX_Control(pDX, IDC_COMBO_SPOTINSPNUM, m_combObjNum);
	DDX_Text(pDX, IDC_EDIT_MINSIZE, m_edit_dMinSizePixel);
	DDX_Text(pDX, IDC_EDIT_SPTHRESHOLD, m_edit_nThreshold);
	DDX_Control(pDX, IDC_COMBO_SPOTINSP_COLOR, m_comSpotInspColor);
	DDX_Text(pDX, IDC_EDIT_MAXSIZE, m_edit_dMaxSizePixel);
}


BEGIN_MESSAGE_MAP(CPageSpotInspectSet, CBCGPDialog)
	ON_BN_CLICKED(IDC_BUTTON_GENSPOTINSPREGION, &CPageSpotInspectSet::OnBnClickedButtonGenParm)
	ON_CBN_EDITCHANGE(IDC_COMBO_SPOTINSPTYPE, &CPageSpotInspectSet::OnCbnEditchangeComboSpotinsptype)
	ON_CBN_EDITCHANGE(IDC_COMBO_SPOTINSPNUM, &CPageSpotInspectSet::OnCbnEditchangeComboSpotinspnum)
	ON_EN_CHANGE(IDC_EDIT_MINSIZE, &CPageSpotInspectSet::OnEnChangeEditMinsize)
	ON_EN_CHANGE(IDC_EDIT_SPTHRESHOLD, &CPageSpotInspectSet::OnEnChangeEditSpthreshold)
	ON_BN_CLICKED(IDC_BUTTON_GEN_SPOTINSPECT_REGION, &CPageSpotInspectSet::OnBnClickedButtonGenSpotinspectRegion)
	ON_CBN_SELCHANGE(IDC_COMBO_SPOTINSP_COLOR, &CPageSpotInspectSet::OnCbnSelchangeComboSpotinspColor)
	ON_EN_CHANGE(IDC_EDIT_MAXSIZE, &CPageSpotInspectSet::OnEnChangeEditMaxsize)
END_MESSAGE_MAP()


// CPageSpotInspectSet ��Ϣ�������


BOOL CPageSpotInspectSet::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();
	EnableVisualManagerStyle(TRUE, TRUE);
	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	InitUI();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}


BOOL CPageSpotInspectSet::PreTranslateMessage(MSG* pMsg)
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


void CPageSpotInspectSet::PostNcDestroy()
{
	// TODO: �ڴ����ר�ô����/����û���
	delete this;
	CBCGPDialog::PostNcDestroy();
}

void CPageSpotInspectSet::InitUI()
{
	CSpotInspection::tagInspectionParam tParam = m_pOwners->m_SpotInspection.GetParam();

	m_combType.AddString(_T("����"));
	m_combType.AddString(_T("Բ��"));
	m_combType.SetCurSel(m_pOwners->m_SpotInspection.GetType());
	m_combObjNum.AddString(_T("1"));
	m_combObjNum.AddString(_T("2"));
	m_combObjNum.SetCurSel(m_pOwners->m_tInit.hvCamSNList.Length() - 1);
	m_comSpotInspColor.AddString(_T("�׵�ڱ���"));
	m_comSpotInspColor.AddString(_T("�ڵ�ױ���"));
	m_comSpotInspColor.SetCurSel(tParam.eColorType);

	m_edit_nThreshold = tParam.nThres;
	m_edit_dMinSizePixel = tParam.dPixelMinSize;
	m_edit_dMaxSizePixel = tParam.dPixelMaxSize;

	UpdateData(FALSE);
}

void CPageSpotInspectSet::RefreshUIData()
{
	CSpotInspection::tagInspectionParam tParam = m_pOwners->m_SpotInspection.GetParam();
	m_pOwners->m_SpotInspection.SetType((CSpotInspection::eInspectionType)m_combType.GetCurSel());
	tParam.eColorType = (CSpotInspection::eInspectionColor)m_comSpotInspColor.GetCurSel();
	tParam.nThres = m_edit_nThreshold;
	tParam.dPixelMinSize = m_edit_dMinSizePixel;
	tParam.dPixelMaxSize = m_edit_dMaxSizePixel;
	tParam.nObjectNum = m_combObjNum.GetCurSel() + 1;
	m_pOwners->m_SpotInspection.SetParam(tParam);
}

void CPageSpotInspectSet::OnBnClickedButtonGenParm()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CSpotInspection::SpotInspInput tIn;
	tIn.hoImage = m_pParentDlg->m_tInput.hoGrabImage;
	tIn.nMode = SpotInspectionMode;

	CSpotInspection::InspectionOutput tOut;
	if (!m_pOwners->DoSomething("GenSpotInspectParm", &tIn, &tOut))
	{
		BCGPMessageBox("һ�����ɵ�����ʧ�ܣ�");
		return;
	}

	m_pParentDlg->m_Wnd_Image.ClearMessage();
	m_pParentDlg->m_Wnd_Image.DispImage(tIn.hoImage, FALSE);
	m_pParentDlg->m_Wnd_Image.DispRegion(tOut.hoDisp, "blue", "margin", 2, TRUE);

	BCGPMessageBox("һ�����ɵ��������");
}


void CPageSpotInspectSet::OnBnClickedButtonGenSpotinspectRegion()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_pParentDlg->m_Wnd_Image.SetDrawing(TRUE);
	m_pParentDlg->m_Wnd_Image.DispImage(FALSE);

	CSpotInspection::tagInspectionParam tParam = m_pOwners->m_SpotInspection.GetParam();
	//if (m_pOwners->m_tInit.hvCamSNList.Length() == 2)
	{
		HTuple hvRow1, hvCol1, hvRow2, hvCol2;
		HObject hoRegion;
		GenEmptyObj(&tParam.hoSearchReigon);
		SetColor(m_pParentDlg->m_Wnd_Image.GetDispWnd(), "green");
		
		CString strMsg;
		strMsg.Format("�������϶�������������������, ����Ҽ����");
		m_pParentDlg->m_Wnd_Image.DispMessage(strMsg, XDL_LeftTop, "green");
		DrawRectangle1(m_pParentDlg->m_Wnd_Image.GetDispWnd(), &hvRow1, &hvCol1, &hvRow2, &hvCol2);
		GenRectangle1(&hoRegion, hvRow1, hvCol1, hvRow2, hvCol2);
		ConcatObj(tParam.hoSearchReigon, hoRegion, &tParam.hoSearchReigon);

		strMsg.Format("�������϶�������������������, ����Ҽ����");
		m_pParentDlg->m_Wnd_Image.DispMessage(strMsg, XDL_LeftTop, "green");
		DrawRectangle1(m_pParentDlg->m_Wnd_Image.GetDispWnd(), &hvRow1, &hvCol1, &hvRow2, &hvCol2);
		GenRectangle1(&hoRegion, hvRow1, hvCol1, hvRow2, hvCol2);
		ConcatObj(tParam.hoSearchReigon, hoRegion, &tParam.hoSearchReigon);
	}
	

	m_pOwners->m_SpotInspection.SetParam(tParam);
	DispRegion(tParam.hoSearchReigon, m_pParentDlg->m_Wnd_Image.GetDispWnd());
	m_pParentDlg->m_Wnd_Image.SetDrawing(FALSE);

	BCGPMessageBox("����������������");
}


void CPageSpotInspectSet::OnCbnEditchangeComboSpotinsptype()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	RefreshUIData();
}


void CPageSpotInspectSet::OnCbnEditchangeComboSpotinspnum()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	RefreshUIData();
}


void CPageSpotInspectSet::OnEnChangeEditMinsize()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CBCGPDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	RefreshUIData();

}


void CPageSpotInspectSet::OnEnChangeEditSpthreshold()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CBCGPDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	RefreshUIData();
}



void CPageSpotInspectSet::OnCbnSelchangeComboSpotinspColor()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	RefreshUIData();
}


void CPageSpotInspectSet::OnEnChangeEditMaxsize()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CBCGPDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	RefreshUIData();
}
