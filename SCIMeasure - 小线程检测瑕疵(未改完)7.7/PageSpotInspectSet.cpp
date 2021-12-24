// PageSpotInspectSet.cpp : 实现文件
//

#include "stdafx.h"
#include "PageSpotInspectSet.h"
#include "afxdialogex.h"


// CPageSpotInspectSet 对话框

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


// CPageSpotInspectSet 消息处理程序


BOOL CPageSpotInspectSet::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();
	EnableVisualManagerStyle(TRUE, TRUE);
	// TODO:  在此添加额外的初始化
	InitUI();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


BOOL CPageSpotInspectSet::PreTranslateMessage(MSG* pMsg)
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


void CPageSpotInspectSet::PostNcDestroy()
{
	// TODO: 在此添加专用代码和/或调用基类
	delete this;
	CBCGPDialog::PostNcDestroy();
}

void CPageSpotInspectSet::InitUI()
{
	CSpotInspection::tagInspectionParam tParam = m_pOwners->m_SpotInspection.GetParam();

	m_combType.AddString(_T("矩形"));
	m_combType.AddString(_T("圆形"));
	m_combType.SetCurSel(m_pOwners->m_SpotInspection.GetType());
	m_combObjNum.AddString(_T("1"));
	m_combObjNum.AddString(_T("2"));
	m_combObjNum.SetCurSel(m_pOwners->m_tInit.hvCamSNList.Length() - 1);
	m_comSpotInspColor.AddString(_T("白点黑背景"));
	m_comSpotInspColor.AddString(_T("黑点白背景"));
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
	// TODO: 在此添加控件通知处理程序代码
	CSpotInspection::SpotInspInput tIn;
	tIn.hoImage = m_pParentDlg->m_tInput.hoGrabImage;
	tIn.nMode = SpotInspectionMode;

	CSpotInspection::InspectionOutput tOut;
	if (!m_pOwners->DoSomething("GenSpotInspectParm", &tIn, &tOut))
	{
		BCGPMessageBox("一键生成点检参数失败！");
		return;
	}

	m_pParentDlg->m_Wnd_Image.ClearMessage();
	m_pParentDlg->m_Wnd_Image.DispImage(tIn.hoImage, FALSE);
	m_pParentDlg->m_Wnd_Image.DispRegion(tOut.hoDisp, "blue", "margin", 2, TRUE);

	BCGPMessageBox("一键生成点检参数完成");
}


void CPageSpotInspectSet::OnBnClickedButtonGenSpotinspectRegion()
{
	// TODO: 在此添加控件通知处理程序代码
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
		strMsg.Format("请点击和拖动鼠标绘制左相机点检区域, 点击右键完成");
		m_pParentDlg->m_Wnd_Image.DispMessage(strMsg, XDL_LeftTop, "green");
		DrawRectangle1(m_pParentDlg->m_Wnd_Image.GetDispWnd(), &hvRow1, &hvCol1, &hvRow2, &hvCol2);
		GenRectangle1(&hoRegion, hvRow1, hvCol1, hvRow2, hvCol2);
		ConcatObj(tParam.hoSearchReigon, hoRegion, &tParam.hoSearchReigon);

		strMsg.Format("请点击和拖动鼠标绘制右相机点检区域, 点击右键完成");
		m_pParentDlg->m_Wnd_Image.DispMessage(strMsg, XDL_LeftTop, "green");
		DrawRectangle1(m_pParentDlg->m_Wnd_Image.GetDispWnd(), &hvRow1, &hvCol1, &hvRow2, &hvCol2);
		GenRectangle1(&hoRegion, hvRow1, hvCol1, hvRow2, hvCol2);
		ConcatObj(tParam.hoSearchReigon, hoRegion, &tParam.hoSearchReigon);
	}
	

	m_pOwners->m_SpotInspection.SetParam(tParam);
	DispRegion(tParam.hoSearchReigon, m_pParentDlg->m_Wnd_Image.GetDispWnd());
	m_pParentDlg->m_Wnd_Image.SetDrawing(FALSE);

	BCGPMessageBox("画点检搜索区域完成");
}


void CPageSpotInspectSet::OnCbnEditchangeComboSpotinsptype()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	RefreshUIData();
}


void CPageSpotInspectSet::OnCbnEditchangeComboSpotinspnum()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	RefreshUIData();
}


void CPageSpotInspectSet::OnEnChangeEditMinsize()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CBCGPDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	UpdateData(TRUE);
	RefreshUIData();

}


void CPageSpotInspectSet::OnEnChangeEditSpthreshold()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CBCGPDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	UpdateData(TRUE);
	RefreshUIData();
}



void CPageSpotInspectSet::OnCbnSelchangeComboSpotinspColor()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	RefreshUIData();
}


void CPageSpotInspectSet::OnEnChangeEditMaxsize()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CBCGPDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	UpdateData(TRUE);
	RefreshUIData();
}
