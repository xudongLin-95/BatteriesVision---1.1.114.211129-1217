// PageDefectSet.cpp : 实现文件
//

#include "stdafx.h"
#include "PageDefectSet.h"
#include "afxdialogex.h"
#include "DlgParam.h"
#include "Defect.h"

// CPageDefectSet 对话框

IMPLEMENT_DYNAMIC(CPageDefectSet, CBCGPDialog)

CPageDefectSet::CPageDefectSet(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(IDD_DIALOG_DEFECT_SET, pParent)
	, m_CHK_bUseBubbleDetect(FALSE)
	, m_CHK_bUseDarkDotDetect(FALSE)
	, m_CHK_bUseDarkLineDetect(FALSE)
	, m_CHK_bUseFoilDetect(FALSE)
	, m_CHK_bUseLeakFoilDetect(FALSE)
	, m_CHK_bUseLightDotDetect(FALSE)
	, m_CHK_bUseLightLineDetect(FALSE)
	, m_CHK_bUseDropDetect(FALSE)
	, m_CHK_bUseTapeDetect(FALSE)
	, m_EDT_dAT9DarkMinWidth(0)
	, m_EDT_nAT9MinThresh(0)
	, m_EDT_nAT9MaxThresh(0)
	, m_EDT_dAT9DarkMinArea(0)
	, m_EDT_dAT9MinHeight(0)
	, m_EDT_dAT9MinWidth(0)
	, m_EDT_dBorderDarkCoff(0)
	, m_EDT_dBorderErosionLength(0)
	, m_EDT_dBorderLightCoff(0)
	, m_EDT_nBubbleDynThresh(0)
	, m_EDT_nBubbleInOutDiff(0)
	, m_EDT_dBubbleMinCircularity(0)
	, m_EDT_dBubbleMinDiameter(0)
	, m_EDT_dDarkDotMaxArea(0)
	, m_EDT_nDarkDotMaxThresh(0)
	, m_EDT_dDarkDotMinArea(0)
	, m_EDT_dDarkDotMinHeight(0)
	, m_EDT_dDarkDotMinWidth(0)
	, m_EDT_nDarkLineDynThresh(0)
	, m_EDT_dDarkLineMinHeight(0)
	, m_EDT_dDarkLineMinRatio(0)
	, m_EDT_dDarkLineMinWidth(0)
	, m_EDT_dFoilDarkMinArea(0)
	, m_EDT_dFoilDarkMinHeight(0)
	, m_EDT_nFoilDarkMinNum(0)
	, m_EDT_dFoilDarkMinWidth(0)
	, m_EDT_nFoilDarkThresh(0)
	, m_EDT_dFoilLightMinArea(0)
	, m_EDT_nFoilLightThresh(0)
	, m_EDT_dFoilLightMinWidth(0)
	, m_EDT_nLTMaxThresh(0)
	, m_EDT_nJDDarkThresh(0)
	, m_EDT_nJDLightThresh(0)
	, m_EDT_dJDDarkArea(0)
	, m_EDT_dJDLightArea(0)
	, m_EDT_nLeakFoilDynThresh(0)
	, m_EDT_dLeakFoilMaxDiameter(0)
	, m_EDT_dLeakFoilMinArea(0)
	, m_EDT_dLeakFoilMinDeviation(0)
	, m_EDT_dLeakFoilMinDiameter(0)
	, m_EDT_nLeakFoilThresh(0)
	, m_EDT_nLightDotGrayDeviation(0)
	, m_EDT_nLightDotGrayRange(0)
	, m_EDT_dLightDotMaxArea(0)
	, m_EDT_nLightDotMaxThresh(0)
	, m_EDT_dLightDotMinArea(0)
	, m_EDT_dLightDotMinHeight(0)
	, m_EDT_nLightDotMinThresh(0)
	, m_EDT_dLightDotMinWidth(0)
	, m_EDT_nLightLineDynThresh(0)
	, m_EDT_dLightLineMinHeight(0)
	, m_EDT_dLightLineMinRatio(0)
	, m_EDT_dLightLineMinWidth(0)
	, m_EDT_nLTMinThresh(0)
	, m_EDT_dMidDarkCoff(0)
	, m_EDT_dMidLightCoff(0)
	, m_EDT_dLTMinArea(0)
	, m_EDT_dScaleFactor(0)
	, m_EDT_nTapeMaxThresh(0)
	, m_EDT_dTapeMinArea(0)
	, m_EDT_dZoomFactor(0)
	, m_EDIT_nDefectMaxNum(0)
{

}

CPageDefectSet::~CPageDefectSet()
{

}

void CPageDefectSet::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHK_USEBUBBLEDETECT, m_CHK_bUseBubbleDetect);
	DDX_Check(pDX, IDC_CHK_USEDARKDOTDETECT, m_CHK_bUseDarkDotDetect);
	DDX_Check(pDX, IDC_CHK_USEDARKLINEDETECT, m_CHK_bUseDarkLineDetect);
	DDX_Check(pDX, IDC_CHK_USEFOILDETECT, m_CHK_bUseFoilDetect);
	DDX_Check(pDX, IDC_CHK_USELEAKFOILDETECT, m_CHK_bUseLeakFoilDetect);
	DDX_Check(pDX, IDC_CHK_USELIGHTDOTDETECT, m_CHK_bUseLightDotDetect);
	DDX_Check(pDX, IDC_CHK_USELIGHTLINEDETECT, m_CHK_bUseLightLineDetect);
	DDX_Check(pDX, IDC_CHK_USEDROPDETECT, m_CHK_bUseDropDetect);
	DDX_Check(pDX, IDC_CHK_USETAPEDETECT, m_CHK_bUseTapeDetect);
	DDX_Text(pDX, IDC_EDT_AT9DARKMINWIDTH, m_EDT_dAT9DarkMinWidth);
	DDX_Text(pDX, IDC_EDT_AT9LOWTHRESH, m_EDT_nAT9MinThresh);
	DDX_Text(pDX, IDC_EDT_AT9MAXTHRESH, m_EDT_nAT9MaxThresh);
	DDX_Text(pDX, IDC_EDT_AT9MINAREA, m_EDT_dAT9DarkMinArea);
	DDX_Text(pDX, IDC_EDT_AT9MINHEIGHT, m_EDT_dAT9MinHeight);
	DDX_Text(pDX, IDC_EDT_AT9MINWIDTH, m_EDT_dAT9MinWidth);
	DDX_Text(pDX, IDC_EDT_BORDERDARKCOFF, m_EDT_dBorderDarkCoff);
	DDX_Text(pDX, IDC_EDT_BORDEREROSIONLENGTH, m_EDT_dBorderErosionLength);
	DDX_Text(pDX, IDC_EDT_BORDERLIGHTCOFF, m_EDT_dBorderLightCoff);
	DDX_Text(pDX, IDC_EDT_BUBBLEDYNTHRESH, m_EDT_nBubbleDynThresh);
	DDX_Text(pDX, IDC_EDT_BUBBLEINOUTDIFF, m_EDT_nBubbleInOutDiff);
	DDX_Text(pDX, IDC_EDT_BUBBLEMINCIRCULARITY, m_EDT_dBubbleMinCircularity);
	DDX_Text(pDX, IDC_EDT_BUBBLEMINDIAM, m_EDT_dBubbleMinDiameter);
	DDX_Text(pDX, IDC_EDT_DARKDOTMAXAREA, m_EDT_dDarkDotMaxArea);
	DDX_Text(pDX, IDC_EDT_DARKDOTMAXTHRESH, m_EDT_nDarkDotMaxThresh);
	DDX_Text(pDX, IDC_EDT_DARKDOTMINAREA, m_EDT_dDarkDotMinArea);
	DDX_Text(pDX, IDC_EDT_DARKDOTMINHEIGHT, m_EDT_dDarkDotMinHeight);
	DDX_Text(pDX, IDC_EDT_DARKDOTMINWIDTH, m_EDT_dDarkDotMinWidth);
	DDX_Text(pDX, IDC_EDT_DARKLINEDYNTHRESH, m_EDT_nDarkLineDynThresh);
	DDX_Text(pDX, IDC_EDT_DARKLINEMINHEIGHT, m_EDT_dDarkLineMinHeight);
	DDX_Text(pDX, IDC_EDT_DARKLINEMINWHRATIO, m_EDT_dDarkLineMinRatio);
	DDX_Text(pDX, IDC_EDT_DARKLINEMINWIDTH, m_EDT_dDarkLineMinWidth);
	DDX_Text(pDX, IDC_EDT_FOILDARKMINAREA, m_EDT_dFoilDarkMinArea);
	DDX_Text(pDX, IDC_EDT_FOILDARKMINHEIGHT, m_EDT_dFoilDarkMinHeight);
	DDX_Text(pDX, IDC_EDT_FOILDARKMINNUM, m_EDT_nFoilDarkMinNum);
	DDX_Text(pDX, IDC_EDT_FOILDARKMINWIDTH, m_EDT_dFoilDarkMinWidth);
	DDX_Text(pDX, IDC_EDT_FOILDARKTHRESH, m_EDT_nFoilDarkThresh);
	DDX_Text(pDX, IDC_EDT_FOILLIGHTMINAREA, m_EDT_dFoilLightMinArea);
	DDX_Text(pDX, IDC_EDT_FOILLIGHTTHRESH, m_EDT_nFoilLightThresh);
	DDX_Text(pDX, IDC_EDT_FOILLIGHTWIDTH, m_EDT_dFoilLightMinWidth);
	DDX_Text(pDX, IDC_EDT_HTHRESH, m_EDT_nLTMaxThresh);
	DDX_Text(pDX, IDC_EDT_JD_DARKTHRESH, m_EDT_nJDDarkThresh);
	DDX_Text(pDX, IDC_EDT_JD_LIGHTTHRESH, m_EDT_nJDLightThresh);
	DDX_Text(pDX, IDC_EDT_JD_MINDARKAREA, m_EDT_dJDDarkArea);
	DDX_Text(pDX, IDC_EDT_JD_MINLIGHTAREA, m_EDT_dJDLightArea);
	DDX_Text(pDX, IDC_EDT_LEAKFOILDYNTHRESH, m_EDT_nLeakFoilDynThresh);
	DDX_Text(pDX, IDC_EDT_LEAKFOILMAXDIAM, m_EDT_dLeakFoilMaxDiameter);
	DDX_Text(pDX, IDC_EDT_LEAKFOILMINAREA, m_EDT_dLeakFoilMinArea);
	DDX_Text(pDX, IDC_EDT_LEAKFOILMINDEVIATION, m_EDT_dLeakFoilMinDeviation);
	DDX_Text(pDX, IDC_EDT_LEAKFOILMINDIAM, m_EDT_dLeakFoilMinDiameter);
	DDX_Text(pDX, IDC_EDT_LEAKFOILTHRESH, m_EDT_nLeakFoilThresh);
	DDX_Text(pDX, IDC_EDT_LIGHTDOTDEVIATION, m_EDT_nLightDotGrayDeviation);
	DDX_Text(pDX, IDC_EDT_LIGHTDOTGRAYRANGE, m_EDT_nLightDotGrayRange);
	DDX_Text(pDX, IDC_EDT_LIGHTDOTMAXAREA, m_EDT_dLightDotMaxArea);
	DDX_Text(pDX, IDC_EDT_LIGHTDOTMAXTHRESH, m_EDT_nLightDotMaxThresh);
	DDX_Text(pDX, IDC_EDT_LIGHTDOTMINAREA, m_EDT_dLightDotMinArea);
	DDX_Text(pDX, IDC_EDT_LIGHTDOTMINHEIGHT, m_EDT_dLightDotMinHeight);
	DDX_Text(pDX, IDC_EDT_LIGHTDOTMINTHRESH, m_EDT_nLightDotMinThresh);
	DDX_Text(pDX, IDC_EDT_LIGHTDOTMINWIDTH, m_EDT_dLightDotMinWidth);
	DDX_Text(pDX, IDC_EDT_LIGHTLINEDYNTHRESH, m_EDT_nLightLineDynThresh);
	DDX_Text(pDX, IDC_EDT_LIGHTLINEMINHEIGHT, m_EDT_dLightLineMinHeight);
	DDX_Text(pDX, IDC_EDT_LIGHTLINEMINWHRATIO, m_EDT_dLightLineMinRatio);
	DDX_Text(pDX, IDC_EDT_LIGHTLINEMINWIDTH, m_EDT_dLightLineMinWidth);
	DDX_Text(pDX, IDC_EDT_LTHRESH, m_EDT_nLTMinThresh);
	DDX_Text(pDX, IDC_EDT_MIDDARKCOFF, m_EDT_dMidDarkCoff);
	DDX_Text(pDX, IDC_EDT_MIDLIGHTCOFF, m_EDT_dMidLightCoff);
	DDX_Text(pDX, IDC_EDT_MINLIGHTTAPEAREA, m_EDT_dLTMinArea);
	DDX_Text(pDX, IDC_EDT_SCALEFACTOR, m_EDT_dScaleFactor);
	DDX_Text(pDX, IDC_EDT_TAPEMAXTHRESH, m_EDT_nTapeMaxThresh);
	DDX_Text(pDX, IDC_EDT_TAPEMINAREA, m_EDT_dTapeMinArea);
	DDX_Control(pDX, IDC_COM_ERRORNAME, m_Combo_ErrorName);
	DDX_Control(pDX, IDC_BUTTON_FEATURE, m_MenuBtn_FeatureSelect);
	DDX_Text(pDX, IDC_EDT_ZOOMFACTOR, m_EDT_dZoomFactor);
	DDX_Text(pDX, IDC_EDT_DEFECT_MAXNUM, m_EDIT_nDefectMaxNum);
}

