// DlgDefctImageDisp.cpp: 实现文件
//

#include "stdafx.h"
#include "BatteriesVision.h"
#include "DlgDefctImageDisp.h"
#include "afxdialogex.h"


// DlgDefctImageDisp 对话框

IMPLEMENT_DYNAMIC(DlgDefctImageDisp, CBCGPDialog)

DlgDefctImageDisp::DlgDefctImageDisp(CWnd* pParent /*=nullptr*/)
	: CBCGPDialog(IDD_DIALOG_DEFECT_DISP, pParent)
{

}

DlgDefctImageDisp::~DlgDefctImageDisp()
{
}

void DlgDefctImageDisp::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(DlgDefctImageDisp, CBCGPDialog)
	ON_BN_CLICKED(IDOK, &DlgDefctImageDisp::OnBnClickedOk)
END_MESSAGE_MAP()


// DlgDefctImageDisp 消息处理程序


BOOL DlgDefctImageDisp::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();
	SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOREPOSITION | SWP_NOSIZE);
	// TODO:  在此添加额外的初始化
	EnableVisualManagerStyle(TRUE, TRUE);
	ResetList();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void DlgDefctImageDisp::PostNcDestroy()
{
	// TODO: 在此添加专用代码和/或调用基类
	delete this;
	CBCGPDialog::PostNcDestroy();
}


BOOL DlgDefctImageDisp::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (pMsg->message == WM_KEYDOWN || pMsg->message == WM_SYSKEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN)//屏蔽回车
		{
			return TRUE;
		}
		else if (pMsg->wParam == VK_ESCAPE)//屏蔽ESC
		{
			return TRUE;
		}
		else if (pMsg->wParam == VK_SPACE)//
		{
			return TRUE;
		}
	}
	return CBCGPDialog::PreTranslateMessage(pMsg);
}

void DlgDefctImageDisp::ShowAlarm(C_MergeWork& tOutput)
{
	UpdateList(tOutput);
	ShowWindow(SW_SHOW);
}

void DlgDefctImageDisp::UpdateList(C_MergeWork& tOutput)
{
	try
	{
		HObject hoRgn, hoRect, hoCrop;
		SelectObj(tOutput.vecB[0].tMeasure.tDefect.ho_Region, &hoRgn, 1);
		HTuple hvRow1, hvRow2, hvCol1, hvCol2, hvWidth, hvHeight, hvCenterRow, hvCenterCol, hvRegionWidth, hvRegionHeight;
		GetImageSize(tOutput.vecB[0].tMeasure.ho_DispImage, &hvWidth, &hvHeight);
		SmallestRectangle1(hoRgn, &hvRow1, &hvCol1, &hvRow2, &hvCol2);
		RegionFeatures(hoRgn, "width", &hvRegionWidth);
		RegionFeatures(hoRgn, "height", &hvRegionHeight);
		AreaCenter(hoRgn, &HTuple(), &hvCenterRow, &hvCenterCol);
		int r0 = (int)max(0, hvCenterRow.D() - hvRegionHeight.D() * 2);
		int c0 = (int)max(0, hvCenterCol.D() - hvRegionWidth.D() * 2);
		int r1 = (int)min(hvHeight.D() - 1, hvCenterRow.D() + hvRegionHeight.D() * 2);
		int c1 = (int)min(hvWidth.D() - 1, hvCenterCol.D() + hvRegionWidth.D() * 2);
		CropRectangle1(tOutput.vecB[0].tMeasure.ho_DispImage, &hoCrop, r0, c0, r1, c1);

		GetImageSize(hoCrop, &hvWidth, &hvHeight);
		SetPart(m_hWndHandle, 0, 0, hvHeight - 1, hvWidth - 1);
		DispObj(hoCrop, m_hWndHandle);
	}
	catch (const std::exception&)
	{

	}
	
}

void DlgDefctImageDisp::ResetList()
{
	CRect rcPic, rcDlg;
	GetClientRect(rcDlg);

	SetCheck("~father");
	///////两个窗体//
	SetWindowAttr("background_color", "#353539");//#252525

	rcPic.left = rcDlg.left + 1;
	rcPic.right = rcDlg.right - 1;

	rcPic.top = rcDlg.top + 1;
	rcPic.bottom = rcDlg.bottom - 1;

	HTuple hvTmp;
	OpenWindow(rcPic.top, rcPic.left, rcPic.Width(), rcPic.Height(),
		(Hlong)this->GetSafeHwnd(), "visible", "", &m_hWndHandle);

	SetCheck("father");

	CString strBG;
	strBG.Format("%s\\res\\bg.png", GetCurrentAppPath());
	if (!PathFileExists(strBG))
		return;
	HObject hoBG;
	HTuple hvW, hvH, hvNum;
	ReadImage(&hoBG, (HTuple)strBG);
	GetImageSize(hoBG, &hvW, &hvH);
	SetPart(m_hWndHandle, 0, 0, hvH - 1, hvW - 1);
	DispObj(hoBG, m_hWndHandle);
}

void DlgDefctImageDisp::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	ShowWindow(SW_HIDE);
}
