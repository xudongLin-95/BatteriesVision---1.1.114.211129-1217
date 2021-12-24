#pragma once
#include "resource.h"
#include "afxwin.h"
#include "Defect.h"

// CPageDefectSet 对话框
class CDlgParam;
class CPageDefectSet : public CBCGPDialog
{
	DECLARE_DYNAMIC(CPageDefectSet)

public:
	CPageDefectSet(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPageDefectSet();

// 对话框数据
//#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_DEFECT_SET };
//#endif

	CMeasure* m_pOwners;
	CDlgParam* m_pParentDlg;

private:
	void InitUI();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	void AddFeature(EM_Feature eFeature);
	void DeleteFeature();
	void FreshParam();//更新参数
	virtual BOOL OnInitDialog();
	virtual void PostNcDestroy();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	BOOL m_CHK_bUseBubbleDetect;
	BOOL m_CHK_bUseDarkDotDetect;
	BOOL m_CHK_bUseDarkLineDetect;
	BOOL m_CHK_bUseFoilDetect;
	BOOL m_CHK_bUseLeakFoilDetect;
	BOOL m_CHK_bUseLightDotDetect;
	BOOL m_CHK_bUseLightLineDetect;
	BOOL m_CHK_bUseDropDetect;
	BOOL m_CHK_bUseTapeDetect;
	double m_EDT_dAT9DarkMinWidth;
	int m_EDT_nAT9MinThresh;
	int m_EDT_nAT9MaxThresh;
	double m_EDT_dAT9DarkMinArea;
	double m_EDT_dAT9MinHeight;
	double m_EDT_dAT9MinWidth;
	double m_EDT_dBorderDarkCoff;
	double m_EDT_dBorderErosionLength;
	double m_EDT_dBorderLightCoff;
	int m_EDT_nBubbleDynThresh;
	int m_EDT_nBubbleInOutDiff;
	double m_EDT_dBubbleMinCircularity;
	double m_EDT_dBubbleMinDiameter;
	double m_EDT_dDarkDotMaxArea;
	int m_EDT_nDarkDotMaxThresh;
	int m_EDT_dDarkDotMinArea;
	double m_EDT_dDarkDotMinHeight;
	double m_EDT_dDarkDotMinWidth;
	double m_EDT_nDarkLineDynThresh;
	double m_EDT_dDarkLineMinHeight;
	double m_EDT_dDarkLineMinRatio;
	double m_EDT_dDarkLineMinWidth;
	double m_EDT_dFoilDarkMinArea;
	double m_EDT_dFoilDarkMinHeight;
	int m_EDT_nFoilDarkMinNum;
	double m_EDT_dFoilDarkMinWidth;
	int m_EDT_nFoilDarkThresh;
	double m_EDT_dFoilLightMinArea;
	int m_EDT_nFoilLightThresh;
	double m_EDT_dFoilLightMinWidth;
	int m_EDT_nLTMaxThresh;
	int m_EDT_nJDDarkThresh;
	int m_EDT_nJDLightThresh;
	double m_EDT_dJDDarkArea;
	double m_EDT_dJDLightArea;
	int m_EDT_nLeakFoilDynThresh;
	double m_EDT_dLeakFoilMaxDiameter;
	double m_EDT_dLeakFoilMinArea;
	double m_EDT_dLeakFoilMinDeviation;
	double m_EDT_dLeakFoilMinDiameter;
	int m_EDT_nLeakFoilThresh;
	int m_EDT_nLightDotGrayDeviation;
	int m_EDT_nLightDotGrayRange;
	double m_EDT_dLightDotMaxArea;
	int m_EDT_nLightDotMaxThresh;
	double m_EDT_dLightDotMinArea;
	double m_EDT_dLightDotMinHeight;
	int m_EDT_nLightDotMinThresh;
	double m_EDT_dLightDotMinWidth;
	int m_EDT_nLightLineDynThresh;
	double m_EDT_dLightLineMinHeight;
	double m_EDT_dLightLineMinRatio;
	double m_EDT_dLightLineMinWidth;
	int m_EDT_nLTMinThresh;
	double m_EDT_dMidDarkCoff;
	double m_EDT_dMidLightCoff;
	double m_EDT_dLTMinArea;
	double m_EDT_dScaleFactor;
	int m_EDT_nTapeMaxThresh;
	double m_EDT_dTapeMinArea;
	afx_msg void OnEnChangeEdtBordererosionlength();
	afx_msg void OnEnChangeEdtScalefactor();
	afx_msg void OnEnChangeEdtMidlightcoff();
	afx_msg void OnEnChangeEdtMiddarkcoff();
	afx_msg void OnEnChangeEdtBorderlightcoff();
	afx_msg void OnEnChangeEdtBorderdarkcoff();
	afx_msg void OnBnClickedChkUseleakfoildetect();
	afx_msg void OnEnChangeEdtLeakfoilthresh();
	afx_msg void OnEnChangeEdtLeakfoilminarea();
	afx_msg void OnEnChangeEdtLeakfoildynthresh();
	afx_msg void OnEnChangeEdtLeakfoilmindiam();
	afx_msg void OnEnChangeEdtLeakfoilmindeviation();
	afx_msg void OnEnChangeEdtLeakfoilmaxdiam();
	afx_msg void OnEnChangeEdtAt9lowthresh();
	afx_msg void OnEnChangeEdtAt9minwidth();
	afx_msg void OnEnChangeEdtAt9minheight();
	afx_msg void OnBnClickedChkUsebubbledetect();
	afx_msg void OnEnChangeEdtBubbledynthresh();
	afx_msg void OnEnChangeEdtBubblemindiam();
	afx_msg void OnEnChangeEdtBubblemincircularity();
	afx_msg void OnEnChangeEdtBubbleinoutdiff();
	afx_msg void OnBnClickedChkUselightlinedetect();
	afx_msg void OnEnChangeEdtLightlinedynthresh();
	afx_msg void OnEnChangeEdtLightlineminwidth();
	afx_msg void OnEnChangeEdtLightlineminheight();
	afx_msg void OnEnChangeEdtLightlineminwhratio();
	afx_msg void OnBnClickedChkUsedarklinedetect();
	afx_msg void OnEnChangeEdtDarklinedynthresh();
	afx_msg void OnEnChangeEdtDarklineminwidth();
	afx_msg void OnEnChangeEdtDarklineminheight();
	afx_msg void OnEnChangeEdtDarklineminwhratio();
	afx_msg void OnEnChangeEdtLightdotminthresh();
	afx_msg void OnEnChangeEdtLightdotmaxthresh();
	afx_msg void OnEnChangeEdtLightdotminwidth();
	afx_msg void OnEnChangeEdtLightdotminheight();
	afx_msg void OnEnChangeEdtLightdotminarea();
	afx_msg void OnEnChangeEdtLightdotmaxarea();
	afx_msg void OnEnChangeEdtLightdotdeviation();
	afx_msg void OnEnChangeEdtLightdotgrayrange();
	afx_msg void OnBnClickedChkUsedarkdotdetect();
	afx_msg void OnEnChangeEdtDarkdotmaxthresh();
	afx_msg void OnEnChangeEdtDarkdotminwidth();
	afx_msg void OnEnChangeEdtDarkdotminheight();
	afx_msg void OnEnChangeEdtDarkdotminarea();
	afx_msg void OnEnChangeEdtDarkdotmaxarea();
	afx_msg void OnBnClickedChkUsedropdetect();
	afx_msg void OnEnChangeEdtTapemaxthresh();
	afx_msg void OnEnChangeEdtTapeminarea();
	afx_msg void OnEnChangeEdtLthresh();
	afx_msg void OnEnChangeEdtHthresh();
	afx_msg void OnEnChangeEdtMinlighttapearea();
	afx_msg void OnEnChangeEdtAt9maxthresh();
	afx_msg void OnEnChangeEdtAt9darkminwidth();
	afx_msg void OnEnChangeEdtAt9minarea();
	afx_msg void OnBnClickedChkUsetapedetect();
	afx_msg void OnEnChangeEdtJdDarkthresh();
	afx_msg void OnEnChangeEdtJdMindarkarea();
	afx_msg void OnEnChangeEdtJdLightthresh();
	afx_msg void OnEnChangeEdtJdMinlightarea();
	afx_msg void OnBnClickedChkUsefoildetect();
	afx_msg void OnEnChangeEdtFoillightthresh();
	afx_msg void OnEnChangeEdtFoillightminarea();
	afx_msg void OnEnChangeEdtFoillightwidth();
	afx_msg void OnEnChangeEdtFoildarkthresh();
	afx_msg void OnEnChangeEdtFoildarkminwidth();
	afx_msg void OnEnChangeEdtFoildarkminheight();
	afx_msg void OnEnChangeEdtFoildarkminarea();
	afx_msg void OnEnChangeEdtFoildarkminnum();
	CBCGPComboBox m_Combo_ErrorName;
	CBCGPMenuButton m_MenuBtn_FeatureSelect;
	CBCGPGridCtrl m_Grid_Feature;
	CBCGPPropList m_PropList_Defect;
	afx_msg void OnBnClickedButtonFeature();
	afx_msg void OnBnClickedBtnAddalgrithm();
	afx_msg void OnBnClickedBtnDelete();
	afx_msg void OnBnClickedBtnDeletealgrithm();
	afx_msg void OnBnClickedBtnLoadparam();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	double m_EDT_dZoomFactor;
	afx_msg void OnEnChangeEdtZoomfactor();
};