BEGIN_MESSAGE_MAP(CPageDefectSet, CBCGPDialog)
	ON_WM_MOUSEHWHEEL()
	ON_WM_GETMINMAXINFO()
	ON_WM_VSCROLL()
	ON_EN_CHANGE(IDC_EDT_BORDEREROSIONLENGTH, &CPageDefectSet::OnEnChangeEdtBordererosionlength)
	ON_EN_CHANGE(IDC_EDT_SCALEFACTOR, &CPageDefectSet::OnEnChangeEdtScalefactor)
	ON_EN_CHANGE(IDC_EDT_MIDLIGHTCOFF, &CPageDefectSet::OnEnChangeEdtMidlightcoff)
	ON_EN_CHANGE(IDC_EDT_MIDDARKCOFF, &CPageDefectSet::OnEnChangeEdtMiddarkcoff)
	ON_EN_CHANGE(IDC_EDT_BORDERLIGHTCOFF, &CPageDefectSet::OnEnChangeEdtBorderlightcoff)
	ON_EN_CHANGE(IDC_EDT_BORDERDARKCOFF, &CPageDefectSet::OnEnChangeEdtBorderdarkcoff)
	ON_BN_CLICKED(IDC_CHK_USELEAKFOILDETECT, &CPageDefectSet::OnBnClickedChkUseleakfoildetect)
	ON_EN_CHANGE(IDC_EDT_LEAKFOILTHRESH, &CPageDefectSet::OnEnChangeEdtLeakfoilthresh)
	ON_EN_CHANGE(IDC_EDT_LEAKFOILMINAREA, &CPageDefectSet::OnEnChangeEdtLeakfoilminarea)
	ON_EN_CHANGE(IDC_EDT_LEAKFOILDYNTHRESH, &CPageDefectSet::OnEnChangeEdtLeakfoildynthresh)
	ON_EN_CHANGE(IDC_EDT_LEAKFOILMINDIAM, &CPageDefectSet::OnEnChangeEdtLeakfoilmindiam)
	ON_EN_CHANGE(IDC_EDT_LEAKFOILMINDEVIATION, &CPageDefectSet::OnEnChangeEdtLeakfoilmindeviation)
	ON_EN_CHANGE(IDC_EDT_LEAKFOILMAXDIAM, &CPageDefectSet::OnEnChangeEdtLeakfoilmaxdiam)
	ON_EN_CHANGE(IDC_EDT_AT9LOWTHRESH, &CPageDefectSet::OnEnChangeEdtAt9lowthresh)
	ON_EN_CHANGE(IDC_EDT_AT9MINWIDTH, &CPageDefectSet::OnEnChangeEdtAt9minwidth)
	ON_EN_CHANGE(IDC_EDT_AT9MINHEIGHT, &CPageDefectSet::OnEnChangeEdtAt9minheight)
	ON_BN_CLICKED(IDC_CHK_USEBUBBLEDETECT, &CPageDefectSet::OnBnClickedChkUsebubbledetect)
	ON_EN_CHANGE(IDC_EDT_BUBBLEDYNTHRESH, &CPageDefectSet::OnEnChangeEdtBubbledynthresh)
	ON_EN_CHANGE(IDC_EDT_BUBBLEMINDIAM, &CPageDefectSet::OnEnChangeEdtBubblemindiam)
	ON_EN_CHANGE(IDC_EDT_BUBBLEMINCIRCULARITY, &CPageDefectSet::OnEnChangeEdtBubblemincircularity)
	ON_EN_CHANGE(IDC_EDT_BUBBLEINOUTDIFF, &CPageDefectSet::OnEnChangeEdtBubbleinoutdiff)
	ON_BN_CLICKED(IDC_CHK_USELIGHTLINEDETECT, &CPageDefectSet::OnBnClickedChkUselightlinedetect)
	ON_EN_CHANGE(IDC_EDT_LIGHTLINEDYNTHRESH, &CPageDefectSet::OnEnChangeEdtLightlinedynthresh)
	ON_EN_CHANGE(IDC_EDT_LIGHTLINEMINWIDTH, &CPageDefectSet::OnEnChangeEdtLightlineminwidth)
	ON_EN_CHANGE(IDC_EDT_LIGHTLINEMINHEIGHT, &CPageDefectSet::OnEnChangeEdtLightlineminheight)
	ON_EN_CHANGE(IDC_EDT_LIGHTLINEMINWHRATIO, &CPageDefectSet::OnEnChangeEdtLightlineminwhratio)
	ON_BN_CLICKED(IDC_CHK_USEDARKLINEDETECT, &CPageDefectSet::OnBnClickedChkUsedarklinedetect)
	ON_EN_CHANGE(IDC_EDT_DARKLINEDYNTHRESH, &CPageDefectSet::OnEnChangeEdtDarklinedynthresh)
	ON_EN_CHANGE(IDC_EDT_DARKLINEMINWIDTH, &CPageDefectSet::OnEnChangeEdtDarklineminwidth)
	ON_EN_CHANGE(IDC_EDT_DARKLINEMINHEIGHT, &CPageDefectSet::OnEnChangeEdtDarklineminheight)
	ON_EN_CHANGE(IDC_EDT_DARKLINEMINWHRATIO, &CPageDefectSet::OnEnChangeEdtDarklineminwhratio)
	ON_EN_CHANGE(IDC_EDT_LIGHTDOTMINTHRESH, &CPageDefectSet::OnEnChangeEdtLightdotminthresh)
	ON_EN_CHANGE(IDC_EDT_LIGHTDOTMAXTHRESH, &CPageDefectSet::OnEnChangeEdtLightdotmaxthresh)
	ON_EN_CHANGE(IDC_EDT_LIGHTDOTMINWIDTH, &CPageDefectSet::OnEnChangeEdtLightdotminwidth)
	ON_EN_CHANGE(IDC_EDT_LIGHTDOTMINHEIGHT, &CPageDefectSet::OnEnChangeEdtLightdotminheight)
	ON_EN_CHANGE(IDC_EDT_LIGHTDOTMINAREA, &CPageDefectSet::OnEnChangeEdtLightdotminarea)
	ON_EN_CHANGE(IDC_EDT_LIGHTDOTMAXAREA, &CPageDefectSet::OnEnChangeEdtLightdotmaxarea)
	ON_EN_CHANGE(IDC_EDT_LIGHTDOTDEVIATION, &CPageDefectSet::OnEnChangeEdtLightdotdeviation)
	ON_EN_CHANGE(IDC_EDT_LIGHTDOTGRAYRANGE, &CPageDefectSet::OnEnChangeEdtLightdotgrayrange)
	ON_BN_CLICKED(IDC_CHK_USEDARKDOTDETECT, &CPageDefectSet::OnBnClickedChkUsedarkdotdetect)
	ON_EN_CHANGE(IDC_EDT_DARKDOTMAXTHRESH, &CPageDefectSet::OnEnChangeEdtDarkdotmaxthresh)
	ON_EN_CHANGE(IDC_EDT_DARKDOTMINWIDTH, &CPageDefectSet::OnEnChangeEdtDarkdotminwidth)
	ON_EN_CHANGE(IDC_EDT_DARKDOTMINHEIGHT, &CPageDefectSet::OnEnChangeEdtDarkdotminheight)
	ON_EN_CHANGE(IDC_EDT_DARKDOTMINAREA, &CPageDefectSet::OnEnChangeEdtDarkdotminarea)
	ON_EN_CHANGE(IDC_EDT_DARKDOTMAXAREA, &CPageDefectSet::OnEnChangeEdtDarkdotmaxarea)
	ON_BN_CLICKED(IDC_CHK_USEDROPDETECT, &CPageDefectSet::OnBnClickedChkUsedropdetect)
	ON_EN_CHANGE(IDC_EDT_TAPEMAXTHRESH, &CPageDefectSet::OnEnChangeEdtTapemaxthresh)
	ON_EN_CHANGE(IDC_EDT_TAPEMINAREA, &CPageDefectSet::OnEnChangeEdtTapeminarea)
	ON_EN_CHANGE(IDC_EDT_LTHRESH, &CPageDefectSet::OnEnChangeEdtLthresh)
	ON_EN_CHANGE(IDC_EDT_HTHRESH, &CPageDefectSet::OnEnChangeEdtHthresh)
	ON_EN_CHANGE(IDC_EDT_MINLIGHTTAPEAREA, &CPageDefectSet::OnEnChangeEdtMinlighttapearea)
	ON_EN_CHANGE(IDC_EDT_AT9MAXTHRESH, &CPageDefectSet::OnEnChangeEdtAt9maxthresh)
	ON_EN_CHANGE(IDC_EDT_AT9DARKMINWIDTH, &CPageDefectSet::OnEnChangeEdtAt9darkminwidth)
	ON_EN_CHANGE(IDC_EDT_AT9MINAREA, &CPageDefectSet::OnEnChangeEdtAt9minarea)
	ON_BN_CLICKED(IDC_CHK_USETAPEDETECT, &CPageDefectSet::OnBnClickedChkUsetapedetect)
	ON_EN_CHANGE(IDC_EDT_JD_DARKTHRESH, &CPageDefectSet::OnEnChangeEdtJdDarkthresh)
	ON_EN_CHANGE(IDC_EDT_JD_MINDARKAREA, &CPageDefectSet::OnEnChangeEdtJdMindarkarea)
	ON_EN_CHANGE(IDC_EDT_JD_LIGHTTHRESH, &CPageDefectSet::OnEnChangeEdtJdLightthresh)
	ON_EN_CHANGE(IDC_EDT_JD_MINLIGHTAREA, &CPageDefectSet::OnEnChangeEdtJdMinlightarea)
	ON_BN_CLICKED(IDC_CHK_USEFOILDETECT, &CPageDefectSet::OnBnClickedChkUsefoildetect)
	ON_EN_CHANGE(IDC_EDT_FOILLIGHTTHRESH, &CPageDefectSet::OnEnChangeEdtFoillightthresh)
	ON_EN_CHANGE(IDC_EDT_FOILLIGHTMINAREA, &CPageDefectSet::OnEnChangeEdtFoillightminarea)
	ON_EN_CHANGE(IDC_EDT_FOILLIGHTWIDTH, &CPageDefectSet::OnEnChangeEdtFoillightwidth)
	ON_EN_CHANGE(IDC_EDT_FOILDARKTHRESH, &CPageDefectSet::OnEnChangeEdtFoildarkthresh)
	ON_EN_CHANGE(IDC_EDT_FOILDARKMINWIDTH, &CPageDefectSet::OnEnChangeEdtFoildarkminwidth)
	ON_EN_CHANGE(IDC_EDT_FOILDARKMINHEIGHT, &CPageDefectSet::OnEnChangeEdtFoildarkminheight)
	ON_EN_CHANGE(IDC_EDT_FOILDARKMINAREA, &CPageDefectSet::OnEnChangeEdtFoildarkminarea)
	ON_EN_CHANGE(IDC_EDT_FOILDARKMINNUM, &CPageDefectSet::OnEnChangeEdtFoildarkminnum)
	ON_BN_CLICKED(IDC_BUTTON_FEATURE, &CPageDefectSet::OnBnClickedButtonFeature)
	ON_BN_CLICKED(IDC_BTN_ADDALGRITHM, &CPageDefectSet::OnBnClickedBtnAddalgrithm)
	ON_BN_CLICKED(IDC_BTN_DELETE, &CPageDefectSet::OnBnClickedBtnDelete)
	ON_BN_CLICKED(IDC_BTN_DELETEALGRITHM, &CPageDefectSet::OnBnClickedBtnDeletealgrithm)
	ON_BN_CLICKED(IDC_BTN_LOADPARAM, &CPageDefectSet::OnBnClickedBtnLoadparam)
	ON_WM_MOUSEWHEEL()
	ON_EN_CHANGE(IDC_EDT_ZOOMFACTOR, &CPageDefectSet::OnEnChangeEdtZoomfactor)
	ON_EN_CHANGE(IDC_EDT_DEFECT_MAXNUM, &CPageDefectSet::OnEnChangeEdtDefectMaxnum)
