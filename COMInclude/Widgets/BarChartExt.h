// BarChart.h : header file


#if !defined(AFX_BARCHART_H__24B6B328_2855_4810_8F81_DC2EA0217FBD__INCLUDED_)
#define AFX_BARCHART_H__24B6B328_2855_4810_8F81_DC2EA0217FBD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <sql.h>
#include <sqlext.h>

#ifndef _AFX_NOFORCE_LIBS

#pragma comment(lib, "odbc32.lib")
#pragma comment(lib, "odbccp32.lib")

#endif //!_AFX_NOFORCE_LIBS

 ///////////////////////////////////////////////////////////////////////
 // Some definitions for more customization
 //
 //		The following definitions can be used to customize the chart.
 //		Many values and variables in chart considered to have a default
 //		Value to make it easier to use the chart.
 //		But in some situations you might decide to customize them.
 //
 ///////////////////////////////////////////////////////////////////////

#define CHART_TITLE_HEIGHT		30	// This reserves enough room for the title at the bottom of the chart. If you don't need a title, set it to zero

#define GRID_MARGIN_WIDTH		55	// Distance between Top & Right side of the control and the Grid box
#define GRID_MARGIN_HEIGHT		30
#define GRID_DEFAULT_SIZE		10	// WARNING: Should not set to 0 - This is the size of the grid rectangles

#define BAR_DEFAULT_HEIGHT		100	// Default height for each bar. Almost neutral since it will be replaced by the height of the control
#define BAR_DEFAULT_WIDTH		24	// Default width of each bar, increase it to make bars fat 
#define BAR_DEFAULT_GAP			4	// Distance between bars 
#define BAR_REFLECT_AMOUNT		32	// Decrease to make WHITE shines more

#define BAR_TXT_PERCENT_HEIGHT	20	// Room for a Value/Percent at the top of each bar
#define BAR_TXT_LABEL_HEIGHT	20	// Room for the label at the buttom of each bar
#define BAR_TXT_VLABEL_MARGIN	30	// The right margin, room for a value at the right side and in front of each bar in % mode

#define BAR_TXT_LABEL_COLOR		RGB(50, 50, 50)

#define TIP_BK_COLOR			RGB(0, 118, 251)	// Background color for tooltip 255, 255, 220
#define TIP_FRAME_COLOR			RGB(0, 118, 251)	// Frame color for tooltip 240, 210, 80
#define TIP_TITLE_COLOR			RGB(255, 255, 255)	// Color of the tip's top most text 255, 100, 0
#define TIP_TEXT_COLOR			RGB(255, 255, 255)	// Color of the tip's middle text 10, 100, 10
#define TIP_PERCENT_COLOR		RGB(200, 200, 200)	// Color of the tip's bottom most text 150, 150, 150

#define TIP_LEFT_MARGIN			15					// Left margin for text in the tip
#define TIP_TOP_MARGIN			10					// Top margin for text in the tip

#define TIP_FRAME_THICKNESS		1					// Thickness of the frame line around the tooltip ,2
#define TIP_TEXT_MAX_WIDTH		210					// Maximum width of the text within the tip
#define TIP_TEXT_MAX_HEIGHT		26					// Maximum height of the text within the tip 26
#define TIP_RGN_ROUND_CORNERS	3					// Increase to round tip corners more ,10
#define TIP_SHADOW_SIZE			4					// NOT USED YET
#define TIP_SHADOW_COLOR		RGB(155, 155, 155)	// NOT USED YET
#define TIP_TOP_OFFSET			20
#define TIP_RIGHT_OFFSET		10

#define TIP_DELAY_TIMER			1	// Don't change this, please. It does not have any influence.
#define CHART_BORDER_DARKNESS	100 // Don't change this, please.
#define GRID_FRAME_HEIGHT		2	// Don't change this, please.


/////////////////////////////////////////////////////////////////////////////

 ///////////////////////////////////////////////////////////////////////
 // CChartGCalc Class
 //
 //
 //			A set of handy GDI functions for manipulating
 //			pixel colors, etc.
 //
 ///////////////////////////////////////////////////////////////////////
class CChartGCalc
{
protected:
	UINT GABS (const int& nmbr);
	COLORREF DifColor(COLORREF color, UINT nDist);
	COLORREF OrColor(COLORREF colFore, COLORREF colBK, UINT nForePercent);

