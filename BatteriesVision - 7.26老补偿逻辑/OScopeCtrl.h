// OScopeCtrl.h : header file 
// 

#ifndef __OScopeCtrl_H__ 
#define __OScopeCtrl_H__ 
#include <vector>
///////////////////////////////////////////////////////////////////////////// 
// COScopeCtrl window 

class COScopeCtrl : public CWnd
{
public:
	struct tagPoint//输入点
	{
		std::vector<double> vecPt;//多点
		double dCpk;
		double dSig;
		double dMean;
	};
	// Construction 
public:
	COScopeCtrl();

	// Attributes 
public:
	void AppendPoint(tagPoint tPt);
	void SetPlotName(std::vector<CString> vecName, BOOL bInvalidate = FALSE);
	void SetPlotColor(std::vector<COLORREF> vecColor, BOOL bInvalidate = FALSE);
	void SetLimit(double dStandardValue, double dNGTolerance, double dWarnTolerance, double dMarginDiff, int nDecimalPlaces = 1, BOOL bInvalidate = FALSE);
	void SetXUnits(CString string, BOOL bInvalidate = FALSE);
	void SetYUnits(CString string, BOOL bInvalidate = FALSE);
	void SetTitleText(CString string, BOOL bInvalidate = FALSE);
	void SetGridColor(COLORREF color, BOOL bInvalidate = FALSE);
	void SetLimitLineColor(COLORREF colorNG, COLORREF colorWarn, BOOL bInvalidate = FALSE);
	void SetBackgroundColor(COLORREF color, BOOL bInvalidate = FALSE);
	void SetTitleBackgroundColor(COLORREF color, BOOL bInvalidate = FALSE);
	void SetTitleTextColor(COLORREF color, BOOL bInvalidate = FALSE);
	void InvalidateCtrl();
	void DrawPoint();
	void Reset();
	void SetWarnLineShow(const BOOL& bShow) { m_bShowWarnLimitLine = bShow; }
	void SetCpkSigShow(const BOOL &bShowCpk, const BOOL &bShowSig) { m_bShowCpk = bShowCpk; m_bShowSig = bShowSig; }
	void SetCpkLimit(double dUpperNGLimit);

private:
	void DrawTitle();
	void DrawTitle(CDC &memDC);
	void DrawGridLine();
	// Operations 
public:

	// Overrides 
	  // ClassWizard generated virtual function overrides 
	  //{{AFX_VIRTUAL(COScopeCtrl) 
public:
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID = NULL);
	//}}AFX_VIRTUAL 

  // Implementation 
public:
	int m_nShiftPixels;          // amount to shift with each new point  
	int m_nYDecimals;

	CString m_strXUnitsString;
	CString m_strYUnitsString;
	CString m_strTitleString;

	COLORREF m_crBackColor;        // background color 
	COLORREF m_crTitleBackColor;   // title background color
	COLORREF m_crTitleTextColor;	 // title text color
	COLORREF m_crGridColor;        // grid color 
	COLORREF m_crGridNGColor;
	COLORREF m_crGridWarnColor;
	COLORREF m_crPointNGColor[2];
	COLORREF m_crPointWarnColor[2];

	std::vector<COLORREF> m_vecPlotColor;	//折线图颜色
	std::vector<CString> m_vecPlotName;		//折线图名字

	std::vector<tagPoint> m_vecPlotPoint;		//折线图的点0是当前点，1是上一点

	virtual ~COScopeCtrl();

	// Generated message map functions 
protected:
	//{{AFX_MSG(COScopeCtrl) 
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG 
	DECLARE_MESSAGE_MAP()

	int m_nHalfShiftPixels;
	int m_nPlotShiftPixels;
	int m_nClientHeight;
	int m_nClientWidth;
	int m_nPlotHeight;
	int m_nPlotWidth;

	BOOL m_bShowWarnLimitLine;
	BOOL m_bShowCpk;
	BOOL m_bShowSig;

	double m_dLowerWarnLimit;       // lower bounds 
	double m_dUpperWarnLimit;       // upper bounds 
	double m_dUpperNGLimit;			//客户要求的上限
	double m_dLowerNGLimit;			//客户要求的下限2017.09.01
	double m_dUpperMargin;
	double m_dLowerMargin;
	
	double m_dCpkLowerNGLimit;

	double m_dRange;
	double m_dVerticalFactor;

	CRect  m_rectClient;
	CRect  m_rectPlot;
	CRect  m_rectTitle;
	CPen   m_penPlot;
	CBrush m_brushBack;
	CBrush m_brushTitleBack;

	CDC     m_dcGrid;
	CDC     m_dcPlot;
	CBitmap *m_pbitmapOldGrid;
	CBitmap *m_pbitmapOldPlot;
	CBitmap m_bitmapGrid;
	CBitmap m_bitmapPlot;

	CPen*	m_pOldPen;
	CBrush*	m_pOldBrush;
	CFont*  m_pOldFont;

	CString m_strCpk;
public:
	afx_msg void OnNcPaint();
};

///////////////////////////////////////////////////////////////////////////// 
#endif