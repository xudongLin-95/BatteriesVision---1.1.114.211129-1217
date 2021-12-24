// BaseTabCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "BaseTabCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBaseTabCtrl

CBaseTabCtrl::CBaseTabCtrl()
{
	m_crTabBKNormal = RGB(255,255,255);// use default color
	m_crTabBKFocus = RGB(255,255,255);
	m_crBDNormal = RGB(200,200,200);
	m_crBDFocus = RGB(18,183,245);
	m_crTextFocus = RGB(0,0,0);
	m_crTextNormal = RGB(150,150,150);

	LOGFONT lf;
	//strcpy(lf.lfFaceName,"Microsoft YaHei");//Tahoma
	//lf.lfHeight=-15;
	//lf.lfWidth=0;
	//lf.lfEscapement = 0;
	//lf.lfOrientation = 0;
	//lf.lfItalic = 0;
	//lf.lfUnderline = 0;
	//lf.lfStrikeOut = 0;
	//lf.lfWeight = 400;
	//lf.lfCharSet = ANSI_CHARSET;
	//lf.lfOutPrecision = OUT_STROKE_PRECIS;
	//lf.lfClipPrecision=2;
	//lf.lfQuality=1;
	//lf.lfPitchAndFamily=2;

	// Create font
	HFONT hfont=(HFONT)GetStockObject(DEFAULT_GUI_FONT);
	if (hfont!=NULL)
	{
		::GetObject(hfont, sizeof(LOGFONT), &lf);
		::DeleteObject(hfont);
	}
	m_ftTab.CreateFontIndirect(&lf);
	
	m_nDrawType = BTC_ALL;

	EnableDraw(m_nDrawType);
}

CBaseTabCtrl::~CBaseTabCtrl()
{
	if (m_ftTab.GetSafeHandle())
		m_ftTab.DeleteObject();
}


BEGIN_MESSAGE_MAP(CBaseTabCtrl, CTabCtrl)
	//{{AFX_MSG_MAP(CBaseTabCtrl)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBaseTabCtrl message handlers

void CBaseTabCtrl::SetBkgndColor(COLORREF color)
{
	// set new color
	m_crTabBKNormal = color;

	// redraw
	if (GetSafeHwnd())
		Invalidate();
}

BOOL CBaseTabCtrl::EnableDraw(int nType)
{
	ASSERT (nType >= BTC_NONE && nType <= BTC_ALL);

	if (nType < BTC_NONE || nType > BTC_ALL)
		return FALSE;

	m_nDrawType = nType;

	if (GetSafeHwnd())
	{
		if (m_nDrawType != BTC_NONE) 
			ModifyStyle(0, TCS_OWNERDRAWFIXED);
		else
			ModifyStyle(TCS_OWNERDRAWFIXED, 0);

		Invalidate();
	}

	return TRUE;
}

void CBaseTabCtrl::DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/)
{
	ASSERT (0);
}

void CBaseTabCtrl::DrawItemBorder(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/)
{
	ASSERT (0);
}

void CBaseTabCtrl::DrawMainBorder(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/)
{
	ASSERT (0);
}

COLORREF CBaseTabCtrl::GetTabColor(BOOL bSelected)
{
	//ASSERT(0);
	return 0;
}

COLORREF CBaseTabCtrl::GetTabTextColor(BOOL bSelected)
{
	//ASSERT(0);
	return 0;
}

void CBaseTabCtrl::PreSubclassWindow() 
{
	CTabCtrl::PreSubclassWindow();

	if (m_nDrawType != BTC_NONE) 
		ModifyStyle(0, TCS_OWNERDRAWFIXED);
}