	// Refer to the following article in 'CodeProject.com' By 'Roger Allen'
	// 'Printing tips and tricks from the trenches' 
	// And goto topic: 'Use DIB's instead of DDB's'
	HANDLE DDBToDIB( CBitmap* bitmap, DWORD dwCompression, CPalette* pPal );
}; // end of GDICalc


 ///////////////////////////////////////////////////////////////////////
 // CChartTip Class
 //
 //
 //				Adds tooltip capabilities for bar chart.
 //				Tooltip should show: value, Label and percentage
 //
 ///////////////////////////////////////////////////////////////////////
class W_LIB CChartTipExt : public CWnd, CChartGCalc
{
// Construction
public:
	CChartTipExt();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChartTip)
	//}}AFX_VIRTUAL

// Implementation
public:
	void Hide();
	void Pop();
	BOOL Create(CWnd* pParentWnd, BOOL* pbEnable, CString* lpszTitle, int nDelay);
	void SetTipColor(COLORREF clBK, COLORREF clBoder, COLORREF clTitle, COLORREF clText, COLORREF clText2);
	void SetTipSize(int nWidth, int nHeight, int nTextHeightMax);
	void SetTitleFont(CString strlfFaceName, int nlfHeight, int nlfWeight);
	void SetTextFont(CString strlfFaceName, int nlfHeight, int nlfWeight);
	void SetTipText(CString* lpszTitle);
	virtual ~CChartTipExt();

	// Generated message map functions
protected:
	static LPCTSTR		m_lpszClass;
	BOOL		*m_pbEnable,
				m_bTimer;
	CString		*m_lpszTitle,
				m_szText,
				m_szOldTitle;
	CString		m_strTitleFontFaceName,
				m_strTextFontFaceName;
	CPoint		m_pointOld;
	int			m_nDelay;
	CRect		m_rcClient;
	CBrush		m_brFrame,
				m_brBK,
				m_brShadow;
	CFont		m_fontBold,
				m_fontNorm;

	CRgn		m_rgnTip, m_rgnCopy, m_rgnShadow;

	COLORREF	m_clBK, m_clBD, m_clTitle, m_clText, m_clText2;
	int			m_nWidth, m_nHeight;
	int			m_nTextFontHeight, m_nTextFontWeight, m_nTitleFontHeight, m_nTitleFontWeight;
	int			m_nTextHeightMax;

	//{{AFX_MSG(CChartTip)
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void HideWindow();
};

 ///////////////////////////////////////////////////////////////////////
 // CBarChart Class
 //
 //				Represents a chart control. The chart shows some bars
 //				Each bar has a value. Bars heights will be calculated
 //				Regarding other bars. The bars are able to show a percentage
 //				Instead of a value. The bar with maximum value has 100% and
 //				Percentage of other bars will be calculated in accordance to
 //				This maximum value.
 //
 ///////////////////////////////////////////////////////////////////////
class W_LIB CBarChartExt : public CWnd, CChartGCalc
{
public:
	enum emBarTextType
	{
		TEXT_PERCENT,	//显示所占总数的比重
		TEXT_DOUBLE,	//显示小数
		TEXT_INT		//显示整数
	};
// Construction
public:
	CBarChartExt();

// Operations
protected:

	/*----------------------------------------------------------*
	 | CChartData Class											|
	 |															|
	 |		Holds data of a chart and let the chart to do		|
	 |		reqauered operations on it							|
	 |															|
	 *----------------------------------------------------------*/
	class CChartData
	{
		struct data
		{
			CString m_szLabel;
			double m_udValue;
			COLORREF m_color;
		};

		CObArray m_arr;
		double m_udMax;
		double m_udSum;

	protected:
		void CalculateMaximum();

	public:
		CChartData();
		void RemoveAll();
		void RemoveAt(int nIndex);
		int GetCount();
		void Add (CString& szLabel, double udValue, COLORREF color);
		void SetValue(int &nIndex, double &udValue);
		
		int GetBarHeightUpperBound();
		double GetMaxValue();
		double GetSumValue();
		double GetValue(int nIndex);
		CString GetLabel(int nIndex);
		COLORREF GetColor(int nIndex);
	};	// end of CChartData

	/*-----------------------------------------------------------*
	 | CGDIGrid Class											 |
	 |															 |
	 |		Draws a grid behind the chart.						 |
	 |		UNDONE: The grid class should act better. instead of |
	 |		drawing horizontal lines from top, it should start	 |
	 |		from the line under the bars. (Logical y = 0 axe)	 |
	 |															 |
	 *-----------------------------------------------------------*/
	class CGDIGrid
	{
		// Attributes
		UINT	m_nVLineCount,
				m_nHLineCount,
				m_nFixedSize;
		int		i,
				m_nHeightUpperBound;

