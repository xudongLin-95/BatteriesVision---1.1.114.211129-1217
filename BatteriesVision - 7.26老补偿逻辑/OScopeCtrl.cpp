
// OScopeCtrl.cpp : implementation file//   

#include "stdafx.h"   
#include "math.h"   

#include "OScopeCtrl.h"   

#ifdef _DEBUG   
#define new DEBUG_NEW   
#undef THIS_FILE   
static char THIS_FILE[] = __FILE__;
#endif   

/////////////////////////////////////////////////////////////////////////////   
// COScopeCtrl   
COScopeCtrl::COScopeCtrl()
{
	// since plotting is based on a LineTo for each new point   
	// we need a starting point (i.e. a "previous" point)   
	// use 0.0 as the default first point.   
	// these are public member variables, and can be changed outside   
	// (after construction).  Therefore m_perviousPosition could be set to   
	// a more appropriate value prior to the first call to SetPosition.   
	// public variable for the number of decimal places on the y axis   
	m_nYDecimals = 3;

	// set some initial values for the scaling until "SetRange" is called.   
	// these are protected varaibles and must be set with SetRange   
	// in order to ensure that m_dRange is updated accordingly   
	m_dLowerNGLimit = -10.0;
	m_dUpperNGLimit = 10.0;
	m_dLowerWarnLimit = -5.0;
	m_dUpperWarnLimit = 5.0;
	m_dLowerMargin = -20;
	m_dUpperMargin = 20;
	m_dRange = m_dUpperMargin - m_dLowerMargin;   // protected member variable   

	// m_nShiftPixels determines how much the plot shifts (in terms of pixels)    
	// with the addition of a new data point   
	m_nShiftPixels = 4;
	m_nHalfShiftPixels = m_nShiftPixels / 2;                     // protected   
	m_nPlotShiftPixels = m_nShiftPixels + m_nHalfShiftPixels;  // protected   

	// background, grid and data colors   
	// these are public variables and can be set directly   
	m_crBackColor = RGB(0, 0, 0);  // see also SetBackgroundColor
	m_crTitleBackColor = RGB(85, 85, 85);
	m_crTitleTextColor = RGB(255, 255, 255);

	m_crGridColor = RGB(125, 125, 130);  // see also SetGridColor
	m_crGridWarnColor = RGB(126, 111, 77);//156, 141, 107
	m_crGridNGColor = RGB(119, 83, 84);  // see also SetPlotColor

	m_crPointNGColor[0] = RGB(235, 33, 33);
	m_crPointNGColor[1] = RGB(255, 89, 49);
	m_crPointWarnColor[0] = RGB(248, 182, 43);
	m_crPointWarnColor[1] = RGB(228, 162, 23);

	m_bShowWarnLimitLine = FALSE;
	m_bShowCpk = FALSE;
	m_bShowSig = FALSE;

	m_dCpkLowerNGLimit = 10;

	m_vecPlotColor.push_back(RGB(0, 255, 0));
	m_vecPlotColor.push_back(RGB(255, 255, 255));
	m_vecPlotColor.push_back(RGB(242, 150, 0));
	m_vecPlotColor.push_back(RGB(204, 255, 102));
	m_vecPlotColor.push_back(RGB(204, 255, 255));
	m_vecPlotColor.push_back(RGB(204, 153, 255));
	m_vecPlotColor.push_back(RGB(255, 102, 102));
	m_vecPlotColor.push_back(RGB(0, 153, 204));
	m_vecPlotColor.push_back(RGB(153, 204, 204));
	m_vecPlotColor.push_back(RGB(255, 255, 204));
	m_vecPlotColor.push_back(RGB(217, 135, 98));
	m_vecPlotColor.push_back(RGB(255, 153, 204));
	m_vecPlotColor.push_back(RGB(120, 186, 124));
	m_vecPlotColor.push_back(RGB(204, 204, 0));
	m_vecPlotColor.push_back(RGB(204, 204, 0));
	m_vecPlotColor.push_back(RGB(152, 251, 152));
	m_vecPlotColor.push_back(RGB(100, 149, 237));

	// protected variables   
	m_penPlot.CreatePen(PS_SOLID, 0, m_vecPlotColor[0]);
	m_brushBack.CreateSolidBrush(m_crBackColor);
	m_brushTitleBack.CreateSolidBrush(m_crTitleBackColor);

	// public member variables, can be set directly
	m_strXUnitsString.Format("");  // can also be set with SetXUnits
	m_strYUnitsString.Format("");  // can also be set with SetYUnits  //2017.09.01将默认值去掉
	m_strTitleString.Format("");

	// protected bitmaps to restore the memory DC's   
	m_pbitmapOldGrid = NULL;
	m_pbitmapOldPlot = NULL;

}  // COScopeCtrl   

/////////////////////////////////////////////////////////////////////////////   
COScopeCtrl::~COScopeCtrl()
{
	// just to be picky restore the bitmaps for the two memory dc's   
	// (these dc's are being destroyed so there shouldn't be any leaks)   
	if (m_pbitmapOldGrid != NULL)
		m_dcGrid.SelectObject(m_pbitmapOldGrid);
	if (m_pbitmapOldPlot != NULL)
		m_dcPlot.SelectObject(m_pbitmapOldPlot);

} // ~COScopeCtrl   


BEGIN_MESSAGE_MAP(COScopeCtrl, CWnd)
	//{{AFX_MSG_MAP(COScopeCtrl)   
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP   
	ON_WM_NCPAINT()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////   
// COScopeCtrl message handlers   