END_MESSAGE_MAP()

// CPageDefectSet 消息处理程序

BOOL CPageDefectSet::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();
	EnableVisualManagerStyle(TRUE, TRUE);
	// TODO:  在此添加额外的初始化
	InitUI();

	CRect rcMine, rcParent;
	GetClientRect(&rcMine);
	GetParent()->GetClientRect(&rcParent);
	
	SCROLLINFO vinfo;
	vinfo.cbSize = sizeof(SCROLLINFO);
	vinfo.fMask = SIF_ALL;
	//另外nPage取值-1时，滚动条会不见了。
	vinfo.nMax = rcMine.Height() - rcParent.Height();//滚动条所能滚动的最大值
	vinfo.nPage = rcParent.Height();//滚动块自身的长短，通常有如下关系：其长度/滚动条长度（含两个箭头）=nPage/(nMax+2)，
	vinfo.nMin = 0;//滚动条所能滚动的最小值
	vinfo.nTrackPos = 0;
	CBCGPDialog::SetScrollInfo(SB_VERT, &vinfo);//即使上述步骤一不做，使用此条语句也可以显示滚动条
	//CBCGPDialog::GetScrollBarCtrl(0)->SetFocus();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void CPageDefectSet::PostNcDestroy()
{
	// TODO: 在此添加专用代码和/或调用基类
	delete this;
	CBCGPDialog::PostNcDestroy();
}

