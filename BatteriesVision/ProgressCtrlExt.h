#if !defined(AFX_GRADIENTPROGRESSCTRL_H__91ECB8BC_A7DA_42EB_B2C5_37419D31B753__INCLUDED_)
#define AFX_GRADIENTPROGRESSCTRL_H__91ECB8BC_A7DA_42EB_B2C5_37419D31B753__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ProgressCtrlExt.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CProgressCtrlExt window

class CProgressCtrlExt : public CProgressCtrl
{
// Construction
public:
	CProgressCtrlExt();

	//Set_function
	void SetTextColor(COLORREF color)	{m_clrText=color;}
	void SetBkColor(COLORREF color)		{m_clrBkGround=color;}
	void SetBorderColor(COLORREF color)	{m_clrBorder=color;}
	void SetStartColor(COLORREF color)	{m_clrStart=color;}
	void SetEndColor(COLORREF color)	{m_clrEnd=color;}
	void SetColor(COLORREF clBK, COLORREF clBorder, COLORREF clStart, COLORREF clEnd=RGB(255,255,255), COLORREF clText=RGB(255,255,255), BOOL bUseGradient=FALSE){
		m_clrBkGround=clBK;m_clrBorder=clBorder;m_clrText=clText;m_clrStart=clStart;m_clrEnd=clEnd;m_bUseGradient=bUseGradient;
	}
	void SetGradientColor(COLORREF clStart, COLORREF clEnd, BOOL bUseGradient=TRUE){m_clrStart=clStart;m_clrEnd=clEnd;m_bUseGradient=bUseGradient;}
	//Percent TXT showed in the Progress 
	void ShowPercent(BOOL bShowPercent=TRUE)	{m_bShowPercent=bShowPercent;}
	void SetCalPercentAuto(BOOL bCalPercent=TRUE)	{m_bCalPercentAuto=bCalPercent;}
	void SetBorder(BOOL bUseBorder=TRUE)	{m_bUseBorder=bUseBorder;}

	void SetPercent(const double& dPercent) {m_strPercentShow.Format("%0.0f",dPercent);m_strPercentShow+="%";}

	//Get_Function
	COLORREF GetTextColor(void)		{return m_clrText;}
	COLORREF GetBkColor(void)		{return m_clrBkGround;}
	COLORREF GetBorderColor(void)	{return m_clrBorder;}
	COLORREF GetStartColor(void)	{return m_clrStart;}
	COLORREF GetEndColor(void)		{return m_clrEnd;}


// Attributes
public:
	int			m_nLower,m_nUpper,m_nStep,m_nCurrentPosition;
	COLORREF	m_clrStart,m_clrEnd,m_clrBkGround,m_clrBorder,m_clrText;
	BOOL		m_bShowPercent;
	BOOL		m_bCalPercentAuto;
	BOOL		m_bUseGradient;
	BOOL		m_bUseBorder;
	CString		m_strPercentShow;

	CFont		m_ftPercent;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProgressCtrlExt)
	//}}AFX_VIRTUAL

// Implementation
public:
	int SetPos(int nPos);
	int	 SetStep(int nStep);
	void SetRange(int nLower,int nUpper);
	virtual ~CProgressCtrlExt();

	// Generated message map functions
protected:
	void DrawGradient(CPaintDC *pDC,const RECT& rectClient,const int & nMaxWidth);
	void DrawColor(CPaintDC *pDC,const RECT& rectClient,const int & nMaxWidth);
	//{{AFX_MSG(CProgressCtrlExt)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnNcPaint();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GRADIENTPROGRESSCTRL_H__91ECB8BC_A7DA_42EB_B2C5_37419D31B753__INCLUDED_)