/////////////////////////////////////////////////////////////////////////////   
BOOL COScopeCtrl::Create(DWORD dwStyle, const RECT& rect,
	CWnd* pParentWnd, UINT nID)
{
	BOOL result;
	static CString className = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW);

	result = CWnd::CreateEx(WS_EX_CLIENTEDGE | WS_EX_STATICEDGE,
		className, NULL, dwStyle&(~WS_CAPTION),
		rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,
		pParentWnd->GetSafeHwnd(), (HMENU)nID);

	//设置边框
	ModifyStyleEx(WS_EX_CLIENTEDGE, NULL, SWP_DRAWFRAME);
	ModifyStyleEx(WS_EX_WINDOWEDGE, NULL, SWP_DRAWFRAME);

	if (result != 0)
		InvalidateCtrl();

	return result;

} // Create   

void COScopeCtrl::SetLimit(double dStandardValue, double dNGTolerance, double dWarnTolerance, double dMarginDiff, int nDecimalPlaces, BOOL bInvalidate)
{
	ASSERT(dNGTolerance != 0);
	ASSERT(dWarnTolerance != 0);
	ASSERT(dMarginDiff > dNGTolerance);

	m_dLowerWarnLimit = dStandardValue - abs(dWarnTolerance);
	m_dUpperWarnLimit = dStandardValue + abs(dWarnTolerance);

	m_dLowerNGLimit = dStandardValue - abs(dNGTolerance);
	m_dUpperNGLimit = dStandardValue + abs(dNGTolerance);

	m_dLowerMargin = dStandardValue - abs(dMarginDiff);
	m_dUpperMargin = dStandardValue + abs(dMarginDiff);

	m_nYDecimals = nDecimalPlaces;
	m_dRange = m_dUpperMargin - m_dLowerMargin;
	m_dVerticalFactor = (double)m_nPlotHeight / m_dRange;

	// clear out the existing garbage, re-start with a clean plot  
	if (bInvalidate)
		InvalidateCtrl();
}

/////////////////////////////////////////////////////////////////////////////   
void COScopeCtrl::SetXUnits(CString string, BOOL bInvalidate)
{
	m_strXUnitsString = string;

	// clear out the existing garbage, re-start with a clean plot   
	if (bInvalidate)
		InvalidateCtrl();

}  // SetXUnits   

/////////////////////////////////////////////////////////////////////////////   
void COScopeCtrl::SetYUnits(CString string, BOOL bInvalidate)
{
	m_strYUnitsString = string;

	// clear out the existing garbage, re-start with a clean plot   
	if (bInvalidate)
		InvalidateCtrl();

}  // SetYUnits   

/////////////////////////////////////////////////////////////////////////////  
void COScopeCtrl::SetTitleText(CString string, BOOL bInvalidate)
{
	m_strTitleString = string;

	// clear out the existing garbage, re-start with a clean plot   
	if (bInvalidate)
		InvalidateCtrl();
}

/////////////////////////////////////////////////////////////////////////////   
void COScopeCtrl::SetGridColor(COLORREF color, BOOL bInvalidate)
{
	m_crGridColor = color;

	// clear out the existing garbage, re-start with a clean plot   
	if (bInvalidate)
		InvalidateCtrl();
}

void COScopeCtrl::SetLimitLineColor(COLORREF colorNG, COLORREF colorWarn, BOOL bInvalidate)
{
	m_crGridNGColor = colorNG;
	m_crGridWarnColor = colorWarn;

	//// clear out the existing garbage, re-start with a clean plot   
	if (bInvalidate)
		InvalidateCtrl();
}


/////////////////////////////////////////////////////////////////////////////   
void COScopeCtrl::SetBackgroundColor(COLORREF color, BOOL bInvalidate)
{
	m_crBackColor = color;

	m_brushBack.DeleteObject();
	m_brushBack.CreateSolidBrush(m_crBackColor);

	// clear out the existing garbage, re-start with a clean plot   
	if (bInvalidate)
		InvalidateCtrl();

}  // SetBackgroundColor

/////////////////////////////////////////////////////////////////////////////   
void COScopeCtrl::SetTitleBackgroundColor(COLORREF color, BOOL bInvalidate)
{
	m_crTitleBackColor = color;

	m_brushTitleBack.DeleteObject();
	m_brushTitleBack.CreateSolidBrush(m_crTitleBackColor);
	// clear out the existing garbage, re-start with a clean plot   
	if (bInvalidate)
		InvalidateCtrl();
}  // SetTitleBackgroundColor

void COScopeCtrl::SetTitleTextColor(COLORREF color, BOOL bInvalidate)
{
	m_crTitleTextColor = color;
	if (bInvalidate)
		InvalidateCtrl();
}  // SetTitleTextColor