BOOL CPageDefectSet::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (pMsg->message == WM_KEYDOWN || pMsg->message == WM_SYSKEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN)//屏蔽回车
			return TRUE;
		/*else if (pMsg->message == WM_MOUSEWHEEL)
		{
			short zDelta = HIWORD(pMsg->wParam);
			UINT nFlags = LOWORD(pMsg->wParam);
			OnMouseWheel(nFlags, zDelta, pMsg->pt);
			return TRUE;
		}*/
		else if (pMsg->wParam == VK_ESCAPE)//屏蔽ESC
			return TRUE;
		else if (pMsg->wParam == VK_SPACE)//
			return TRUE;
		else if (pMsg->wParam == VK_F4  && HIBYTE(::GetKeyState(VK_MENU)))//屏蔽ALT+F4
			return TRUE;
	}
	return CBCGPDialog::PreTranslateMessage(pMsg);
}

void CPageDefectSet::FreshParam()
{
	UpdateData(TRUE);
	///预处理参数
	m_pOwners->m_Defect.m_tParameters.tPretreatParam.dZoomFactor = m_EDT_dZoomFactor;
	m_pOwners->m_Defect.m_tParameters.tPretreatParam.dBorderWidth = m_EDT_dBorderErosionLength;
	m_pOwners->m_Defect.m_tParameters.tPretreatParam.dScaleFactor = m_EDT_dScaleFactor;
	m_pOwners->m_Defect.m_tParameters.tPretreatParam.dMidLightCof = m_EDT_dMidLightCoff;
	m_pOwners->m_Defect.m_tParameters.tPretreatParam.dMidDarkCof = m_EDT_dMidDarkCoff;
	m_pOwners->m_Defect.m_tParameters.tPretreatParam.dBorderLightCof = m_EDT_dBorderLightCoff;
	m_pOwners->m_Defect.m_tParameters.tPretreatParam.dBorderDarkCof = m_EDT_dBorderDarkCoff;
	m_pOwners->m_Defect.m_tParameters.tPretreatParam.nMaxDefectNum = m_EDIT_nDefectMaxNum;


	/////漏箔参数
	m_pOwners->m_Defect.m_tParameters.tLeakFoilParam.bUseDetect = m_CHK_bUseLeakFoilDetect;
	m_pOwners->m_Defect.m_tParameters.tLeakFoilParam.nThresh = m_EDT_nLeakFoilThresh;
	m_pOwners->m_Defect.m_tParameters.tLeakFoilParam.dMinArea = m_EDT_dLeakFoilMinArea;
	m_pOwners->m_Defect.m_tParameters.tLeakFoilParam.nDynThresh = m_EDT_nLeakFoilDynThresh;
	m_pOwners->m_Defect.m_tParameters.tLeakFoilParam.dMinDiameter = m_EDT_dLeakFoilMinDiameter;
	m_pOwners->m_Defect.m_tParameters.tLeakFoilParam.dMaxDiameter = m_EDT_dLeakFoilMaxDiameter;
	m_pOwners->m_Defect.m_tParameters.tLeakFoilParam.dDeviation = m_EDT_dLeakFoilMinDeviation;

	//AT11漏箔
	m_pOwners->m_Defect.m_tParameters.tAT11LeakFoilParam.bUseDetect = m_CHK_bUseLeakFoilDetect;
	m_pOwners->m_Defect.m_tParameters.tAT11LeakFoilParam.nMinThresh = m_EDT_nAT9MinThresh;
	m_pOwners->m_Defect.m_tParameters.tAT11LeakFoilParam.dMinWidth = m_EDT_dAT9MinWidth;
	m_pOwners->m_Defect.m_tParameters.tAT11LeakFoilParam.dMinHeight = m_EDT_dAT9MinHeight;

	//气泡
	m_pOwners->m_Defect.m_tParameters.tBubbleParam.bUseDetect = m_CHK_bUseBubbleDetect;
	m_pOwners->m_Defect.m_tParameters.tBubbleParam.nDynThresh = m_EDT_nBubbleDynThresh;
	m_pOwners->m_Defect.m_tParameters.tBubbleParam.dMinDiameter = m_EDT_dBubbleMinDiameter;
	m_pOwners->m_Defect.m_tParameters.tBubbleParam.nInOutGrayDiff = m_EDT_nBubbleInOutDiff;
	m_pOwners->m_Defect.m_tParameters.tBubbleParam.dMinCircularity = m_EDT_dBubbleMinCircularity;

	//白线
	m_pOwners->m_Defect.m_tParameters.tLightLineParam.bUseDetect = m_CHK_bUseLightLineDetect;
	m_pOwners->m_Defect.m_tParameters.tLightLineParam.nDynThresh = m_EDT_nLightLineDynThresh;
	m_pOwners->m_Defect.m_tParameters.tLightLineParam.dMinWidth = m_EDT_dLightLineMinWidth;
	m_pOwners->m_Defect.m_tParameters.tLightLineParam.dMinHeight = m_EDT_dLightLineMinHeight;
	m_pOwners->m_Defect.m_tParameters.tLightLineParam.dMinLengthWidthRatio = m_EDT_dLightLineMinRatio;

	//黑线
	m_pOwners->m_Defect.m_tParameters.tDarkLineParam.bUseDetect = m_CHK_bUseDarkLineDetect;
	m_pOwners->m_Defect.m_tParameters.tDarkLineParam.nDynThresh = m_EDT_nDarkLineDynThresh;
	m_pOwners->m_Defect.m_tParameters.tDarkLineParam.dMinWidth = m_EDT_dDarkLineMinWidth;
	m_pOwners->m_Defect.m_tParameters.tDarkLineParam.dMinHeight = m_EDT_dDarkLineMinHeight;
	m_pOwners->m_Defect.m_tParameters.tDarkLineParam.dMinLengthWidthRatio = m_EDT_dDarkLineMinRatio;

	//白斑
	m_pOwners->m_Defect.m_tParameters.tLightDotParam.bUseDetect = m_CHK_bUseLightDotDetect;
	m_pOwners->m_Defect.m_tParameters.tLightDotParam.nMinThresh = m_EDT_nLightDotMinThresh;
	m_pOwners->m_Defect.m_tParameters.tLightDotParam.nMaxThresh = m_EDT_nLightDotMaxThresh;
	m_pOwners->m_Defect.m_tParameters.tLightDotParam.dMinHeight = m_EDT_dLightDotMinHeight;
	m_pOwners->m_Defect.m_tParameters.tLightDotParam.dMinWidth = m_EDT_dLightDotMinWidth;
	m_pOwners->m_Defect.m_tParameters.tLightDotParam.dMinArea = m_EDT_dLightDotMinArea;
	m_pOwners->m_Defect.m_tParameters.tLightDotParam.dMaxArea = m_EDT_dLightDotMaxArea;

	m_pOwners->m_Defect.m_tParameters.tLightDotParam.nMinDeviation = m_EDT_nLightDotGrayDeviation;
	m_pOwners->m_Defect.m_tParameters.tLightDotParam.nMinGrayRange = m_EDT_nLightDotGrayRange;

	//黑斑
	m_pOwners->m_Defect.m_tParameters.tDarkDotParam.bUseDetect = m_CHK_bUseDarkDotDetect;
	m_pOwners->m_Defect.m_tParameters.tDarkDotParam.nMaxThresh = m_EDT_nDarkDotMaxThresh;
	m_pOwners->m_Defect.m_tParameters.tDarkDotParam.dMinHeight = m_EDT_dDarkDotMinHeight;
	m_pOwners->m_Defect.m_tParameters.tDarkDotParam.dMinWidth = m_EDT_dDarkDotMinWidth;
	m_pOwners->m_Defect.m_tParameters.tDarkDotParam.dMinArea = m_EDT_dDarkDotMinArea;
	m_pOwners->m_Defect.m_tParameters.tDarkDotParam.dMaxArea = m_EDT_dDarkDotMaxArea;

	//脱碳
	m_pOwners->m_Defect.m_tParameters.tDropParam.bUseDetect = m_CHK_bUseDropDetect;
	m_pOwners->m_Defect.m_tParameters.tDropParam.nMaxThresh = m_EDT_nTapeMaxThresh;
	m_pOwners->m_Defect.m_tParameters.tDropParam.dMinArea = m_EDT_dTapeMinArea;
	m_pOwners->m_Defect.m_tParameters.tDropParam.nLTMinThresh = m_EDT_nLTMinThresh;
	m_pOwners->m_Defect.m_tParameters.tDropParam.nLTMaxThresh = m_EDT_nLTMaxThresh;
	m_pOwners->m_Defect.m_tParameters.tDropParam.dLTMinArea = m_EDT_dLTMinArea;

	m_pOwners->m_Defect.m_tParameters.tDropParam.nAT9MaxThresh = m_EDT_nAT9MaxThresh;
	m_pOwners->m_Defect.m_tParameters.tDropParam.dAT9DarkMinWidth = m_EDT_dAT9DarkMinWidth;
	m_pOwners->m_Defect.m_tParameters.tDropParam.dAT9DarkMinArea = m_EDT_dAT9DarkMinArea;

	//极耳缺陷
	m_pOwners->m_Defect.m_tParameters.tFoilParam.bUseDetect = m_CHK_bUseFoilDetect;
	m_pOwners->m_Defect.m_tParameters.tFoilParam.nMaxDarkThresh = m_EDT_nFoilDarkThresh;
	m_pOwners->m_Defect.m_tParameters.tFoilParam.dMinDarkWidth = m_EDT_dFoilDarkMinWidth;
	m_pOwners->m_Defect.m_tParameters.tFoilParam.dMinDarkHeight = m_EDT_dFoilDarkMinHeight;
	m_pOwners->m_Defect.m_tParameters.tFoilParam.dMinDarkArea = m_EDT_dFoilDarkMinArea;
	m_pOwners->m_Defect.m_tParameters.tFoilParam.nMaxDarkNum = m_EDT_nFoilDarkMinNum;
	m_pOwners->m_Defect.m_tParameters.tFoilParam.nLightThresh = m_EDT_nFoilLightThresh;
	m_pOwners->m_Defect.m_tParameters.tFoilParam.dMinLightArea = m_EDT_dFoilLightMinArea;
	m_pOwners->m_Defect.m_tParameters.tFoilParam.dFoilLightMinWidth = m_EDT_dFoilLightMinWidth;

	///////胶带检测参数
	m_pOwners->m_Defect.m_tParameters.tTapeParam.bUseDetect = m_CHK_bUseTapeDetect;
	m_pOwners->m_Defect.m_tParameters.tTapeParam.nDarkThresh = m_EDT_nJDDarkThresh;
	m_pOwners->m_Defect.m_tParameters.tTapeParam.dDarkMinArea = m_EDT_dJDDarkArea;
	m_pOwners->m_Defect.m_tParameters.tTapeParam.nLightThresh = m_EDT_nJDLightThresh;
	m_pOwners->m_Defect.m_tParameters.tTapeParam.dLightMinArea = m_EDT_dJDLightArea;

}

