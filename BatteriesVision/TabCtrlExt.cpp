//_ **********************************************************
//_ 
//_ TabCtrlExt.cpp 
//_ 
//_ **********************************************************

// EnTabCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "TabCtrlExt.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTabCtrlExt

DWORD CTabCtrlExt::s_dwCustomLook = 0;

enum { PADDING = 3, EDGE = 20};

//////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////

CTabCtrlExt::CTabCtrlExt()
{
	s_dwCustomLook = s_dwCustomLook | ETC_FLAT | ETC_SELECTION;
}

CTabCtrlExt::~CTabCtrlExt()
{
}

/////////////////////////////////////////////////////////////////////////////
// CTabCtrlExt message handlers

void CTabCtrlExt::DrawItem(LPDRAWITEMSTRUCT lpdis)
{
	TC_ITEM     tci;
	CDC* pDC = CDC::FromHandle(lpdis->hDC);
	HIMAGELIST hilTabs = (HIMAGELIST)TabCtrl_GetImageList(GetSafeHwnd());

	BOOL bSelected = (lpdis->itemID == (UINT)GetCurSel());
	//BOOL bColor = (s_dwCustomLook & ETC_COLOR);

	CRect rItem(lpdis->rcItem);
	rItem.top -= 2;

	// tab
	// blend from back color to COLOR_3DFACE if 16 bit mode or better
	COLORREF crFrom = GetTabColor(bSelected);
	pDC->FillSolidRect(rItem, crFrom);

	// text & icon
	//rItem.left += PADDING;
	//rItem.top += PADDING;

	pDC->SetBkMode(TRANSPARENT);

	CString sTemp;
	tci.mask        = TCIF_TEXT | TCIF_IMAGE;
	tci.pszText     = sTemp.GetBuffer(100);
	tci.cchTextMax  = 99;
	GetItem(lpdis->itemID, &tci);
	sTemp.ReleaseBuffer();

	// icon
	if (hilTabs)
	{
		ImageList_Draw(hilTabs, tci.iImage, *pDC, rItem.left, rItem.top, ILD_TRANSPARENT);
		rItem.left += 16 + PADDING;
	}

	CFont *pFtOld;
	pFtOld = pDC->SelectObject(&m_ftTab);

	// text
	//rItem.right -= PADDING;
	//FormatText(sTemp, pDC, rItem.Width());

	pDC->SetTextColor(GetTabTextColor(bSelected));
	pDC->DrawText(sTemp, rItem, DT_NOPREFIX | DT_SINGLELINE|DT_VCENTER|DT_CENTER);

	pDC->SelectObject(pFtOld);
}

void CTabCtrlExt::DrawItemBorder(LPDRAWITEMSTRUCT lpdis)
{
	//ASSERT (s_dwCustomLook & ETC_FLAT);

	BOOL bSelected = (lpdis->itemID == (UINT)GetCurSel());
	//BOOL bBackTabs = (s_dwCustomLook & ETC_BACKTABS);

	CRect rItem(lpdis->rcItem);
	CDC* pDC = CDC::FromHandle(lpdis->hDC); 

	CPen* oldObj;

	if (bSelected /*|| bBackTabs*/)
	{
		//rItem.bottom += bSelected ? -1 : 1;
		//rItem.bottom -=2;

		// edges
		//pDC->FillSolidRect(CRect(rItem.left, rItem.top, rItem.left + 1, rItem.bottom), crHighlight);
		//pDC->FillSolidRect(CRect(rItem.left, rItem.top, rItem.right, rItem.top + 1), crHighlight);
		//pDC->FillSolidRect(CRect(rItem.right - 1, rItem.top, rItem.right, rItem.bottom), crShadow);

		//pDC->FillSolidRect(CRect(rItem.left, rItem.bottom-4, rItem.right, rItem.bottom-1), m_crBDFocus);
		CPen p1(PS_SOLID,1,m_crBDFocus);
		oldObj = pDC->SelectObject(&p1);
		if (lpdis->itemID!=0 && lpdis->itemID!=GetItemCount()-1)
		{
			pDC->MoveTo(rItem.left,rItem.top+rItem.bottom/4);
			pDC->LineTo(rItem.left,rItem.bottom/4*3);
		}
		if (lpdis->itemID!=GetItemCount()-1)
		{
			pDC->MoveTo(rItem.right-1,rItem.top+rItem.bottom/4);
			pDC->LineTo(rItem.right-1,rItem.bottom/4*3);
		}
		pDC->SelectObject(oldObj);
	}
	else // draw simple dividers
	{
		//rItem.bottom +=4;
		//pDC->FillSolidRect(CRect(rItem.left - 1, rItem.top, rItem.left, rItem.bottom), crShadow);
		//pDC->FillSolidRect(CRect(rItem.right - 1, rItem.top, rItem.right, rItem.bottom), crShadow);
		//pDC->FillSolidRect(CRect(rItem.left, rItem.bottom-1, rItem.right, rItem.bottom), RGB(200,200,200));

		CPen p1(PS_SOLID,1,m_crBDNormal);
		oldObj = pDC->SelectObject(&p1);
		if (lpdis->itemID!=0)
		{
			pDC->MoveTo(rItem.left,rItem.top+rItem.bottom/4);
			pDC->LineTo(rItem.left,rItem.bottom/4*3);
		}
		if (lpdis->itemID!=GetItemCount()-1)
		{
			pDC->MoveTo(rItem.right-1,rItem.top+rItem.bottom/4);
			pDC->LineTo(rItem.right-1,rItem.bottom/4*3);
		}
		pDC->SelectObject(oldObj);
	}
}