void COScopeCtrl::DrawGridLine()
{
	//画横线
	CPen solidPen(PS_SOLID, 2, m_crGridColor);
	CPen MidGridPen(PS_SOLID, 2, m_crGridColor);
	CPen NGGridPen(PS_SOLID, 2, m_crGridNGColor);
	CPen WarnGridPen(PS_SOLID, 2, m_crGridWarnColor);

	//改为预警线内放大显示比例，NG线到预警线缩小显示比例
	double dStdVal = (m_dUpperNGLimit + m_dLowerNGLimit) / 2;
	int nMidGridPix = (m_rectPlot.top + m_rectPlot.bottom) / 2;
	int nUpperNGGridPix = (m_rectPlot.top + m_rectPlot.bottom) / 2 - m_rectPlot.Height() / 2 * (m_dUpperNGLimit - dStdVal) / (m_dUpperMargin - dStdVal);
	int nLowerNGGridPix = (m_rectPlot.top + m_rectPlot.bottom) / 2 + m_rectPlot.Height() / 2 * (dStdVal - m_dLowerNGLimit) / (dStdVal - m_dLowerMargin);
	int nUpperWarnGridPix = (m_rectPlot.top + m_rectPlot.bottom) / 2 - m_rectPlot.Height() / 2 * (m_dUpperWarnLimit - dStdVal) / (m_dUpperMargin - dStdVal);
	int nLowerWarnGridPix = (m_rectPlot.top + m_rectPlot.bottom) / 2 + m_rectPlot.Height() / 2 * (dStdVal - m_dLowerWarnLimit) / (dStdVal - m_dLowerMargin);
	int nUpperMarginGridPix = (m_rectPlot.top + m_rectPlot.bottom) / 2 - m_rectPlot.Height() / 2;
	int nLowerMarginGridPix = (m_rectPlot.top + m_rectPlot.bottom) / 2 + m_rectPlot.Height() / 2;

	// draw the plot rectangle   
	m_pOldPen = m_dcGrid.SelectObject(&solidPen);
	m_dcGrid.MoveTo(m_rectPlot.right, m_rectPlot.bottom + 1);
	m_dcGrid.LineTo(m_rectPlot.left, m_rectPlot.bottom + 1);

	m_dcGrid.MoveTo(m_rectPlot.right, m_rectPlot.top - 1);
	m_dcGrid.LineTo(m_rectPlot.left, m_rectPlot.top - 1);

	m_dcGrid.SelectObject(m_pOldPen);

	//中轴线
	m_pOldPen = m_dcGrid.SelectObject(&MidGridPen);
	for (int i = m_rectPlot.left; i < m_rectPlot.right; i += m_nShiftPixels)
	{
		m_dcGrid.MoveTo(i, nMidGridPix);
		m_dcGrid.LineTo(i + m_nShiftPixels - 2, nMidGridPix);
	}
	m_dcGrid.SelectObject(&m_pOldPen);

	//NG线
	m_pOldPen = m_dcGrid.SelectObject(&NGGridPen);
	for (int i = m_rectPlot.left; i < m_rectPlot.right; i += m_nShiftPixels)
	{
		m_dcGrid.MoveTo(i, nUpperNGGridPix);
		m_dcGrid.LineTo(i + m_nShiftPixels - 2, nUpperNGGridPix);

		m_dcGrid.MoveTo(i, nLowerNGGridPix);
		m_dcGrid.LineTo(i + m_nShiftPixels - 2, nLowerNGGridPix);
	}
	m_dcGrid.SelectObject(&m_pOldPen);
	//预警线
	if (m_bShowWarnLimitLine)
	{
		m_pOldPen = m_dcGrid.SelectObject(&WarnGridPen);
		for (int i = m_rectPlot.left; i < m_rectPlot.right; i += m_nShiftPixels)
		{
			m_dcGrid.MoveTo(i, nUpperWarnGridPix);
			m_dcGrid.LineTo(i + m_nShiftPixels - 2, nUpperWarnGridPix);

			m_dcGrid.MoveTo(i, nLowerWarnGridPix);
			m_dcGrid.LineTo(i + m_nShiftPixels - 2, nLowerWarnGridPix);
		}
		m_dcGrid.SelectObject(&m_pOldPen);
	}
}

void COScopeCtrl::DrawTitle(CDC &memDC)
{
	CFont axisFont, titleFont;
	titleFont.CreateFontA(21, 0, 0, 0, 600,
		FALSE, FALSE, 0, ANSI_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_SWISS, "Microsoft Yahei");// 
	memDC.SetBkMode(OPAQUE);
	memDC.SetBkColor(m_crTitleBackColor);
	memDC.FillRect(m_rectTitle, &m_brushTitleBack);

	//m_dcGrid.SetBkMode(TRANSPARENT);
	m_pOldFont = memDC.SelectObject(&titleFont);
	//title
	if ("" != m_strTitleString)
	{
		memDC.SetTextColor(m_crTitleTextColor);

		if ((m_bShowCpk || m_bShowSig) && m_vecPlotPoint.size() > 1)
		{
			memDC.SetTextAlign(TA_LEFT);
			memDC.TextOut(m_rectTitle.left + 2, m_rectTitle.top + 3, m_strTitleString);
		}
		else
		{
			memDC.SetTextAlign(TA_CENTER);
			memDC.TextOut(m_rectTitle.right / 2, m_rectTitle.top + 3, m_strTitleString);
		}

	}

	//写Cpk
	BOOL bShow = FALSE;
	if (m_vecPlotPoint.size() > 1 && m_bShowCpk && m_vecPlotPoint[1].dCpk != 0)
		bShow = TRUE;
	else if(m_vecPlotPoint.size() > 1 && !m_bShowCpk && m_bShowSig)
		bShow = TRUE;
	if (bShow)
	{
		tagPoint tPt = m_vecPlotPoint[1];
		COLORREF cl(m_crTitleTextColor);
		TEXTMETRIC metric;
		memDC.GetTextMetrics(&metric);
		m_strCpk = "";
		if (m_bShowCpk && m_bShowSig)
		{
			m_strCpk.Format("Cpk:%0.2f Sig:%0.2f", tPt.dCpk, tPt.dSig);
			if (tPt.dCpk < m_dCpkLowerNGLimit)
				cl = RGB(255, 0, 0);
		}
		else if (m_bShowCpk)
		{
			m_strCpk.Format("Cpk:%0.2f", tPt.dCpk);
			COLORREF cl(m_crTitleTextColor);
			if (tPt.dCpk < m_dCpkLowerNGLimit)
				cl = RGB(255, 0, 0);
		}
		else if (m_bShowSig)
		{
			m_strCpk.Format("Sig:%0.2f", tPt.dSig);
			COLORREF cl(m_crTitleTextColor);
		}

		if (m_strCpk != "")
		{
			CFont CpkFont;
			CpkFont.CreateFontA(18, 0, 0, 0, 600,
				FALSE, FALSE, 0, ANSI_CHARSET,
				OUT_DEFAULT_PRECIS,
				CLIP_DEFAULT_PRECIS,
				DEFAULT_QUALITY,
				DEFAULT_PITCH | FF_SWISS, "Microsoft Yahei");

			m_pOldFont = memDC.SelectObject(&CpkFont);
			memDC.SetTextColor(cl);
			memDC.SetTextAlign(TA_RIGHT | TA_TOP);
			memDC.TextOut(m_rectTitle.right - 2, m_rectTitle.top + 5, m_strCpk);
		}
	}

	memDC.SelectObject(m_pOldFont);
	//m_dcGrid.SetBkMode(OPAQUE);
}

