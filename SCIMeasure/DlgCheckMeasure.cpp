// DlgCheckMeasure.cpp : 实现文件
//

#include "stdafx.h"
#include "DlgCheckMeasure.h"
#include "afxdialogex.h"


// CDlgCheckMeasure 对话框

IMPLEMENT_DYNAMIC(CDlgCheckMeasure, CBCGPDialog)

CDlgCheckMeasure::CDlgCheckMeasure(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(IDD_DIALOG_CHECK_MEASURE, pParent)
{
	m_ftTitle.CreatePointFont(250, "黑体");
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


// CDlgCheckMeasure 消息处理程序


BOOL CDlgCheckMeasure::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (pMsg->message == WM_KEYDOWN || pMsg->message == WM_SYSKEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN)//屏蔽回车
			return TRUE;
		else if (pMsg->wParam == VK_ESCAPE)//屏蔽ESC
			return TRUE;
		else if (pMsg->wParam == VK_SPACE)//
			return TRUE;
		else if (pMsg->wParam == VK_F4  && HIBYTE(::GetKeyState(VK_MENU)))//屏蔽ALT+F4
			return TRUE;
	}
	return CBCGPDialog::PreTranslateMessage(pMsg);
}

void CDlgCheckMeasure::InitUI()
{
	GetDlgItem(IDC_STATIC_TITLE)->SetFont(&m_ftTitle);
	CString strTitle;
	strTitle.Format("请确认%s寻边是否正确?", m_pTemp->m_tInit.nSN == 0 ? "正面" : "反面");
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

	//显示测量具备细节
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
		strName.Format("%d边,", i);
		CLineMeasure::eLineType eLT = m_pTemp->m_LineMeasure.GetMeasureParam().tLineProp.vecLineTypeList[i];
		if (CLineMeasure::LT_BgFoil == eLT)
			strName += "基材边";
		else if (CLineMeasure::LT_FoilAT11 == eLT)
			strName += "极耳AT边";
		else if (CLineMeasure::LT_AT11Coat == eLT)
			strName += "AT涂膜边";
		else if (CLineMeasure::LT_CoatAT11 == eLT)
			strName += "涂膜AT边";
		else if (CLineMeasure::LT_AT11Foil == eLT)
			strName += "AT极耳边";
		else if (CLineMeasure::LT_FoilBg == eLT)
			strName += "基材边";
		else if (CLineMeasure::LT_FoilCoat == eLT)
			strName += "极耳涂膜边";
		else if (CLineMeasure::LT_CoatFoil == eLT)
			strName += "涂膜极耳边";
		else if (CLineMeasure::LT_FoilBaseCoat == eLT)
			strName += "极耳凹版边";
		else if (CLineMeasure::LT_BaseCoatFoil == eLT)
			strName += "凹版极耳边";
		else if (CLineMeasure::LT_BaseCoatCoat == eLT)
			strName += "凹版涂膜边";
		else
			strName += "涂膜凹版边";
		
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
	// TODO:  在此添加额外的初始化
	InitUI();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CDlgCheckMeasure::OnBnClickedButtonCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	OnCancel();
}


void CDlgCheckMeasure::OnBnClickedButtonOk()
{
	// TODO: 在此添加控件通知处理程序代码
	//关闭算法对话框
	OnOK();
	m_pParent->OnCancel();
}