void CPageDefectSet::InitUI()
{
	//预处理
	///预处理参数
	m_EDT_dZoomFactor = m_pOwners->m_Defect.m_tParameters.tPretreatParam.dZoomFactor;
	m_EDT_dBorderErosionLength = m_pOwners->m_Defect.m_tParameters.tPretreatParam.dBorderWidth;
	m_EDT_dScaleFactor = m_pOwners->m_Defect.m_tParameters.tPretreatParam.dScaleFactor;
	m_EDT_dMidLightCoff = m_pOwners->m_Defect.m_tParameters.tPretreatParam.dMidLightCof;
	m_EDT_dMidDarkCoff = m_pOwners->m_Defect.m_tParameters.tPretreatParam.dMidDarkCof;
	m_EDT_dBorderLightCoff = m_pOwners->m_Defect.m_tParameters.tPretreatParam.dBorderLightCof;
	m_EDT_dBorderDarkCoff = m_pOwners->m_Defect.m_tParameters.tPretreatParam.dBorderDarkCof;
	m_EDIT_nDefectMaxNum = m_pOwners->m_Defect.m_tParameters.tPretreatParam.nMaxDefectNum;

	/////漏箔参数
	m_CHK_bUseLeakFoilDetect = m_pOwners->m_Defect.m_tParameters.tLeakFoilParam.bUseDetect;
	m_EDT_nLeakFoilThresh = m_pOwners->m_Defect.m_tParameters.tLeakFoilParam.nThresh;
	m_EDT_dLeakFoilMinArea = m_pOwners->m_Defect.m_tParameters.tLeakFoilParam.dMinArea;
	m_EDT_nLeakFoilDynThresh = m_pOwners->m_Defect.m_tParameters.tLeakFoilParam.nDynThresh;
	m_EDT_dLeakFoilMinDiameter = m_pOwners->m_Defect.m_tParameters.tLeakFoilParam.dMinDiameter;
	m_EDT_dLeakFoilMaxDiameter = m_pOwners->m_Defect.m_tParameters.tLeakFoilParam.dMaxDiameter;
	m_EDT_dLeakFoilMinDeviation = m_pOwners->m_Defect.m_tParameters.tLeakFoilParam.dDeviation;

	//AT11漏箔
	m_CHK_bUseLeakFoilDetect = m_pOwners->m_Defect.m_tParameters.tAT11LeakFoilParam.bUseDetect;
	m_EDT_nAT9MinThresh = m_pOwners->m_Defect.m_tParameters.tAT11LeakFoilParam.nMinThresh;
	m_EDT_dAT9MinWidth = m_pOwners->m_Defect.m_tParameters.tAT11LeakFoilParam.dMinWidth;
	m_EDT_dAT9MinHeight = m_pOwners->m_Defect.m_tParameters.tAT11LeakFoilParam.dMinHeight;

	//气泡
	m_CHK_bUseBubbleDetect = m_pOwners->m_Defect.m_tParameters.tBubbleParam.bUseDetect;
	m_EDT_nBubbleDynThresh = m_pOwners->m_Defect.m_tParameters.tBubbleParam.nDynThresh;
	m_EDT_dBubbleMinDiameter = m_pOwners->m_Defect.m_tParameters.tBubbleParam.dMinDiameter;
	m_EDT_nBubbleInOutDiff = m_pOwners->m_Defect.m_tParameters.tBubbleParam.nInOutGrayDiff;
	m_EDT_dBubbleMinCircularity = m_pOwners->m_Defect.m_tParameters.tBubbleParam.dMinCircularity;

	//白线
	m_CHK_bUseLightLineDetect = m_pOwners->m_Defect.m_tParameters.tLightLineParam.bUseDetect;
	m_EDT_nLightLineDynThresh = m_pOwners->m_Defect.m_tParameters.tLightLineParam.nDynThresh;
	m_EDT_dLightLineMinWidth = m_pOwners->m_Defect.m_tParameters.tLightLineParam.dMinWidth;
	m_EDT_dLightLineMinHeight = m_pOwners->m_Defect.m_tParameters.tLightLineParam.dMinHeight;
	m_EDT_dLightLineMinRatio = m_pOwners->m_Defect.m_tParameters.tLightLineParam.dMinLengthWidthRatio;

	//黑线
	m_CHK_bUseDarkLineDetect = m_pOwners->m_Defect.m_tParameters.tDarkLineParam.bUseDetect;
	m_EDT_nDarkLineDynThresh = m_pOwners->m_Defect.m_tParameters.tDarkLineParam.nDynThresh;
	m_EDT_dDarkLineMinWidth = m_pOwners->m_Defect.m_tParameters.tDarkLineParam.dMinWidth;
	m_EDT_dDarkLineMinHeight = m_pOwners->m_Defect.m_tParameters.tDarkLineParam.dMinHeight;
	m_EDT_dDarkLineMinRatio = m_pOwners->m_Defect.m_tParameters.tDarkLineParam.dMinLengthWidthRatio;

	//白斑
	m_CHK_bUseLightDotDetect = m_pOwners->m_Defect.m_tParameters.tLightDotParam.bUseDetect;
	m_EDT_nLightDotMinThresh = m_pOwners->m_Defect.m_tParameters.tLightDotParam.nMinThresh;
	m_EDT_nLightDotMaxThresh = m_pOwners->m_Defect.m_tParameters.tLightDotParam.nMaxThresh;
	m_EDT_dLightDotMinHeight = m_pOwners->m_Defect.m_tParameters.tLightDotParam.dMinHeight;
	m_EDT_dLightDotMinWidth = m_pOwners->m_Defect.m_tParameters.tLightDotParam.dMinWidth;
	m_EDT_dLightDotMinArea = m_pOwners->m_Defect.m_tParameters.tLightDotParam.dMinArea;
	m_EDT_dLightDotMaxArea = m_pOwners->m_Defect.m_tParameters.tLightDotParam.dMaxArea;

	m_EDT_nLightDotGrayDeviation = m_pOwners->m_Defect.m_tParameters.tLightDotParam.nMinDeviation;
	m_EDT_nLightDotGrayRange = m_pOwners->m_Defect.m_tParameters.tLightDotParam.nMinGrayRange;

	//黑斑
	m_CHK_bUseDarkDotDetect = m_pOwners->m_Defect.m_tParameters.tDarkDotParam.bUseDetect;
	m_EDT_nDarkDotMaxThresh = m_pOwners->m_Defect.m_tParameters.tDarkDotParam.nMaxThresh;
	m_EDT_dDarkDotMinHeight = m_pOwners->m_Defect.m_tParameters.tDarkDotParam.dMinHeight;
	m_EDT_dDarkDotMinWidth = m_pOwners->m_Defect.m_tParameters.tDarkDotParam.dMinWidth;
	m_EDT_dDarkDotMinArea = m_pOwners->m_Defect.m_tParameters.tDarkDotParam.dMinArea;
	m_EDT_dDarkDotMaxArea = m_pOwners->m_Defect.m_tParameters.tDarkDotParam.dMaxArea;

	//脱碳
	m_CHK_bUseDropDetect = m_pOwners->m_Defect.m_tParameters.tDropParam.bUseDetect;
	m_EDT_nTapeMaxThresh = m_pOwners->m_Defect.m_tParameters.tDropParam.nMaxThresh;
	m_EDT_dTapeMinArea = m_pOwners->m_Defect.m_tParameters.tDropParam.dMinArea;
	m_EDT_nLTMinThresh = m_pOwners->m_Defect.m_tParameters.tDropParam.nLTMinThresh;
	m_EDT_nLTMaxThresh = m_pOwners->m_Defect.m_tParameters.tDropParam.nLTMaxThresh;
	m_EDT_dLTMinArea = m_pOwners->m_Defect.m_tParameters.tDropParam.dLTMinArea;

	m_EDT_nAT9MaxThresh = m_pOwners->m_Defect.m_tParameters.tDropParam.nAT9MaxThresh;
	m_EDT_dAT9DarkMinWidth = m_pOwners->m_Defect.m_tParameters.tDropParam.dAT9DarkMinWidth;
	m_EDT_dAT9DarkMinArea = m_pOwners->m_Defect.m_tParameters.tDropParam.dAT9DarkMinArea;

	//极耳缺陷
	m_CHK_bUseFoilDetect = m_pOwners->m_Defect.m_tParameters.tFoilParam.bUseDetect;
	m_EDT_nFoilDarkThresh = m_pOwners->m_Defect.m_tParameters.tFoilParam.nMaxDarkThresh;
	m_EDT_dFoilDarkMinWidth = m_pOwners->m_Defect.m_tParameters.tFoilParam.dMinDarkWidth;
	m_EDT_dFoilDarkMinHeight = m_pOwners->m_Defect.m_tParameters.tFoilParam.dMinDarkHeight;
	m_EDT_dFoilDarkMinArea = m_pOwners->m_Defect.m_tParameters.tFoilParam.dMinDarkArea;
	m_EDT_nFoilDarkMinNum = m_pOwners->m_Defect.m_tParameters.tFoilParam.nMaxDarkNum;
	m_EDT_nFoilLightThresh = m_pOwners->m_Defect.m_tParameters.tFoilParam.nLightThresh;
	m_EDT_dFoilLightMinArea = m_pOwners->m_Defect.m_tParameters.tFoilParam.dMinLightArea;
	m_EDT_dFoilLightMinWidth = m_pOwners->m_Defect.m_tParameters.tFoilParam.dFoilLightMinWidth;

	///////胶带检测参数
	m_CHK_bUseTapeDetect = m_pOwners->m_Defect.m_tParameters.tTapeParam.bUseDetect;
	m_EDT_nJDDarkThresh = m_pOwners->m_Defect.m_tParameters.tTapeParam.nDarkThresh;
	m_EDT_dJDDarkArea = m_pOwners->m_Defect.m_tParameters.tTapeParam.dDarkMinArea;
	m_EDT_nJDLightThresh = m_pOwners->m_Defect.m_tParameters.tTapeParam.nLightThresh;
	m_EDT_dJDLightArea = m_pOwners->m_Defect.m_tParameters.tTapeParam.dLightMinArea;

	//缺陷名
	m_Combo_ErrorName.Clear();
	for (int k = 0; k < m_pOwners->m_Defect.m_hvErrorName.Length(); k++)
	{
		m_Combo_ErrorName.AddString(m_pOwners->m_Defect.m_hvErrorName[k].S());
	}
	m_Combo_ErrorName.SetCurSel(0);

	//特征按钮按钮
	CMenu *pMenu = new CMenu;
	pMenu->LoadMenu(IDR_MENU1);
	pMenu->InsertMenuA(0, MF_BYPOSITION | MF_POPUP | MF_STRING, (int)pMenu->m_hMenu, "子菜单");
	for (int i = 0; i<FEATURE_COUNT; i++)
	{
		pMenu->GetSubMenu(0)->AppendMenuA(MF_STRING, i + 1, NAME_Feature[i]);//这里要用i+1,因为0不能作为响应

	}
	m_MenuBtn_FeatureSelect.m_hMenu = pMenu->GetSubMenu(0)->GetSafeHmenu();

	//特征列表
	CBCGPGridColors theme;
	CRect rcProp, rcGridParam;
	GetDlgItem(IDC_STATIC_GRID)->GetWindowRect(rcGridParam);
	ScreenToClient(rcGridParam);
	m_Grid_Feature.Create(WS_CHILD, rcGridParam, this, 1000);
	CBCGPVisualManager::GetInstance()->OnSetGridColorTheme(&m_Grid_Feature, theme);
	m_Grid_Feature.SendMessage(BCGM_ONSETCONTROLVMMODE, TRUE);
	m_Grid_Feature.SetColorTheme(theme);
	m_Grid_Feature.SetReadOnly(FALSE);//不可编辑
	m_Grid_Feature.SetWholeRowSel(TRUE);
	m_Grid_Feature.EnableMarkSortedColumn(FALSE);//使用列排序
	m_Grid_Feature.EnableHeader(TRUE, BCGP_GRID_HEADER_MOVE_ITEMS);//使用表头
	m_Grid_Feature.EnableRowHeader(TRUE);//使用行表头
	m_Grid_Feature.EnableLineNumbers(TRUE);//使用行序号
	m_Grid_Feature.SetClearInplaceEditOnEnter(FALSE);
	m_Grid_Feature.EnableInvertSelOnCtrl(FALSE);
	m_Grid_Feature.InsertColumn(0, "特征", 100);
	m_Grid_Feature.InsertColumn(1, "最小值", 160);
	m_Grid_Feature.InsertColumn(2, "最大值", 160);
	m_Grid_Feature.LoadState(_T("BasicGrid"));
	m_Grid_Feature.AdjustLayout();//相当于数据显示在表格上
	m_Grid_Feature.ShowWindow(SW_NORMAL);
	
	GetDlgItem(IDC_STATIC_PROP)->GetWindowRect(rcProp);
	ScreenToClient(rcProp);
	m_PropList_Defect.Create(WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER, rcProp, this, 2000);
	m_PropList_Defect.EnableToolBar(TRUE);//设置工具栏
	m_PropList_Defect.EnableSearchBox(TRUE, "Search");//设置搜索框
	m_PropList_Defect.EnableContextMenu(FALSE);
	m_PropList_Defect.EnableHeaderCtrl();//表头
	m_PropList_Defect.EnableDesciptionArea();//描述区域
	m_PropList_Defect.SetVSDotNetLook(FALSE);
	m_PropList_Defect.MarkModifiedProperties(TRUE);
	m_PropList_Defect.SetNameAlign(DT_LEFT);
	m_PropList_Defect.SetAlphabeticMode(FALSE);//按照字母次序的方式,目录
	m_PropList_Defect.SetShowDragContext(TRUE);//显示上下文
	m_PropList_Defect.SetBooleanPropertiesStyle(CBCGPPropList::BOOL_PROPS_SWITCH);//bool类型选择开关
	m_PropList_Defect.SetDescriptionRows(4);
	m_PropList_Defect.SetPropertyColumnWidth(250);

	CDefect::tagTPARAMETERS tParam = m_pParentDlg->m_pTemp->m_Defect.m_tParameters;
	for (int i = 0; i < tParam.tFeatureSelect.nGroupNum; i++)
	{
		CDefect::SingleErrorGroup tSingleErrorGroup;
		tSingleErrorGroup = tParam.tFeatureSelect.Vec_FSelects.at(i);


		CBCGPProp *pGoup0 = new CBCGPProp(_T(tSingleErrorGroup.strErrorName));
		for (int k = 0; k<tSingleErrorGroup.nSingleGroupNum; k = k + 3)
		{
			CDefect::SingleFeature tSingleFeature;
			tSingleFeature = tSingleErrorGroup.Vec_tSingleGroupFeatures.at(k / 3);
			CBCGPProp *pGoup1 = new CBCGPProp(_T("特征"));
			pGoup0->AddSubItem(new CBCGPProp(_T("特征"), (_variant_t)tSingleFeature.StrSingleFeatureName, _T("")));
			pGoup0->AddSubItem(new CBCGPProp(_T("最小值"), (_variant_t)tSingleFeature.dMin, _T("")));
			pGoup0->AddSubItem(new CBCGPProp(_T("最大值"), (_variant_t)tSingleFeature.dMax, _T("")));
		}
		m_PropList_Defect.AddProperty(pGoup0);
	}

	UpdateData(FALSE);
}