void COScopeCtrl::DrawTitle()
{
	return;
	CFont axisFont, titleFont;
	titleFont.CreateFontA(21, 0, 0, 0, 600,
		FALSE, FALSE, 0, ANSI_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_SWISS, "Microsoft Yahei");// 
	m_dcGrid.SetBkMode(OPAQUE);
	m_dcGrid.SetBkColor(m_crTitleBackColor);
	m_dcGrid.FillRect(m_rectTitle, &m_brushTitleBack);

	//m_dcGrid.SetBkMode(TRANSPARENT);
	m_pOldFont = m_dcGrid.SelectObject(&titleFont);
	//title
	if ("" != m_strTitleString)
	{
		m_dcGrid.SetTextColor(m_crTitleTextColor);
		
		/*if (m_bShowCpk || m_bShowSig)
		{
			m_dcGrid.SetTextAlign(TA_LEFT);
			m_dcGrid.TextOut(m_rectTitle.left + 2, m_rectTitle.top + 3, m_strTitleString);
		}
		else*/
		{
			m_dcGrid.SetTextAlign(TA_CENTER);
			m_dcGrid.TextOut(m_rectTitle.right / 2, m_rectTitle.top + 3, m_strTitleString);
		}
			
	}

	//写Cpk
	/*if (m_vecPlotPoint.size() > 1 && m_vecPlotPoint[1].dCpk!=0)
	{
		tagPoint tPt = m_vecPlotPoint[1];
		COLORREF cl(m_crTitleTextColor);
		TEXTMETRIC metric;
		m_dcGrid.GetTextMetrics(&metric);
		m_strCpk = "";
		if (m_bShowCpk && m_bShowSig)
		{
			m_strCpk.Format("Cpk:%0.2f Sig:%0.2f", tPt.dCpk, tPt.dSig);
			if (tPt.dCpk < m_dCpkLowerNGLimit)
				cl = RGB(255, 0, 0);
		}
		else if (m_bShowCpk)
		{
			m_strCpk.Format("Cpk:%0.2f", tPt.dCpk);
			COLORREF cl(m_crTitleTextColor);
			if (tPt.dCpk < m_dCpkLowerNGLimit)
				cl = RGB(255, 0, 0);
		}
		else if (m_bShowSig)
		{
			m_strCpk.Format("Sig:%0.2f", tPt.dSig);
			COLORREF cl(m_crTitleTextColor);
		}

		if (m_strCpk != "")
		{
			CFont CpkFont;
			CpkFont.CreateFontA(18, 0, 0, 0, 600,
				FALSE, FALSE, 0, ANSI_CHARSET,
				OUT_DEFAULT_PRECIS,
				CLIP_DEFAULT_PRECIS,
				DEFAULT_QUALITY,
				DEFAULT_PITCH | FF_SWISS, "Microsoft Yahei");

			m_pOldFont = m_dcGrid.SelectObject(&CpkFont);
			m_dcGrid.SetTextColor(cl);
			m_dcGrid.SetTextAlign(TA_RIGHT | TA_TOP);
			m_dcGrid.TextOut(m_rectTitle.right - 2, m_rectTitle.top + 5, m_strCpk);
		}
	}*/

	m_dcGrid.SelectObject(m_pOldFont);
	//m_dcGrid.SetBkMode(OPAQUE);
}