		CPen	m_penGrid, 
				*m_pPenOld;

		CFont	m_fontGraduation;

		BOOL	m_bShowGraduation;//是否显示刻度

	public:
		// Construction
		CGDIGrid();
		~CGDIGrid();

		void SetLineCount(UINT nVLineCnt, UINT nHLineCnt, BOOL bFixedSize = FALSE, UINT nFixedSize = 0);
		void SetHeightUpperBound(int &nHeight);
		void SetShowGraduation(BOOL bShow);
		void Draw(CDC* pDC, CRect& rcBound);
		void SetColor(COLORREF color);
	}; // end of CGDIGrid

	/*-----------------------------------------------------------*
	 | CChartBar Class											 |
	 |															 |
	 |		This class is responsible for for drawing bars and   |
	 |		performing required calculation for the job.         |
	 |															 |
	 *-----------------------------------------------------------*/
	class CChartBar : public CChartGCalc
	{
		CChartData
				m_chartData;

		CSize	m_sBarPicSize;
		
		BOOL	m_bShowLabel,
				m_bShowBarText;

		int		m_nBarTextType;
		int		m_nBarWidth, m_nBarGap;
		double	m_dAlpha, m_dIndex;
		
		COLORREF m_colorChartBK;

		CFont	m_font, *m_pFontOld, m_fontLable;
		LOGFONT m_lf;

		double m_rMargin;

		void DrawBars(CDC* pDC, CRect& rcBound);
		void CreateLabelFont();

	public:
		BOOL IsLabelVisible();

		void DisplayToolTip(CPoint& point, CRect& rcBound, 
			CRect& rcMemPic, BOOL bScale, CString& szTip);
		
		CChartBar();
		~CChartBar();

		void SetBkColor(COLORREF color);
		int GetBarHeight();
		int GetBarWidth();
		int GetBarHeightUpperBound();

		void SetBarSize(int &nWidth,int &nGap);
		void SetBarValue(int &nIndex, double &dValue);

		void RemoveAll();
		void RemoveAt(int nIndex);
		void AddBar(double udValue,CString szLabel, COLORREF color);

		void Draw(CDC* pDC, CRect& rcBound);
		void ShowLabel(BOOL bShow);
		void ShowBarText(int nType = 0 /* 0 or 1, Default: 0=Percent, 1=value*/, BOOL bShow = TRUE/*Default:Show*/);
	};

	 /*----------------------------------------------------------*
	 | CChartPrinter Class										 |
	 |															 |
	 |		No need to description(!) This class should print the|
	 |		Chart using print dialog.							 |
	 |															 |
	 *----------------------------------------------------------*/
	class CChartPrinter : public CChartGCalc
	{
	protected:
		CDC		*m_pMemDC;
		CRect	m_rcMemPic;
		CBitmap *m_pBmpMem;

		BOOL	m_bScale;

		void OnPrint (CDC *pDC, CPrintInfo* pInfo);
		void OnPrintCompatible (CDC *pDC, CPrintInfo* pInfo);

		//HANDLE DDBToDIB( CBitmap* bitmap, DWORD dwCompression, CPalette* pPal );
	public:
		void Print(CDC* pDC, CDC *pMemDC, CBitmap* pBmp, 
			CString szTitle, CRect& rcMemPic, BOOL bFit, BOOL bCompatible);
	};

	 /*----------------------------------------------------------*
	 | CChartDatabase Class										 |
	 |															 |
	 |		This class provides the ability to connect to an	 |
	 |		ODBC database and read chart data from a table or an |
	 |		Stored procedure.									 |
	 |															 |
	 *----------------------------------------------------------*/
	class CChartDatabase  
	{
	protected:
		BOOL ExecuteSQL(CString& szQuery);
		void Close();
		BOOL ExecuteProc(CString szTable, CString szFilter, CString szLabelFieldName, CString szValueFieldName);
		BOOL ExecuteQuery(CString szTable, CString szFilter, CString szLabelFieldName, CString szValueFieldName);
		BOOL Connect(CString szDSN, CString szUsername, CString szPass);
		BOOL Prepare(ULONG ulODBCVer);