void CPageDefectSet::AddFeature(EM_Feature eFeature)
{
	if (eFeature > FEATURE_COUNT - 1)return;
	if (m_Grid_Feature.GetRowCount() > 0)
	{
		//遍历特征名，防止添加重复
		for (int i = 0; i < m_Grid_Feature.GetRowCount(); i++)
		{
			if ((CString)m_Grid_Feature.GetRow(i)->GetItem(0)->GetValue() == NAME_Feature[eFeature]) return;
		}
	}
	
	CBCGPGridRow* pRow = m_Grid_Feature.CreateRow(m_Grid_Feature.GetColumnCount());
	pRow->GetItem(0)->SetValue(_variant_t(NAME_Feature[eFeature]), FALSE);
	pRow->GetItem(0)->AllowEdit(FALSE);
	CString strValue;
	strValue.Format("%.2f", 0);
	pRow->GetItem(1)->SetValue(_variant_t(strValue), FALSE);
	strValue.Format("%.2f", 999999.00);
	pRow->GetItem(2)->SetValue(_variant_t(strValue), FALSE);
	m_Grid_Feature.AddRow(pRow, FALSE);
	m_Grid_Feature.AdjustLayout();
}

void CPageDefectSet::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	SCROLLINFO scrollinfo;
	GetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);

	switch (nSBCode)
	{
	case SB_BOTTOM: //滑块滚动到最底部
		ScrollWindow(0, (scrollinfo.nPos - scrollinfo.nMax) * 10); //滚动屏幕
		scrollinfo.nPos = scrollinfo.nMax; //设定滑块新位置
		SetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL); //更新滑块位置
		break;
	case SB_TOP: //滑块滚动到最顶部
		ScrollWindow(0, (scrollinfo.nPos - scrollinfo.nMin) * 10);
		scrollinfo.nPos = scrollinfo.nMin;
		SetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);
		break;
	case SB_LINEUP: //单击上箭头
		scrollinfo.nPos -= 1;
		if (scrollinfo.nPos < scrollinfo.nMin)
		{
			//ScrollWindow(0, (scrollinfo.nPos - scrollinfo.nMin) * 10);
			scrollinfo.nPos = scrollinfo.nMin;
			SetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);
			break;
		}
		SetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);
		ScrollWindow(0, 10);
		break;
	case SB_LINEDOWN: //单击下箭头
		scrollinfo.nPos += 1;
		if (scrollinfo.nPos > scrollinfo.nMax)
		{
			//ScrollWindow(0, (scrollinfo.nPos - scrollinfo.nMax) * 10); //滚动屏幕
			scrollinfo.nPos = scrollinfo.nMax; //设定滑块新位置
			SetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL); //更新滑块位置
			break;
		}
		SetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);
		ScrollWindow(0, -10);
		break;
	case SB_PAGEUP: //单击滑块上方空白区域
		scrollinfo.nPos -= 5;
		if (scrollinfo.nPos<scrollinfo.nMin)
		{
			scrollinfo.nPos = scrollinfo.nMin;
			break;
		}
		SetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);
		ScrollWindow(0, 10 * 5);
		break;
	case SB_PAGEDOWN: //单击滑块下方空白区域
		scrollinfo.nPos += 5;
		if (scrollinfo.nPos>scrollinfo.nMax)
		{
			scrollinfo.nPos = scrollinfo.nMax;
			break;
		}
		SetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);
		ScrollWindow(0, -10 * 5);
		break;
	case SB_ENDSCROLL: //鼠标离开滑块，结束滑块拖动
					   // MessageBox("SB_ENDSCROLL");
		break;
	case SB_THUMBPOSITION:
		// ScrollWindow(0,(scrollinfo.nPos-nPos)*10);
		// scrollinfo.nPos = nPos;
		// SetScrollInfo(SB_VERT,&scrollinfo,SIF_ALL);
		break;
	case SB_THUMBTRACK: //拖动滑块
		ScrollWindow(0, (scrollinfo.nPos - nPos) * 10);
		scrollinfo.nPos = nPos;
		SetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);
		break;
	}
	CBCGPDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}