/////////////////////////////////////////////////////////////////////////////   
void COScopeCtrl::InvalidateCtrl()
{
	// There is a lot of drawing going on here - particularly in terms of    
	// drawing the grid.  Don't panic, this is all being drawn (only once)   
	// to a bitmap.  The result is then BitBlt'd to the control whenever needed.   
	LockWindowUpdate();
	int i;
	int nCharacters;
	
	CPen solidPen(PS_SOLID, 2, m_crGridColor);
	
	CPen tlPen(PS_SOLID, 0, RGB(64, 64, 64));
	CFont axisFont, titleFont;
	CString strTemp;

	// in case we haven't established the memory dc's   
	CClientDC dc(this);
	CPaintDC dDc(this);

	// if we don't have one yet, set up a memory dc for the grid   
	if (m_dcGrid.GetSafeHdc() == NULL)
	{
		m_dcGrid.CreateCompatibleDC(&dc);
		m_bitmapGrid.CreateCompatibleBitmap(&dc, m_nClientWidth, m_nClientHeight);
		m_pbitmapOldGrid = m_dcGrid.SelectObject(&m_bitmapGrid);
	}

	DrawTitle();

	//titleFont.CreateFontA(21, 0, 0, 0, 600,
	//	FALSE, FALSE, 0, ANSI_CHARSET,
	//	OUT_DEFAULT_PRECIS,
	//	CLIP_DEFAULT_PRECIS,
	//	DEFAULT_QUALITY,
	//	DEFAULT_PITCH | FF_SWISS, "Microsoft Yahei");// 
	//m_dcGrid.SetBkColor(m_crTitleBackColor);
	//m_dcGrid.FillRect(m_rectTitle, &m_brushTitleBack);

	//oldFont = m_dcGrid.SelectObject(&titleFont);
	////title
	//if ("" != m_strTitleString)
	//{
	//	m_dcGrid.SetTextColor(m_crTitleTextColor);
	//	m_dcGrid.SetTextAlign(TA_LEFT);
	//	m_dcGrid.TextOut(m_rectTitle.left + 2, m_rectTitle.top + 3, m_strTitleString);

	//	CString strCpk;
	//	strCpk.Format("Cpk:%0.2f Sig:%0.2f", 1.1, 1.3);
	//	COLORREF cl(m_crTitleTextColor);
	//	//if (m_vecPlotPoint[0].dCpk < m_dCpkLowerNGLimit)
	//		//cl = RGB(255, 0, 0);
	//	CFont CpkFont;
	//	CpkFont.CreateFontA(18, 0, 0, 0, 600,
	//		FALSE, FALSE, 0, ANSI_CHARSET,
	//		OUT_DEFAULT_PRECIS,
	//		CLIP_DEFAULT_PRECIS,
	//		DEFAULT_QUALITY,
	//		DEFAULT_PITCH | FF_SWISS, "Microsoft Yahei");

	//	oldFont = m_dcGrid.SelectObject(&CpkFont);
	//	m_dcGrid.SetTextColor(cl);
	//	m_dcGrid.SetTextAlign(TA_RIGHT | TA_TOP);
	//	m_dcGrid.TextOut(m_rectTitle.right - 2, m_rectTitle.top + 4, strCpk);
	//}

	//m_dcGrid.SelectObject(oldFont);

	m_dcGrid.SetBkColor(m_crBackColor);
	// fill the grid background   
	m_dcGrid.FillRect(m_rectClient, &m_brushBack);
	m_dcGrid.SetBkMode(TRANSPARENT);
	// draw the plot rectangle:
	// determine how wide the y axis scaling values are   
	nCharacters = abs((int)log10(fabs(m_dUpperMargin)));
	nCharacters = max(nCharacters, abs((int)log10(fabs(m_dLowerMargin))));

	// add the units digit, decimal point and a minus sign, and an extra space   
	// as well as the number of decimal places to display   
	nCharacters = nCharacters + 6 + m_nYDecimals;

	// adjust the plot rectangle dimensions
	// assume 6 pixels per character (this may need to be adjusted)   
	m_rectPlot.left = m_rectClient.left + 5 * (nCharacters);
	m_nPlotWidth = m_rectPlot.Width();

	/*m_pOldPen = m_dcGrid.SelectObject(&tlPen);
	m_dcGrid.MoveTo(m_rectTitle.left, m_rectTitle.bottom);
	m_dcGrid.LineTo(m_rectTitle.right, m_rectTitle.bottom);*/

	
	/*for (i = m_rectPlot.left; i < m_rectPlot.right; i += 4)
	{
		m_dcGrid.SetPixel(i, m_rectPlot.top, m_crGridColor);
	}*/

	// draw the dotted lines,    
	// use SetPixel instead of a dotted pen - this allows for a    
	// finer dotted line and a more "technical" look   

	double dStdVal = (m_dUpperNGLimit + m_dLowerNGLimit) / 2;
	int nMidGridPix = (m_rectPlot.top + m_rectPlot.bottom) / 2;
	int nUpperNGGridPix = (m_rectPlot.top + m_rectPlot.bottom) / 2 - m_rectPlot.Height() / 2 * (m_dUpperNGLimit - dStdVal) / (m_dUpperMargin - dStdVal);
	int nLowerNGGridPix = (m_rectPlot.top + m_rectPlot.bottom) / 2 + m_rectPlot.Height() / 2 * (dStdVal - m_dLowerNGLimit) / (dStdVal - m_dLowerMargin);
	int nUpperWarnGridPix = (m_rectPlot.top + m_rectPlot.bottom) / 2 - m_rectPlot.Height() / 2 * (m_dUpperWarnLimit - dStdVal) / (m_dUpperMargin - dStdVal);
	int nLowerWarnGridPix = (m_rectPlot.top + m_rectPlot.bottom) / 2 + m_rectPlot.Height() / 2 * (dStdVal - m_dLowerWarnLimit) / (dStdVal - m_dLowerMargin);
	int nUpperMarginGridPix = (m_rectPlot.top + m_rectPlot.bottom) / 2 - m_rectPlot.Height() / 2;
	int nLowerMarginGridPix = (m_rectPlot.top + m_rectPlot.bottom) / 2 + m_rectPlot.Height() / 2;
	
	DrawGridLine();

	// create some fonts (horizontal and vertical)   
	// use a height of 14 pixels and 300 weight    
	// (these may need to be adjusted depending on the display)   
	axisFont.CreateFont(17, 0, 0, 0, 600,
		FALSE, FALSE, 0, ANSI_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_SWISS, "Microsoft Yahei");//

	// restore the font   
	//m_dcGrid.SelectObject(m_pOldFont);
	// grab the horizontal font   
	m_pOldFont = m_dcGrid.SelectObject(&axisFont);

	TEXTMETRIC metric;
	m_dcGrid.GetTextMetrics(&metric);

	m_dcGrid.SetTextColor(m_crTitleTextColor);

	//线颜色和名称
	int GAP_HEIGHT = 15;
	int GAP_WIDTH = 15;
	int LINE_WIDTH = 15;
	/*if (m_vecPlotName.size() > 8)
	{
		GAP_HEIGHT = max(9,(m_rectPlot.Height() - metric.tmHeight *m_vecPlotName.size()) / m_vecPlotName.size());
	}*/
	int nWidthCount = 10;
	for (int i = 0; i < m_vecPlotName.size(); i++)
	{
		m_dcGrid.SetTextAlign(TA_LEFT | TA_TOP);
		m_dcGrid.TextOut(m_rectPlot.left + nWidthCount, m_rectClient.bottom - 14 - metric.tmHeight / 2, m_vecPlotName[i]);
		tagRECT rc;
		rc.top = m_rectClient.bottom - 15;
		rc.left = m_rectPlot.left + nWidthCount + m_vecPlotName[i].GetLength()*metric.tmAveCharWidth + 5;
		rc.right = rc.left + LINE_WIDTH;
		rc.bottom = rc.top + 3;
		COLORREF rcl;
		if (m_vecPlotColor.size() > i)
			rcl = m_vecPlotColor[i];
		else
			rcl = RGB(198, 198, 198);
		CBrush br(rcl);
		m_dcGrid.FillRect(&rc, &br);
		nWidthCount += m_vecPlotName[i].GetLength()*metric.tmAveCharWidth + 5 + LINE_WIDTH + GAP_WIDTH;

		/*m_dcGrid.SetTextAlign(TA_RIGHT | TA_TOP);
		m_dcGrid.TextOut(m_rectPlot.right + 70, m_rectTitle.bottom + GAP_HEIGHT * i + 10 - metric.tmHeight / 2, m_vecPlotName[i]);
		tagRECT rc;
		rc.top = m_rectTitle.bottom + GAP_HEIGHT * i + 10 - 3;
		rc.left = m_rectPlot.right + 80;
		rc.right = min(m_rectClient.right - 20, rc.left + 50);
		rc.bottom = rc.top + 5;
		COLORREF rcl;
		if (m_vecPlotColor.size() > i)
			rcl = m_vecPlotColor[i];
		else
			rcl = RGB(198,198,198);
		CBrush br(rcl);
		m_dcGrid.FillRect(&rc, &br);*/
	}

	// y upper ng

	m_dcGrid.SetTextAlign(TA_RIGHT | TA_TOP);
	strTemp.Format("%.*lf", m_nYDecimals, m_dUpperNGLimit);
	m_dcGrid.TextOut(m_rectPlot.left - 5, nUpperNGGridPix - metric.tmHeight / 2, strTemp);   //此处为了显示客户要求的上下限2017.09.01
																												   // y lower ng  
	m_dcGrid.SetTextAlign(TA_RIGHT | TA_TOP);
	strTemp.Format("%.*lf", m_nYDecimals, m_dLowerNGLimit);
	m_dcGrid.TextOut(m_rectPlot.left - 5, nLowerNGGridPix - metric.tmHeight / 2, strTemp);

	if (m_bShowWarnLimitLine)
	{
		// y upper warn
		m_dcGrid.SetTextAlign(TA_RIGHT | TA_TOP);
		strTemp.Format("%.*lf", m_nYDecimals, m_dUpperWarnLimit);
		m_dcGrid.TextOut(m_rectPlot.left - 5, nUpperWarnGridPix - metric.tmHeight / 2, strTemp);
		// y lower warn
		m_dcGrid.SetTextAlign(TA_RIGHT | TA_TOP);
		strTemp.Format("%.*lf", m_nYDecimals, m_dLowerWarnLimit);
		m_dcGrid.TextOut(m_rectPlot.left - 5, nLowerWarnGridPix - metric.tmHeight / 2, strTemp);
	}

	// y upper margin
	m_dcGrid.SetTextAlign(TA_RIGHT | TA_TOP);
	strTemp.Format("%.*lf", m_nYDecimals, m_dUpperMargin);
	m_dcGrid.TextOut(m_rectPlot.left - 5, m_rectPlot.top - metric.tmHeight / 2, strTemp);
	// y lower margin
	m_dcGrid.SetTextAlign(TA_RIGHT | TA_TOP);
	strTemp.Format("%.*lf", m_nYDecimals, m_dLowerMargin);
	m_dcGrid.TextOut(m_rectPlot.left - 5, m_rectPlot.bottom - metric.tmHeight / 2, strTemp);

	// y center  
	m_dcGrid.SetTextAlign(TA_RIGHT | TA_TOP);
	strTemp.Format("%.*lf", m_nYDecimals, (m_dUpperNGLimit + m_dLowerNGLimit) / 2);
	m_dcGrid.TextOut(m_rectPlot.left - 5, nMidGridPix - metric.tmHeight / 2, strTemp);

	// x min   
	m_dcGrid.SetTextAlign(TA_RIGHT | TA_TOP);
	m_dcGrid.TextOut(m_rectPlot.left, m_rectPlot.bottom + 4, "0");

	// x max   
	m_dcGrid.SetTextAlign(TA_RIGHT | TA_TOP);
	strTemp.Format("%d", m_nPlotWidth / m_nShiftPixels);
	m_dcGrid.TextOut(m_rectPlot.right, m_rectPlot.bottom + 4, strTemp);

	// x units   
	if ("" != m_strXUnitsString)
	{
		m_dcGrid.SetTextAlign(TA_CENTER | TA_TOP);
		m_dcGrid.TextOut((m_rectPlot.left + m_rectPlot.right) / 2,
			m_rectPlot.bottom + 4, m_strXUnitsString);
	}

	// y units     
	if ("" != m_strYUnitsString)
	{
		m_dcGrid.SetTextAlign(TA_CENTER | TA_BASELINE);
		m_dcGrid.TextOut((m_rectClient.left + m_rectPlot.left) / 2,
			(m_rectPlot.bottom + m_rectPlot.top) / 2, m_strYUnitsString);
		m_dcGrid.SelectObject(m_pOldFont);
	}

	m_dcGrid.SetBkMode(OPAQUE);

	// at this point we are done filling the the grid bitmap,    
	// no more drawing to this bitmap is needed until the setting are changed   

	// if we don't have one yet, set up a memory dc for the plot   
	if (m_dcPlot.GetSafeHdc() == NULL)
	{
		m_dcPlot.CreateCompatibleDC(&dc);
		m_bitmapPlot.CreateCompatibleBitmap(&dc, m_nClientWidth, m_nClientHeight);
		m_pbitmapOldPlot = m_dcPlot.SelectObject(&m_bitmapPlot);
	}

	// make sure the plot bitmap is cleared   
	//m_dcPlot.SetBkColor(m_crBackColor);
	//m_dcPlot.FillRect(m_rectClient, &m_brushBack);

	// finally, force the plot area to redraw   
	InvalidateRect(m_rectClient);

	UnlockWindowUpdate();
} // InvalidateCtrl   