void CTabCtrlExt::DrawMainBorder(LPDRAWITEMSTRUCT lpdis)
{
	return ;
	CRect rBorder(lpdis->rcItem);
	CDC* pDC = CDC::FromHandle(lpdis->hDC); 

	COLORREF crTab = GetTabColor();

	CRect rcItem;
	GetItemRect(0,rcItem);
	rcItem.bottom;
	pDC->FillSolidRect(CRect(rBorder.left, rcItem.bottom-1, rBorder.right, rcItem.bottom), m_crBDNormal);
}

COLORREF CTabCtrlExt::GetTabColor(BOOL bSelected)
{
	if (bSelected)
	{
		return m_crTabBKFocus;
	}else
	{
		return m_crTabBKNormal;
	}
}

COLORREF CTabCtrlExt::GetTabTextColor(BOOL bSelected)
{
	if (bSelected)
	{
		return m_crTextFocus;
	}
	else
	{
		return m_crTextNormal;
	}
}

void CTabCtrlExt::EnableCustomLook(BOOL bEnable, DWORD dwStyle)
{
	if (!bEnable)
		dwStyle = 0;

	s_dwCustomLook = dwStyle;
}

void CTabCtrlExt::PreSubclassWindow() 
{
	CBaseTabCtrl::PreSubclassWindow();

	if (s_dwCustomLook)
		ModifyStyle(0, TCS_OWNERDRAWFIXED);
}

CSize CTabCtrlExt::FormatText(CString& sText, CDC* pDC, int nWidth)
{
	CRect rect(0, 0, nWidth, 20);
	UINT uFlags = DT_CALCRECT | DT_SINGLELINE | DT_MODIFYSTRING | DT_END_ELLIPSIS;

	::DrawText(pDC->GetSafeHdc(), sText.GetBuffer(sText.GetLength() + 4), -1, rect, uFlags);
	sText.ReleaseBuffer();

	return pDC->GetTextExtent(sText);
}

void CTabCtrlExt::SetBkgndColor(COLORREF color)
{
	CBaseTabCtrl::SetBkgndColor(color);
}

void CTabCtrlExt::SetColor(COLORREF colTabBKNormal, COLORREF colTabBKFocus, COLORREF colBDNormal,
	COLORREF colBDFocus, COLORREF colTextNormal, COLORREF colTextFocus)
{
	CBaseTabCtrl::SetColor(colTabBKNormal,colTabBKFocus,colBDNormal,
		colBDFocus,colTextNormal,colTextFocus);
}

void CTabCtrlExt::SetTextFont(CString strlfFaceName, int nlfHeight, int nlfWeight)
{
	CBaseTabCtrl::SetTextFont(strlfFaceName, nlfHeight, nlfWeight);
}