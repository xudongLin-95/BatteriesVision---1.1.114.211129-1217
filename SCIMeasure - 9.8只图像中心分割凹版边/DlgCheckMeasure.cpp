// DlgCheckMeasure.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "DlgCheckMeasure.h"
#include "afxdialogex.h"


// CDlgCheckMeasure �Ի���

IMPLEMENT_DYNAMIC(CDlgCheckMeasure, CBCGPDialog)

CDlgCheckMeasure::CDlgCheckMeasure(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(IDD_DIALOG_CHECK_MEASURE, pParent)
{
	m_ftTitle.CreatePointFont(250, "����");
}

CDlgCheckMeasure::~CDlgCheckMeasure()
{
	m_ftTitle.DeleteObject();
}

void CDlgCheckMeasure::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgCheckMeasure, CBCGPDialog)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, &CDlgCheckMeasure::OnBnClickedButtonCancel)
	ON_BN_CLICKED(IDC_BUTTON_OK, &CDlgCheckMeasure::OnBnClickedButtonOk)
END_MESSAGE_MAP()


// CDlgCheckMeasure ��Ϣ�������


BOOL CDlgCheckMeasure::PreTranslateMessage(MSG* pMsg)
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

void CDlgCheckMeasure::InitUI()
{
	GetDlgItem(IDC_STATIC_TITLE)->SetFont(&m_ftTitle);
	CString strTitle;
	strTitle.Format("��ȷ��%sѰ���Ƿ���ȷ?", m_pTemp->m_tInit.nSN == 0 ? "����" : "����");
	GetDlgItem(IDC_STATIC_TITLE)->SetWindowTextA(strTitle);

	int nLineNum = m_pTemp->m_LineMeasure.m_tMeasureParam.vecLineParam.size();
	CRect rcShow;
	GetDlgItem(IDC_STATIC_SHOW)->GetWindowRect(rcShow);
	ScreenToClient(rcShow);

	int GAP_COL = 2;
	int GAP_ROW = 2;

	int WIDTH = (rcShow.Width() - ((nLineNum + 1) / 2 - 1) * GAP_COL) / ((nLineNum + 1) / 2);
	int HEIGHT = (rcShow.Height() - GAP_ROW) / 2;

	for (int i = 0; i < nLineNum; i++)
	{
		CRect rcSingle;
		rcSingle.left = rcShow.left + (i % ((nLineNum + 1) / 2)) * (WIDTH + GAP_COL);
		rcSingle.right = rcSingle.left + WIDTH;
		rcSingle.top = rcShow.top + i / ((nLineNum + 1) / 2)*(HEIGHT + GAP_ROW);
		rcSingle.bottom = rcSingle.top + HEIGHT;

		m_ImageWnd[i].Create("", WS_CHILD | WS_VISIBLE | SS_CENTER, rcSingle, this, i + 3000);
	}

	HTuple hvWidth, hvHeight;
	GetImageSize(m_tOutput.ho_DispImage, &hvWidth, &hvHeight);

	//��ʾ�����߱�ϸ��
	for (int i = 0; i < nLineNum; i++)
	{
		double dC = (m_tOutput.tLine.hv_StartColumn[i].D() + m_tOutput.tLine.hv_EndColumn[i].D()) / 2;
		HObject hoCrop, hoLine;
		int r0 = (int)max(0, hvHeight.D()/2 - 100);
		int c0 = (int)max(0, dC - 100);
		int r1 = (int)min(hvHeight.D() - 1, hvHeight.D() / 2 + 100);
		int c1 = (int)min(hvWidth.D() - 1, dC + 100);
		CropRectangle1(m_tOutput.ho_DispImage, &hoCrop, r0, c0, r1, c1);
		GenRegionLine(&hoLine, 50, dC - c0, 150, dC - c0);

		CString strName;
		strName.Format("%d��,", i);
		CLineMeasure::eLineType eLT = m_pTemp->m_LineMeasure.GetMeasureParam().tLineProp.vecLineTypeList[i];
		if (CLineMeasure::LT_BgFoil == eLT)
			strName += "���ı�";
		else if (CLineMeasure::LT_FoilAT11 == eLT)
			strName += "����AT��";
		else if (CLineMeasure::LT_AT11Coat == eLT)
			strName += "ATͿĤ��";
		else if (CLineMeasure::LT_CoatAT11 == eLT)
			strName += "ͿĤAT��";
		else if (CLineMeasure::LT_AT11Foil == eLT)
			strName += "AT������";
		else if (CLineMeasure::LT_FoilBg == eLT)
			strName += "���ı�";
		else if (CLineMeasure::LT_FoilCoat == eLT)
			strName += "����ͿĤ��";
		else if (CLineMeasure::LT_CoatFoil == eLT)
			strName += "ͿĤ������";
		else if (CLineMeasure::LT_FoilBaseCoat == eLT)
			strName += "���������";
		else if (CLineMeasure::LT_BaseCoatFoil == eLT)
			strName += "���漫����";
		else if (CLineMeasure::LT_BaseCoatCoat == eLT)
			strName += "����ͿĤ��";
		else
			strName += "ͿĤ�����";
		
		m_ImageWnd[i].ClearImage();
		m_ImageWnd[i].ClearRegion();
		m_ImageWnd[i].ClearMessage();
		m_ImageWnd[i].DispImage(hoCrop);
		//m_ImageWnd[i].DispMessage(strName, XDL_LeftTop, "green");
		m_ImageWnd[i].DispMessage(strName, 1, 1, "green", DCS_Image, FALSE);
		m_ImageWnd[i].DispRegion(hoLine, "medium slate blue", "margin", 2, FALSE);
	}
}

BOOL CDlgCheckMeasure::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();
	EnableVisualManagerStyle(TRUE, TRUE);
	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	InitUI();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}


void CDlgCheckMeasure::OnBnClickedButtonCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	OnCancel();
}


void CDlgCheckMeasure::OnBnClickedButtonOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//�ر��㷨�Ի���
	OnOK();
	m_pParent->OnCancel();
}