void COScopeCtrl::AppendPoint(tagPoint tPt)
{
	m_vecPlotPoint.push_back(tPt);
	if (m_vecPlotPoint.size() > 2)
		m_vecPlotPoint.erase(m_vecPlotPoint.begin());

	//DrawPoint();
	Invalidate();
}

void COScopeCtrl::SetPlotName(std::vector<CString> vecName, BOOL bInvalidate)
{
	//ASSERT(vecName.size() > 0);

	m_vecPlotName = vecName;

	if (bInvalidate)
		InvalidateCtrl();
}

void COScopeCtrl::SetPlotColor(std::vector<COLORREF> vecColor, BOOL bInvalidate)
{
	ASSERT(vecColor.size() > 0);

	m_vecPlotColor = vecColor;

	if (bInvalidate)
		InvalidateCtrl();
}

////////////////////////////////////////////////////////////////////////////   
void COScopeCtrl::OnPaint()
{
	CPaintDC dc(this);  // device context for painting   
	CDC memDC;
	CBitmap memBitmap;
	CBitmap* oldBitmap; // bitmap originally found in CMemDC   

	// no real plotting work is performed here,    
	// just putting the existing bitmaps on the client   

	// to avoid flicker, establish a memory dc, draw to it    
	// and then BitBlt it to the client   
	memDC.CreateCompatibleDC(&dc);
	memBitmap.CreateCompatibleBitmap(&dc, m_nClientWidth, m_nClientHeight);
	oldBitmap = (CBitmap *)memDC.SelectObject(&memBitmap);

	if (memDC.GetSafeHdc() != NULL)
	{
		// first drop the grid on the memory dc   
		memDC.BitBlt(0, 0, m_nClientWidth, m_nClientHeight,
			&m_dcGrid, 0, 0, SRCCOPY);
		// now add the plot on top as a "pattern" via SRCPAINT.   
		// works well with dark background and a light plot   
		//memDC.BitBlt(0, 0, m_nClientWidth, m_nClientHeight,
			//&m_dcPlot, 0, 0, SRCPAINT);  //SRCPAINT   
			// finally send the result to the display   

		//写Cpk
		DrawTitle(memDC);

		dc.BitBlt(0, 0, m_nClientWidth, m_nClientHeight,
			&memDC, 0, 0, SRCCOPY);
	}

	memDC.SelectObject(oldBitmap);

	//DrawTitle();
	DrawPoint();

} // OnPaint   