BOOL CBaseTabCtrl::OnEraseBkgnd(CDC* pDC)
{
	CRect rClient, rTab, rTotalTab, rBkgnd, rEdge;
	COLORREF crBack;
	int nTab, nTabHeight = 0;

	// calc total tab width
	GetClientRect(rClient);
	nTab = GetItemCount();
	rTotalTab.SetRectEmpty();

	// draw background
	//pDC->FillSolidRect(rClient,RGB(53,53,57));

	while (nTab--)
	{
		GetItemRect(nTab, rTab);
		rTotalTab.UnionRect(rTab, rTotalTab);
	}

	nTabHeight = rTotalTab.Height();

	// add a bit
	//rTotalTab.InflateRect(0, -2);
	rTotalTab.top -= 2;
	rEdge = rTotalTab;

	// then if background color is set, paint the visible background
	// area of the tabs in the bkgnd color
	// note: the mfc code for drawing the tabs makes all sorts of assumptions
	// about the background color of the tab control being the same as the page
	// color - in some places the background color shows thru' the pages!!
	// so we must only paint the background color where we need to, which is that
	// portion of the tab area not excluded by the tabs themselves
	crBack = (m_crTabBKNormal == -1) ? ::GetSysColor(COLOR_3DFACE) : m_crTabBKNormal;
	
	// full width of tab ctrl above top of tabs
	/*rBkgnd = rClient;
	rBkgnd.top = rTotalTab.bottom-3;
	rBkgnd.right = 1;
	pDC->SetBkColor(crBack);
	pDC->ExtTextOut(rBkgnd.left, rBkgnd.top, ETO_CLIPPED | ETO_OPAQUE, rBkgnd, "", NULL);*/
	
	// width of tab ctrl visible bkgnd including bottom pixel of tabs to left of tabs
	pDC->SetBkColor(crBack);
	rBkgnd = rClient;
	rBkgnd.top = rTotalTab.bottom;
	pDC->ExtTextOut(rBkgnd.left, rBkgnd.top, ETO_CLIPPED | ETO_OPAQUE, rBkgnd, "", NULL);
	
	// to right of tabs
	/*rBkgnd = rClient;
	rBkgnd.top = rClient.bottom-3;
	rBkgnd.left = rClient.right - 1;
	pDC->ExtTextOut(rBkgnd.left, rBkgnd.top, ETO_CLIPPED | ETO_OPAQUE, rBkgnd, "", NULL);*/

	return TRUE;

	CTabCtrl::OnEraseBkgnd(pDC);
}

void CBaseTabCtrl::OnPaint() 
{
	if (m_nDrawType == BTC_NONE)
	{
		Default();
	}
	else if (m_nDrawType == BTC_TABS)
	{
		ASSERT (GetStyle() & TCS_OWNERDRAWFIXED);
		Default();
	}
	else // all
	{
		CPaintDC dc(this); // device context for painting

		// prepare dc
		dc.SelectObject(GetFont());

		DRAWITEMSTRUCT dis;
		dis.CtlType = ODT_TAB;
		dis.CtlID = GetDlgCtrlID();
		dis.hwndItem = GetSafeHwnd();
		dis.hDC = dc.GetSafeHdc();
		dis.itemAction = ODA_DRAWENTIRE;

		// draw the rest of the border
		CRect rClient, rPage;
		GetClientRect(&dis.rcItem);
		rPage = dis.rcItem;
		AdjustRect(FALSE, rPage);
		dis.rcItem.top = rPage.top;

		DrawMainBorder(&dis);

		// paint the tabs first and then the borders
		int nTab = GetItemCount();
		int nSel = GetCurSel();

		if (!nTab) // no pages added
			return;

		while (nTab--)
		{
			if (nTab != nSel)
			{
				dis.itemID = nTab;
				dis.itemState = 0;

				VERIFY(GetItemRect(nTab, &dis.rcItem));

				//dis.rcItem.bottom -= 2;
				DrawItem(&dis);
				DrawItemBorder(&dis);
			}
		}

		// now selected tab
		dis.itemID = nSel;
		dis.itemState = ODS_SELECTED;

		VERIFY(GetItemRect(nSel, &dis.rcItem));

		/*dis.rcItem.bottom += 2;
		dis.rcItem.top -= 2;*/
		DrawItem(&dis);
		DrawItemBorder(&dis);
	}
}

void CBaseTabCtrl::SetColor(COLORREF colTabBKNormal,COLORREF colTabBKFocus,COLORREF colBDNormal,COLORREF colBDFocus,COLORREF colTextNormal,COLORREF colTextFocus)
{
	m_crTabBKNormal = colTabBKNormal;
	m_crTabBKFocus = colTabBKFocus;
	m_crBDNormal = colBDNormal;
	m_crBDFocus = colBDFocus;
	m_crTextNormal = colTextNormal;
	m_crTextFocus = colTextFocus;
}

void CBaseTabCtrl::SetTextFont(CString strlfFaceName, int nlfHeight, int nlfWeight)
{
	if (m_ftTab.GetSafeHandle())
		m_ftTab.DeleteObject();
	LOGFONT lf;

	strcpy(lf.lfFaceName,strlfFaceName);//Tahoma
	lf.lfHeight=nlfHeight;
	lf.lfWidth=0;
	lf.lfEscapement = 0;
	lf.lfOrientation = 0;
	lf.lfItalic = 0;
	lf.lfUnderline = 0;
	lf.lfStrikeOut = 0;
	lf.lfWeight = nlfWeight;
	lf.lfCharSet = ANSI_CHARSET;
	lf.lfOutPrecision = OUT_STROKE_PRECIS;
	lf.lfClipPrecision=2;
	lf.lfQuality=1;
	lf.lfPitchAndFamily=2;

	m_ftTab.CreateFontIndirect(&lf);
}