void CPageDefectSet::OnEnChangeEdtBordererosionlength()
{
	FreshParam();
}


void CPageDefectSet::OnEnChangeEdtScalefactor()
{
	FreshParam();
}


void CPageDefectSet::OnEnChangeEdtMidlightcoff()
{
	FreshParam();
}


void CPageDefectSet::OnEnChangeEdtMiddarkcoff()
{
	FreshParam();
}


void CPageDefectSet::OnEnChangeEdtBorderlightcoff()
{
	FreshParam();
}


void CPageDefectSet::OnEnChangeEdtBorderdarkcoff()
{
	FreshParam();
}


void CPageDefectSet::OnBnClickedChkUseleakfoildetect()
{
	FreshParam();
}


void CPageDefectSet::OnEnChangeEdtLeakfoilthresh()
{
	FreshParam();
}


void CPageDefectSet::OnEnChangeEdtLeakfoilminarea()
{
	FreshParam();
}


void CPageDefectSet::OnEnChangeEdtLeakfoildynthresh()
{
	FreshParam();
}


void CPageDefectSet::OnEnChangeEdtLeakfoilmindiam()
{
	FreshParam();
}


void CPageDefectSet::OnEnChangeEdtLeakfoilmindeviation()
{
	FreshParam();
}


void CPageDefectSet::OnEnChangeEdtLeakfoilmaxdiam()
{
	FreshParam();
}


void CPageDefectSet::OnEnChangeEdtAt9lowthresh()
{
	FreshParam();
}


void CPageDefectSet::OnEnChangeEdtAt9minwidth()
{
	FreshParam();
}


void CPageDefectSet::OnEnChangeEdtAt9minheight()
{
	FreshParam();
}


void CPageDefectSet::OnBnClickedChkUsebubbledetect()
{
	FreshParam();
}


void CPageDefectSet::OnEnChangeEdtBubbledynthresh()
{
	FreshParam();
}


void CPageDefectSet::OnEnChangeEdtBubblemindiam()
{
	FreshParam();
}


void CPageDefectSet::OnEnChangeEdtBubblemincircularity()
{
	FreshParam();
}


void CPageDefectSet::OnEnChangeEdtBubbleinoutdiff()
{
	FreshParam();
}


void CPageDefectSet::OnBnClickedChkUselightlinedetect()
{
	FreshParam();
}


void CPageDefectSet::OnEnChangeEdtLightlinedynthresh()
{
	FreshParam();
}


void CPageDefectSet::OnEnChangeEdtLightlineminwidth()
{
	FreshParam();
}


void CPageDefectSet::OnEnChangeEdtLightlineminheight()
{
	FreshParam();
}


void CPageDefectSet::OnEnChangeEdtLightlineminwhratio()
{
	FreshParam();
}


void CPageDefectSet::OnBnClickedChkUsedarklinedetect()
{
	FreshParam();
}


void CPageDefectSet::OnEnChangeEdtDarklinedynthresh()
{
	FreshParam();
}


void CPageDefectSet::OnEnChangeEdtDarklineminwidth()
{
	FreshParam();
}


void CPageDefectSet::OnEnChangeEdtDarklineminheight()
{
	FreshParam();
}


void CPageDefectSet::OnEnChangeEdtDarklineminwhratio()
{
	FreshParam();
}


void CPageDefectSet::OnEnChangeEdtLightdotminthresh()
{
	FreshParam();
}