/////////////////////////////////////////////////////////////////////////////   
void COScopeCtrl::DrawPoint()
{
	// this does the work of "scrolling" the plot to the left   
	// and appending a new data point all of the plotting is    
	// directed to the memory based bitmap associated with m_dcPlot   
	// the will subsequently be BitBlt'd to the client in OnPaint   
	LockWindowUpdate();
	int currX, prevX, currY, prevY;
	int RADIUS = 2;

	CPen *oldPen;
	CRect rectCleanUp;

	if (m_dcPlot.GetSafeHdc() != NULL)
	{
		// shift the plot by BitBlt'ing it to itself    
		// note: the m_dcPlot covers the entire client   
		//       but we only shift bitmap that is the size    
		//       of the plot rectangle   
		// grab the right side of the plot (exluding m_nShiftPixels on the left)   
		// move this grabbed bitmap to the left by m_nShiftPixels   

		// set work region

		//DrawTitle();

		CRgn rgnWork;
		rgnWork.CreateRectRgn(m_rectPlot.left, m_rectPlot.bottom + 3, m_rectPlot.right, m_rectPlot.top - 3);
		m_dcGrid.SelectClipRgn(&rgnWork);

		//m_dcGrid.SetBkMode(TRANSPARENT);
		m_dcGrid.BitBlt(m_rectPlot.left, m_rectPlot.top - 3,
			m_nPlotWidth, m_nPlotHeight + 6, &m_dcGrid,
			m_rectPlot.left + m_nShiftPixels, m_rectPlot.top - 3,
			SRCCOPY);

		// establish a rectangle over the right side of plot   
		// which now needs to be cleaned up proir to adding the new point   
		rectCleanUp = m_rectPlot;
		rectCleanUp.left = rectCleanUp.right - m_nShiftPixels;

		// fill the cleanup area with the background   
		//m_dcPlot.FillRect(rectCleanUp, &m_brushBack);

		DrawGridLine();

		CPen solidPen(PS_SOLID, 2, m_crGridColor);

		if (m_vecPlotPoint.size() == 1)
		{
			//点的数量只有一个
			// draw to the current point   
			currX = m_rectPlot.right - m_nHalfShiftPixels;
			for (int i = 0; i < m_vecPlotPoint[0].vecPt.size(); i++)
			{
				currY = (m_dUpperMargin - m_vecPlotPoint[0].vecPt[i]) / (m_dUpperMargin - m_dLowerMargin) * m_rectPlot.Height() + m_rectPlot.top;

				CBrush br;
				CPen pen;
				COLORREF cl(RGB(255, 255, 0));
				if (m_vecPlotColor.size() > i)
					cl = m_vecPlotColor[i];

				br.CreateSolidBrush(cl);
				pen.CreatePen(0, 4, cl);

				m_pOldBrush = m_dcGrid.SelectObject(&br);
				m_pOldPen = m_dcGrid.SelectObject(&pen);
				//m_dcPlot.Ellipse(currX - RADIUS, currY - RADIUS, currX + RADIUS, currY + RADIUS);
				m_dcGrid.SelectObject(m_pOldBrush);
				m_dcGrid.SelectObject(m_pOldPen);
			}
		}
		else if (m_vecPlotPoint.size() > 1)
		{
			//有两点
			//draw to the current point  
			prevX = m_rectPlot.right - m_nPlotShiftPixels;
			currX = m_rectPlot.right - m_nHalfShiftPixels;
			for (int i = 0; i < m_vecPlotPoint[0].vecPt.size(); i++)
			{
				prevY = (m_dUpperMargin - m_vecPlotPoint[0].vecPt[i]) / (m_dUpperMargin - m_dLowerMargin) * m_rectPlot.Height() + m_rectPlot.top;
				currY = (m_dUpperMargin - m_vecPlotPoint[1].vecPt[i]) / (m_dUpperMargin - m_dLowerMargin) * m_rectPlot.Height() + m_rectPlot.top;

				CBrush br;
				CPen pen;
				COLORREF cl(RGB(255, 255, 0));
				if (m_vecPlotColor.size() > i)
					cl = m_vecPlotColor[i];
				if (m_vecPlotPoint[1].vecPt[i] > m_dUpperNGLimit || m_vecPlotPoint[1].vecPt[i] < m_dLowerNGLimit
					|| m_vecPlotPoint[0].vecPt[i] > m_dUpperNGLimit || m_vecPlotPoint[0].vecPt[i] < m_dLowerNGLimit)
				{
					if(i==0) cl = m_crPointNGColor[0];
					else cl = m_crPointNGColor[1];
				}
					
				

				br.CreateSolidBrush(cl);
				pen.CreatePen(0, 4, cl);

				m_pOldBrush = m_dcGrid.SelectObject(&br);
				m_pOldPen = m_dcGrid.SelectObject(&pen);
				m_dcGrid.MoveTo(prevX, prevY);
				m_dcGrid.LineTo(currX, currY);
				//m_dcPlot.Ellipse(currX - RADIUS, currY - RADIUS, currX + RADIUS, currY + RADIUS);
				m_dcGrid.SelectObject(m_pOldBrush);
				m_dcGrid.SelectObject(m_pOldPen);
			}
		}

		

		//m_dcGrid.SetBkMode(OPAQUE);
		// restore the pen    

		// if the data leaks over the upper or lower plot boundaries   
		// fill the upper and lower leakage with the background   
		// this will facilitate clipping on an as needed basis   
		// as opposed to always calling IntersectClipRect   
			  // store the current point for connection to the next point   
	}
	UnlockWindowUpdate();
} // end DrawPoint   