		SQLRETURN	m_sqlRet;				// To store resaults
		SQLHANDLE	m_hOdbcEnv;				// ODBC Environment handle
		SQLHANDLE	m_hDbConn;				// ODBC Connection handle
		SQLHSTMT	m_hstmt;				// ODBC Statement handle

		BOOL		m_bEOF;					// Set to true when reached the end of database
		CString		m_szError;

		struct scDBRow
		{
			SQLCHAR szLabel[51];
			double	dValue;
			SQLINTEGER nLabelLen;
			SQLINTEGER nValueLen;

		} m_row;

	public:
		CString GetLastErrorMessage();
		void GetRow(CString& szLabel, double& dValue);
		BOOL MoveNext();
		BOOL IsEOF();
		CChartDatabase();
		virtual ~CChartDatabase();

		BOOL OpenTable(CString szDSN, CString szTable,
				CString szLabelFieldName, CString szValueFieldName,
				CString szFilter = "", 
				CString szUsername = "", CString szPass = "", 
				BOOL bStoredProc = FALSE,
				ULONG ulODBCVer = SQL_OV_ODBC3);
		
		BOOL OpenProc(CString szDSN, CString szStoredProc,
				CString szParameterList = "",
				CString szUsername = "", CString szPass = "",
				ULONG ulODBCVer = SQL_OV_ODBC3);

	}; // End of chart database

// Attributes
public:
	
protected:
	COLORREF m_colorBK;
	CRect	m_rctClient;
	CBrush	m_brFrame;
	CGDIGrid m_grid;

	BOOL	m_bShowGrid,
			m_bAutoScale,
			m_bShowTip;
	
	CDC		m_dcMem;

	CChartBar
			m_bars;

	CBitmap *m_pBmpBars,
			*m_pBmpOld;

	CRect	m_rctMemPic;

	CChartPrinter
		m_printer;
	CFont	m_fontTitle;

	CString m_szTitle,
			m_szTip;

	CChartTipExt
		m_toolTip;

	CString m_szLastErr;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBarChart)
	//}}AFX_VIRTUAL

// Implementation
protected:

	BOOL PromptForFile(CString& szPath, CString szFilterName, CString szFilter);
	void ResetBitmap(CDC* pDC);
	void DrawBars(CDC* pDC, CRect& rcBound);
	void RedrawChart(CDC* pDC);
	void DrawTitle(CDC* pDC);
	void InitToolTips();
	int	 GetBarHeightUpperBound();

public:
	void RedrawAll();

	void Print(BOOL bCompatible = FALSE);
	void Reset();
	void RemoveAll();
	void RemoveAt(int nIndex);

	void SetAutoScale(BOOL bAutoScale);
	
	void SetTitle(CString szNewTitle);

	void ShowTooltip(BOOL bShow);
	void ShowLabel(BOOL bShow, BOOL bRedraw = FALSE);
	void ShowGrid(BOOL bShow, BOOL bShowGraduation = TRUE,BOOL bRedraw = FALSE);
	void ShowBarText(int nType = 0 /* 0 or 1, Default: 0=Percent, 1=value*/, BOOL bShow = TRUE/*Default:Show*/, BOOL bRedraw = FALSE);
	

	void AddBar(double udValue,CString szLabel, COLORREF color, BOOL bRepaint=FALSE);
	void SetBarValue(int nIndex,double udValue);//修改柱状图值

	void SetBKColor(COLORREF colorBK);

	void SetBarSize(int nWidth,int nGap);
	
	void Refresh();

	BOOL ReadFromDatabase(CString szDSN, CString szTable, CString szFilter,
						  CString szLabelFieldName, CString szValueFieldName,
						  COLORREF colorBars = RGB(0, 0, 0),
						  CString szUsername = "", CString szPass = "");
	
	BOOL ReadFromDatabase(CString szDSN, CString szSPName, CString szParameterList = "",
					COLORREF colorBars = RGB(0, 0, 0),
					CString szUsername = "", CString szPass = "");

	CString GetLastDatabaseErrMessage();
	CString GetLastErrorMessage();

	BOOL SaveToFile(CString szPath = "");

	void SetGridLines(int nHorLineCount, int nVerLineCount, BOOL bFixedSize = FALSE, int nFixedSize = 0);

	BOOL Create(const RECT& rect, CWnd* pParentWnd, UINT nControlID, CString szChartTitle="");
	virtual ~CBarChartExt();

	// Generated message map functions
protected:
	//{{AFX_MSG(CBarChart)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BARCHART_H__24B6B328_2855_4810_8F81_DC2EA0217FBD__INCLUDED_)