void CPageDefectSet::OnEnChangeEdtLightdotmaxthresh()
{
	FreshParam();
}


void CPageDefectSet::OnEnChangeEdtLightdotminwidth()
{
	FreshParam();
}


void CPageDefectSet::OnEnChangeEdtLightdotminheight()
{
	FreshParam();
}


void CPageDefectSet::OnEnChangeEdtLightdotminarea()
{
	FreshParam();
}


void CPageDefectSet::OnEnChangeEdtLightdotmaxarea()
{
	FreshParam();
}


void CPageDefectSet::OnEnChangeEdtLightdotdeviation()
{
	FreshParam();
}


void CPageDefectSet::OnEnChangeEdtLightdotgrayrange()
{
	FreshParam();
}


void CPageDefectSet::OnBnClickedChkUsedarkdotdetect()
{
	FreshParam();
}


void CPageDefectSet::OnEnChangeEdtDarkdotmaxthresh()
{
	FreshParam();
}


void CPageDefectSet::OnEnChangeEdtDarkdotminwidth()
{
	FreshParam();
}


void CPageDefectSet::OnEnChangeEdtDarkdotminheight()
{
	FreshParam();
}


void CPageDefectSet::OnEnChangeEdtDarkdotminarea()
{
	FreshParam();
}


void CPageDefectSet::OnEnChangeEdtDarkdotmaxarea()
{
	FreshParam();
}


void CPageDefectSet::OnBnClickedChkUsedropdetect()
{
	FreshParam();
}


void CPageDefectSet::OnEnChangeEdtTapemaxthresh()
{
	FreshParam();
}


void CPageDefectSet::OnEnChangeEdtTapeminarea()
{
	FreshParam();
}


void CPageDefectSet::OnEnChangeEdtLthresh()
{
	FreshParam();
}


void CPageDefectSet::OnEnChangeEdtHthresh()
{
	FreshParam();
}


void CPageDefectSet::OnEnChangeEdtMinlighttapearea()
{
	FreshParam();
}


void CPageDefectSet::OnEnChangeEdtAt9maxthresh()
{
	FreshParam();
}


void CPageDefectSet::OnEnChangeEdtAt9darkminwidth()
{
	FreshParam();
}


void CPageDefectSet::OnEnChangeEdtAt9minarea()
{
	FreshParam();
}


void CPageDefectSet::OnBnClickedChkUsetapedetect()
{
	FreshParam();
}


void CPageDefectSet::OnEnChangeEdtJdDarkthresh()
{
	FreshParam();
}


void CPageDefectSet::OnEnChangeEdtJdMindarkarea()
{
	FreshParam();
}


void CPageDefectSet::OnEnChangeEdtJdLightthresh()
{
	FreshParam();
}


void CPageDefectSet::OnEnChangeEdtJdMinlightarea()
{
	FreshParam();
}


void CPageDefectSet::OnBnClickedChkUsefoildetect()
{
	FreshParam();
}


void CPageDefectSet::OnEnChangeEdtFoillightthresh()
{
	FreshParam();
}


void CPageDefectSet::OnEnChangeEdtFoillightminarea()
{
	FreshParam();
}


void CPageDefectSet::OnEnChangeEdtFoillightwidth()
{
	FreshParam();
}


void CPageDefectSet::OnEnChangeEdtFoildarkthresh()
{
	FreshParam();
}


void CPageDefectSet::OnEnChangeEdtFoildarkminwidth()
{
	FreshParam();
}


void CPageDefectSet::OnEnChangeEdtFoildarkminheight()
{
	FreshParam();
}


void CPageDefectSet::OnEnChangeEdtFoildarkminarea()
{
	FreshParam();
}


void CPageDefectSet::OnEnChangeEdtFoildarkminnum()
{
	FreshParam();
}


void CPageDefectSet::OnBnClickedButtonFeature()
{
	m_Grid_Feature.EndEditItem();
	if (0 != m_MenuBtn_FeatureSelect.m_nMenuResult)
	{
		AddFeature((EM_Feature)(m_MenuBtn_FeatureSelect.m_nMenuResult - 1));
	}
}

void CPageDefectSet::DeleteFeature()
{
	CList<CBCGPGridItem*, CBCGPGridItem*> sel;
	m_Grid_Feature.GetSelectedItems(sel);
	int nCount = sel.GetCount();
	if (0 >= nCount)
	{
		return;
	}

	m_Grid_Feature.RemoveSelectedRows();
}

void CPageDefectSet::OnBnClickedBtnAddalgrithm()
{
	UpdateData(TRUE);
	if (0 == m_Grid_Feature.GetRowCount())
	{
		return;
	}

	CString strName, strValve;
	GetDlgItem(IDC_COM_ERRORNAME)->GetWindowTextA(strName);
	CBCGPProp *pGoup0 = new CBCGPProp(_T(strName));
	for (int k = 0; k<m_Grid_Feature.GetRowCount(); k++)
	{
		CBCGPGridRow* pRow = m_Grid_Feature.GetRow(k);
		strName = (CString)pRow->GetItem(0)->GetValue();
		pGoup0->AddSubItem(new CBCGPProp(_T("特征"), (_variant_t)strName, _T("0-1")));
		strValve = (CString)pRow->GetItem(1)->GetValue();
		pGoup0->AddSubItem(new CBCGPProp(_T("最小值"), (_variant_t)atof(strValve), _T("0-1")));
		strValve = (CString)pRow->GetItem(2)->GetValue();
		pGoup0->AddSubItem(new CBCGPProp(_T("最大值"), (_variant_t)atof(strValve), _T("0-1")));
	}
	m_PropList_Defect.AddProperty(pGoup0);
}


void CPageDefectSet::OnBnClickedBtnDelete()
{
	DeleteFeature();
}


void CPageDefectSet::OnBnClickedBtnDeletealgrithm()
{
	if (m_PropList_Defect.GetPropertyCount()>0)
	{
		CBCGPProp* pProperty = m_PropList_Defect.GetCurSel();
		if (pProperty!=NULL && pProperty->IsGroup())
		{
			m_PropList_Defect.DeleteProperty(pProperty);
		}
	}
}


void CPageDefectSet::OnBnClickedBtnLoadparam()
{
	CString strValue = "";
	CString strName = "";
	CString strDiscription = "";
	try
	{
		CDefect::FeatureSelect tFeatureSelect;
		tFeatureSelect.nGroupNum = m_PropList_Defect.GetPropertyCount();
		for (int i = 0; i<m_PropList_Defect.GetPropertyCount(); i++)
		{
			CBCGPProp* pGroup = m_PropList_Defect.GetProperty(i);
			strDiscription = pGroup->GetDescription();
			strValue = (CString)pGroup->GetValue();
			strName = (CString)pGroup->GetName();

			CDefect::SingleErrorGroup tSingleErrorFeature;
			tSingleErrorFeature.nSingleGroupNum = pGroup->GetSubItemsCount();
			tSingleErrorFeature.strErrorName = (CString)pGroup->GetName();

			for (int j = 0; j<pGroup->GetSubItemsCount(); j = j + 3)
			{
				CDefect::SingleFeature tSingleFeature;
				tSingleFeature.StrSingleFeatureName = (CString)pGroup->GetSubItem(j)->GetValue();//参数值
				tSingleFeature.dMin = atof((CString)pGroup->GetSubItem(j + 1)->GetValue());//参数值最小值MIN
				tSingleFeature.dMax = atof((CString)pGroup->GetSubItem(j + 2)->GetValue());//参数值最大值MAX
				tSingleErrorFeature.Vec_tSingleGroupFeatures.push_back(tSingleFeature);
			}

			tFeatureSelect.Vec_FSelects.push_back(tSingleErrorFeature);
		}
		m_pParentDlg->m_pTemp->m_Defect.m_tParameters.tFeatureSelect = tFeatureSelect;
	}
	catch (HException e)
	{
		CString strMessageBox;
		strMessageBox.Format("%s赋值出错 :%s", strName, e.ErrorMessage().Text());
		BCGPMessageBox(strMessageBox);
	}
}


BOOL CPageDefectSet::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (zDelta < 0)
	{
		OnVScroll(SB_LINEDOWN, GetScrollPos(SB_VERT), GetScrollBarCtrl(SB_VERT));
	}
	else if (zDelta > 0)
	{
		OnVScroll(SB_LINEUP, GetScrollPos(SB_VERT), GetScrollBarCtrl(SB_VERT));
	}
	return CBCGPDialog::OnMouseWheel(nFlags, zDelta, pt);
}


void CPageDefectSet::OnEnChangeEdtZoomfactor()
{
	FreshParam();
}


void CPageDefectSet::OnEnChangeEdtDefectMaxnum()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CBCGPDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	FreshParam();
}