/////////////////////////////////////////////////////////////////////////////   
void COScopeCtrl::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	// NOTE: OnSize automatically gets called during the setup of the control   

	GetClientRect(m_rectClient);
	CRect rcs;
	GetWindowRect(rcs);
	m_rectClient.left = 0;
	m_rectClient.right = rcs.right - rcs.left;
	m_rectClient.top = 0;
	m_rectClient.bottom = rcs.bottom - rcs.top;

	// set some member variables to avoid multiple function calls   
	m_nClientHeight = m_rectClient.Height();
	m_nClientWidth = m_rectClient.Width();

	// the "left" coordinate and "width" will be modified in    
	// InvalidateCtrl to be based on the width of the y axis scaling   
	m_rectPlot.left = 70;
	m_rectPlot.top = 50;
	m_rectPlot.right = m_rectClient.right - 20/* - min(150, m_rectClient.Width())*/;
	m_rectPlot.bottom = m_rectClient.bottom - 30;

	m_rectTitle.left = m_rectClient.left;
	m_rectTitle.top = m_rectClient.top;
	m_rectTitle.right = m_rectClient.right;
	m_rectTitle.bottom = m_rectClient.top + 30;
	m_rectClient.top = 30;

	// set some member variables to avoid multiple function calls   
	m_nPlotHeight = m_rectPlot.Height();
	m_nPlotWidth = m_rectPlot.Width();

	// set the scaling factor for now, this can be adjusted    
	// in the SetRange functions   
	m_dVerticalFactor = (double)m_nPlotHeight / m_dRange;

} // OnSize   

BOOL COScopeCtrl::OnEraseBkgnd(CDC* pDC)
{
	return true;
	//CWnd::OnEraseBkgnd(pDC);
}


/////////////////////////////////////////////////////////////////////////////   
void COScopeCtrl::Reset()
{
	// to clear the existing data (in the form of a bitmap)   
	// simply invalidate the entire control   
	InvalidateCtrl();
}

void COScopeCtrl::OnNcPaint()
{
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CWnd::OnNcPaint()
	CDC* pDc = GetWindowDC();

	// Paint into this DC

	RECT rcWin;

	GetWindowRect(&rcWin);

	OffsetRect(&rcWin, -rcWin.left, -rcWin.top);

	HBRUSH hbr = CreateSolidBrush(m_crBackColor);
	FrameRect(pDc->m_hDC, &rcWin, hbr);
	InflateRect(&rcWin, -1, -1);


	ReleaseDC(pDc);
}

void COScopeCtrl::SetCpkLimit(double dUpperNGLimit)
{
	ASSERT(dUpperNGLimit != 0);

	m_dCpkLowerNGLimit = dUpperNGLimit;
}