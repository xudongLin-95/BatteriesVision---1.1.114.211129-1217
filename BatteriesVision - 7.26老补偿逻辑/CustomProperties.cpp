//*******************************************************************************
// COPYRIGHT NOTES
// ---------------
// This is a sample for BCGControlBar Library Professional Edition
// Copyright (C) 1998-2018 BCGSoft Ltd.
// All rights reserved.
//
// This source code can be used, distributed or modified
// only under terms and conditions 
// of the accompanying license agreement.
//*******************************************************************************
//
#include "stdafx.h"
#include "CustomProperties.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

////////////////////////////////////////////////////////////////////////////////
// CMyDurationCtrl

BEGIN_MESSAGE_MAP(CMyDurationCtrl, CBCGPDurationCtrl)
	ON_WM_KILLFOCUS()
END_MESSAGE_MAP()

void CMyDurationCtrl::OnKillFocus(CWnd* pNewWnd)
{
	CBCGPDurationCtrl::OnKillFocus(pNewWnd);

	CBCGPPropList* pParent = DYNAMIC_DOWNCAST(CBCGPPropList, GetParent());
	if (pParent == NULL || pNewWnd->GetSafeHwnd() == pParent->GetSafeHwnd())
	{
		return;
	}

	CBCGPProp* pSel = pParent->GetCurSel();
	if (pSel != NULL && pSel->IsInPlaceEditing() && pSel->IsEnabled())
	{
		if (!pParent->EndEditItem ())
		{
			SetFocus();
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
// CDurationProp class

CDurationProp::CDurationProp(const CString& strName, const COleDateTimeSpan& duration, 
		LPCTSTR lpszDescr/* = NULL*/, DWORD_PTR dwData/* = 0*/) :
	CBCGPProp (strName, _variant_t((double)duration), lpszDescr, dwData)
{
}
//****************************************************************************************
void CDurationProp::OnDrawValue (CDC* pDC, CRect rect)
{
	ASSERT_VALID (m_pWndList);

	rect.DeflateRect (1, 2);

	rect.left = m_pWndList->GetListRect ().left + 
				m_pWndList->GetPropertyColumnWidth () + TEXT_MARGIN + 1;

	DRAWITEMSTRUCT dis;
	memset (&dis, 0, sizeof (DRAWITEMSTRUCT));

	dis.CtlType = ODT_BUTTON;
	dis.hDC = pDC->GetSafeHdc ();
	dis.rcItem = rect;

	COLORREF clrTextOld = pDC->GetTextColor ();

	m_wndDuration.SetTextColor (m_bEnabled ? m_pWndList->GetTextColor () : globalData.clrGrayedText, FALSE);

	COLORREF clrFill = GetAlternateColor();
	if (clrFill == CLR_NONE)
	{
		clrFill = m_pWndList->GetBkColor();
	}

	m_wndDuration.SetBackgroundColor (clrFill, FALSE);

	m_wndDuration.SetFont (
		IsModified () && m_pWndList->IsMarkModifiedProperties () ? 
			&m_pWndList->GetBoldFont () : m_pWndList->GetFont ());

	m_wndDuration.DrawItem (&dis);
	pDC->SetTextColor (clrTextOld);
}
//*****************************************************************************
CWnd* CDurationProp::CreateInPlaceEdit (CRect rectEdit, BOOL& bDefaultFormat)
{
	ASSERT_VALID (m_pWndList);

	CMyDurationCtrl* pDurationCtrl = new CMyDurationCtrl;
	ASSERT_VALID (pDurationCtrl);

	pDurationCtrl->SetTextColor (m_bEnabled ? m_pWndList->GetTextColor () : globalData.clrGrayedText, FALSE);
	pDurationCtrl->SetBackgroundColor (m_pWndList->GetBkColor (), FALSE);

	pDurationCtrl->SetAutoResize (FALSE);

	pDurationCtrl->EnableVisualManagerStyle();

	CRect rectSpin;
	AdjustInPlaceEditRect (rectEdit, rectSpin);

	rectEdit.DeflateRect(1, 1);

	pDurationCtrl->Create (_T(""), WS_CHILD | WS_VISIBLE, rectEdit, 
		m_pWndList, BCGPROPLIST_ID_INPLACE);
	pDurationCtrl->SetFont (IsModified () && m_pWndList->IsMarkModifiedProperties () ? 
			&m_pWndList->GetBoldFont () : m_pWndList->GetFont ());

	pDurationCtrl->SetState (CBCGPDurationCtrl::DRTN_DAYS | CBCGPDurationCtrl::DRTN_HOURS_MINS, 
		CBCGPDurationCtrl::DRTN_DAYS | CBCGPDurationCtrl::DRTN_HOURS_MINS);
	pDurationCtrl->SetDuration (GetDuration ());

	pDurationCtrl->EnableWindow(m_bEnabled);

	bDefaultFormat = FALSE;

	return pDurationCtrl;
}
//*******************************************************************************
void CDurationProp::OnPosSizeChanged (CRect /*rectOld*/)
{
	ASSERT_VALID (m_pWndList);

	CRect rectEdit;
	CRect rectSpin;

	AdjustInPlaceEditRect (rectEdit, rectSpin);

	if (m_wndDuration.GetSafeHwnd () == NULL)
	{
		m_wndDuration.SetAutoResize (FALSE);
		m_wndDuration.Create (_T(""), WS_CHILD, rectEdit,
								m_pWndList, (UINT)-1);

		m_wndDuration.SetState (
			CBCGPDurationCtrl::DRTN_DAYS | CBCGPDurationCtrl::DRTN_HOURS_MINS, 
			CBCGPDurationCtrl::DRTN_DAYS | CBCGPDurationCtrl::DRTN_HOURS_MINS);
		m_wndDuration.SetFont (
			IsModified () && m_pWndList->IsMarkModifiedProperties () ? 
				&m_pWndList->GetBoldFont () : m_pWndList->GetFont ());
		m_wndDuration.SetDuration (GetDuration ());
	}
	else
	{
		m_wndDuration.SetFont (
			IsModified () && m_pWndList->IsMarkModifiedProperties () ? 
				&m_pWndList->GetBoldFont () : m_pWndList->GetFont ());

		m_wndDuration.SetWindowPos (NULL, rectEdit.left, rectEdit.top,
			rectEdit.Width (), rectEdit.Height (),
			SWP_NOZORDER | SWP_NOACTIVATE);
	}

	m_wndDuration.AdjustControl (rectEdit);
}
//********************************************************************************
void CDurationProp::AdjustInPlaceEditRect (CRect& rectEdit, CRect& rectSpin)
{
	rectSpin.SetRectEmpty ();

	rectEdit = m_Rect;

	rectEdit.left = m_pWndList->GetListRect().left + m_pWndList->GetPropertyColumnWidth();
}
//******************************************************************************************
BOOL CDurationProp::OnUpdateValue ()
{
	ASSERT_VALID (this);
	ASSERT_VALID (m_pWndInPlace);
	ASSERT_VALID (m_pWndList);
	ASSERT (::IsWindow (m_pWndInPlace->GetSafeHwnd ()));

	CBCGPDurationCtrl* pDurationCtrl = (CBCGPDurationCtrl*) m_pWndInPlace;
	ASSERT_VALID (pDurationCtrl);

	COleDateTimeSpan durationOld = GetDuration ();
	COleDateTimeSpan durationNew = pDurationCtrl->GetDuration ();

	if (durationNew.GetDays() > 2)
	{
		CRect rect;
		m_pWndInPlace->GetWindowRect(rect);

		CBCGPPopupWindow* pPopup = new CBCGPPopupWindow();
		
		pPopup->SetStemLocation(CBCGPPopupWindow::BCGPPopupWindowStemLocation_TopLeft);
		pPopup->SetSmallCaptionGripper(FALSE);
		pPopup->EnableCloseButton(FALSE);
		pPopup->SetShadow();
		pPopup->SetRoundedCorners();

		CBCGPPopupWndParams params;
		params.m_strText = _T("The duration should not exceed 2 days. Please correct the entered duration value.");

		pPopup->Create(m_pWndList, params, NULL, CPoint(rect.left + 5, rect.bottom + 5));

		m_pWndInPlace->SetFocus();

		return FALSE;
	}

	m_varValue = _variant_t ((double) durationNew);
	m_wndDuration.SetDuration (durationNew);

	if (durationOld != durationNew)
	{
		m_pWndList->OnPropertyChanged (this);
	}

	return TRUE;
}
//********************************************************************************
void CDurationProp::SetDuration (COleDateTimeSpan duration)
{
	ASSERT_VALID (this);
	SetValue (_variant_t ((double) duration));
}
//********************************************************************************
void CDurationProp::OnSetSelection (CBCGPProp* /*pOldSel*/)
{
	Redraw ();
}
//********************************************************************************
void CDurationProp::OnKillSelection (CBCGPProp* /*pNewSel*/)
{
	Redraw ();
}
//********************************************************************************
BOOL CDurationProp::PushChar (UINT nChar)
{
	ASSERT_VALID (this);
	ASSERT_VALID (m_pWndList);
	ASSERT (m_pWndList->GetCurSel () == this);
	ASSERT_VALID (m_pWndInPlace);

	if (m_bEnabled && m_bAllowEdit)
	{
		CString str;
		str = (TCHAR) nChar;
		str.MakeUpper ();

		m_pWndInPlace->SendMessage (WM_KEYDOWN, (WPARAM) str [0]);
		return TRUE;
	}

	OnEndEdit ();
	
	if (::GetCapture () == m_pWndList->GetSafeHwnd ())
	{
		ReleaseCapture ();
	}

	return FALSE;
}
//********************************************************************************
BOOL CDurationProp::DoPaste()
{
	if (!CBCGPProp::DoPaste())
	{
		return FALSE;
	}

	if (m_wndDuration.GetSafeHwnd() != NULL)
	{
		m_wndDuration.SetDuration(GetDuration ());
		Redraw();
	}

	return TRUE;
}
//********************************************************************************
void CDurationProp::SetValue (const _variant_t& varValue)
{
	ASSERT_VALID (this);

	CBCGPProp::SetValue(varValue);

	if (m_wndDuration.GetSafeHwnd() != NULL)
	{
		m_wndDuration.SetDuration(GetDuration ());
	}
}

////////////////////////////////////////////////////////////////////////////////
// CPasswordProp class

const TCHAR cPassword = _T('*');

CPasswordProp::CPasswordProp(const CString& strName, const CString& strPassword, 
							 LPCTSTR lpszDescr, DWORD_PTR dwData)
	:	CBCGPProp (strName, (LPCTSTR) strPassword, lpszDescr, dwData)
{
}
//************************************************************************************
CWnd* CPasswordProp::CreateInPlaceEdit (CRect rectEdit, BOOL& bDefaultFormat)
{
	CEdit* pWndEdit = new CEdit;

	DWORD dwStyle = WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL | ES_PASSWORD;

	if (!m_bEnabled || !m_bAllowEdit)
	{
		dwStyle |= ES_READONLY;
	}

	pWndEdit->Create (dwStyle, rectEdit, m_pWndList, BCGPROPLIST_ID_INPLACE);
	pWndEdit->SetPasswordChar (cPassword);

	bDefaultFormat = TRUE;
	return pWndEdit;
}
//************************************************************************************
CString CPasswordProp::FormatProperty ()
{
	CString strVal = (LPCTSTR)(_bstr_t)m_varValue;
	
	for (int i = 0; i < strVal.GetLength (); i++)
	{
		strVal.SetAt (i, cPassword);
	}

	return strVal;
}

/////////////////////////////////////////////////////////////////////////////
// CPropSliderCtrl

CPropSliderCtrl::CPropSliderCtrl(CSliderProp* pProp, COLORREF clrBack)
{
	m_clrBack = clrBack;
	m_brBackground.CreateSolidBrush (m_clrBack);
	m_pProp = pProp;
	m_bVisualManagerStyle = TRUE;
	m_bDrawFocus = FALSE;
}

CPropSliderCtrl::~CPropSliderCtrl()
{
}


BEGIN_MESSAGE_MAP(CPropSliderCtrl, CBCGPSliderCtrl)
	//{{AFX_MSG_MAP(CPropSliderCtrl)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_HSCROLL_REFLECT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropSliderCtrl message handlers

HBRUSH CPropSliderCtrl::CtlColor(CDC* pDC, UINT /*nCtlColor*/) 
{
	pDC->SetBkColor (m_clrBack);
	return m_brBackground;
}
//************************************************************************************
void CPropSliderCtrl::HScroll (UINT /*nSBCode*/, UINT /*nPos*/)
{
	ASSERT_VALID (m_pProp);

	m_pProp->OnUpdateValue ();
	m_pProp->Redraw ();
}
//************************************************************************************
void CPropSliderCtrl::OnDrawThumb(CDC* pDC, CRect rectThumb, BOOL /*bIsHighlighted*/, BOOL /*bIsPressed*/, BOOL /*bIsDisabled*/, BOOL /*bVert*/, BOOL /*bLeftTop*/, BOOL /*bRightBottom*/, BOOL /*bDrawOnGlass*/)
{
	pDC->FillRect(rectThumb, &globalData.brBarFace);
	pDC->Draw3dRect(rectThumb, globalData.clrBarDkShadow, globalData.clrBarDkShadow);
}

////////////////////////////////////////////////////////////////////////////////
// CSliderProp class

CSliderProp::CSliderProp(const CString& strName, long nValue, 
							 LPCTSTR lpszDescr, DWORD_PTR dwData)
	:	CBCGPProp (strName, nValue, lpszDescr, dwData)
{
}
//************************************************************************************
CWnd* CSliderProp::CreateInPlaceEdit (CRect rectEdit, BOOL& bDefaultFormat)
{
	ASSERT_VALID (this);
	ASSERT_VALID (m_pWndList);

	CPropSliderCtrl* pWndSlider = new CPropSliderCtrl (this, m_pWndList->GetBkColor ());

	// Calculate label width:
	CClientDC dc(m_pWndList);
	CBCGPFontSelector fs(dc, m_pWndList->GetFont());

	CString strLabel(_T("0000"));
	rectEdit.left += dc.GetTextExtent (strLabel).cx;

	pWndSlider->Create (WS_VISIBLE | WS_CHILD | TBS_NOTICKS, rectEdit, m_pWndList, BCGPROPLIST_ID_INPLACE);
	pWndSlider->SetPos ((long) m_varValue);
	pWndSlider->EnableWindow(m_bEnabled);

	bDefaultFormat = TRUE;
	return pWndSlider;
}
//******************************************************************************************
BOOL CSliderProp::OnUpdateValue ()
{
	ASSERT_VALID (this);
	ASSERT_VALID (m_pWndInPlace);
	ASSERT_VALID (m_pWndList);
	ASSERT (::IsWindow (m_pWndInPlace->GetSafeHwnd ()));

	long lCurrValue = m_varValue;

	CBCGPSliderCtrl* pSlider = (CBCGPSliderCtrl*) m_pWndInPlace;

	m_varValue = (long) pSlider->GetPos ();

	if (lCurrValue != (long) m_varValue)
	{
		m_pWndList->OnPropertyChanged (this);
	}

	return TRUE;
}
//******************************************************************************************
BOOL CSliderProp::OnClickValue (UINT /*uiMsg*/, CPoint /*point*/)
{
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// CIconListProp class

#define ICON_PADDING globalUtils.ScaleByDPI(3)

CIconListProp::CIconListProp(const CString& strName, 
							CBCGPToolBarImages& imageListIcons,
							int nSelectedIcon,
							CStringList* plstIconNames,
							LPCTSTR lpszDescr, DWORD_PTR dwData)
	:	CBCGPProp (strName, (long) nSelectedIcon, lpszDescr, dwData),
		m_imageListIcons(imageListIcons)
{
	if (plstIconNames != NULL)
	{
		m_lstIconNames.AddTail (plstIconNames);
		ASSERT (m_lstIconNames.GetCount () == m_imageListIcons.GetCount ());
	}

	for (int i = 0; i < m_imageListIcons.GetCount (); i++)
	{
		CString strItem;
		strItem.Format (_T("%d"), i);

		AddOption (strItem);
	}

	AllowEdit (FALSE);
}
//*******************************************************************************
CComboBox* CIconListProp::CreateCombo (CWnd* pWndParent, CRect rect)
{
	rect.bottom = rect.top + 400;

	CIconCombo* pWndCombo = new CIconCombo (m_imageListIcons, m_lstIconNames);
	if (!pWndCombo->Create (WS_CHILD | CBS_NOINTEGRALHEIGHT | CBS_DROPDOWNLIST | WS_VSCROLL | CBS_OWNERDRAWFIXED | CBS_HASSTRINGS,
		rect, pWndParent, BCGPROPLIST_ID_INPLACE_COMBO))
	{
		delete pWndCombo;
		return NULL;
	}

	return pWndCombo;
}
//*******************************************************************************
void CIconListProp::OnDrawValue (CDC* pDC, CRect rect)
{
	ASSERT_VALID (m_pWndList);
	ASSERT_VALID (pDC);

	CString strVal = (LPCTSTR)(_bstr_t) m_varValue;
	if (strVal.IsEmpty () || m_imageListIcons.GetCount () == 0)
	{
		return;
	}
	
	int nIndex = -1;
#if _MSC_VER < 1400
	_stscanf (strVal, _T("%d"), &nIndex);
#else
	_stscanf_s (strVal, _T("%d"), &nIndex);
#endif

	if (nIndex < 0)
	{
		return;
	}

	COLORREF clrTextOld = pDC->GetTextColor ();

	CFont* pOldFont = pDC->SelectObject (
			IsModified () && m_pWndList->IsMarkModifiedProperties () ? 
				&m_pWndList->GetBoldFont () : m_pWndList->GetFont ());

	CRect rectImage = rect;
	rectImage.left += ICON_PADDING;
	rectImage.right = rectImage.left + m_imageListIcons.GetImageSize().cx + ICON_PADDING;

	m_imageListIcons.DrawEx(pDC, rectImage, nIndex, CBCGPToolBarImages::ImageAlignHorzCenter, CBCGPToolBarImages::ImageAlignVertCenter);

	if (!m_lstIconNames.IsEmpty ())
	{
		CString str = m_lstIconNames.GetAt (m_lstIconNames.FindIndex (nIndex));

		rect.left = rectImage.right + 2 * ICON_PADDING;

		pDC->DrawText (str, rect, DT_SINGLELINE | DT_VCENTER);
	}

	pDC->SetTextColor (clrTextOld);
	pDC->SelectObject (pOldFont);

	m_bValueIsTrancated = FALSE;
}
//*******************************************************************************
CWnd* CIconListProp::CreateInPlaceEdit (CRect rectEdit, BOOL& bDefaultFormat)
{
	CWnd* pWnd = CBCGPProp::CreateInPlaceEdit (rectEdit, bDefaultFormat);
	if (pWnd != NULL)
	{
		pWnd->ShowWindow (SW_HIDE);
	}

	return pWnd;
}

/////////////////////////////////////////////////////////////////////////////
// CIconCombo

CIconCombo::CIconCombo(CBCGPToolBarImages& imageListIcons, CStringList& lstIconNames) :
	m_imageListIcons (imageListIcons),
	m_lstIconNames (lstIconNames)
{
	m_bVisualManagerStyle = TRUE;
}

CIconCombo::~CIconCombo()
{
}

BEGIN_MESSAGE_MAP(CIconCombo, CBCGPComboBox)
	//{{AFX_MSG_MAP(CIconCombo)
	ON_WM_DRAWITEM()
	ON_WM_MEASUREITEM()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIconCombo message handlers

void CIconCombo::OnDrawItem(int /*nIDCtl*/, LPDRAWITEMSTRUCT lpDIS) 
{
	CDC* pDC = CDC::FromHandle (lpDIS->hDC);
	ASSERT_VALID (pDC);

	CRect rect = lpDIS->rcItem;
	int nIcon = lpDIS->itemID;

	COLORREF clrText = OnFillLbItem(pDC, nIcon, rect, lpDIS->itemState & ODS_SELECTED, lpDIS->itemState & ODS_SELECTED); 

	if (nIcon < 0)
	{
		return;
	}

	rect.left += ICON_PADDING;

	m_imageListIcons.DrawEx(pDC, rect, nIcon, CBCGPToolBarImages::ImageAlignHorzLeft, CBCGPToolBarImages::ImageAlignVertCenter);

	if (!m_lstIconNames.IsEmpty ())
	{
		CString str = m_lstIconNames.GetAt (m_lstIconNames.FindIndex (nIcon));
		CFont* pOldFont = pDC->SelectObject (&globalData.fontRegular);

		pDC->SetBkMode (TRANSPARENT);
		pDC->SetTextColor (clrText);

		CRect rectText = rect;
		rectText.left += m_imageListIcons.GetImageSize ().cx + ICON_PADDING;

		pDC->DrawText (str, rectText, DT_SINGLELINE | DT_VCENTER);

		pDC->SelectObject (pOldFont);
	}
}
//*******************************************************************************
void CIconCombo::OnMeasureItem(int /*nIDCtl*/, LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
	int nTextWidth = 0;
	int nTextHeight = 0;

	if (!m_lstIconNames.IsEmpty ())
	{
		nTextHeight = globalData.GetTextHeight ();

		CClientDC dc (this);
		CFont* pOldFont = dc.SelectObject (&globalData.fontRegular);

		for (POSITION pos = m_lstIconNames.GetHeadPosition (); pos != NULL;)
		{
			CString str = m_lstIconNames.GetNext (pos);

			nTextWidth = max (nTextWidth, dc.GetTextExtent (str).cx + ICON_PADDING);
		}

		dc.SelectObject (pOldFont);
	}

	lpMeasureItemStruct->itemWidth = m_imageListIcons.GetImageSize ().cx + nTextWidth + 2 * ICON_PADDING;
	lpMeasureItemStruct->itemHeight = max (nTextHeight, m_imageListIcons.GetImageSize ().cy + 2 * ICON_PADDING);
}

////////////////////////////////////////////////////////////////////////////////
// CComboBoxExProp class

CComboBoxExProp::CComboBoxExProp(const CString& strName, 
							const CString& strValue,
							LPCTSTR lpszDescr, DWORD_PTR dwData,
							CBCGPToolBarImages* pImageList)
	:	CBCGPProp (strName, (LPCTSTR) strValue, lpszDescr, dwData)
{
	if (pImageList != NULL)
	{
		pImageList->ExportToImageList(m_ImageList);
	}
}
//*******************************************************************************
CComboBox* CComboBoxExProp::CreateCombo (CWnd* pWndParent, CRect rect)
{
	rect.bottom = rect.top + 400;

	CComboBoxEx* pWndCombo = new CComboBoxEx;

	if (!pWndCombo->Create (WS_CHILD | CBS_DROPDOWNLIST | WS_VSCROLL,
		rect, pWndParent, BCGPROPLIST_ID_INPLACE_COMBO))
	{
		delete pWndCombo;
		return NULL;
	}

	if (m_ImageList.GetSafeHandle() != NULL)
	{
		pWndCombo->SetImageList(&m_ImageList);
	}

	return pWndCombo;
}
//*******************************************************************************
BOOL CComboBoxExProp::OnEdit (LPPOINT lptClick)
{
	if (!CBCGPProp::OnEdit (lptClick))
	{
		return FALSE;
	}

	CComboBoxEx* pWndComboEx = DYNAMIC_DOWNCAST (CComboBoxEx, m_pWndCombo);
	if (pWndComboEx == NULL)
	{
		ASSERT (FALSE);
		return FALSE;
	}

	pWndComboEx->ResetContent ();

	int i = 0;

	COMBOBOXEXITEM item;
	memset (&item, 0, sizeof (item));

	item.mask = CBEIF_IMAGE | CBEIF_INDENT |
				CBEIF_SELECTEDIMAGE | CBEIF_TEXT;

	for (POSITION pos = m_lstOptions.GetHeadPosition (); pos != NULL; i++)
	{
		CString strItem = m_lstOptions.GetNext (pos);

		item.iItem = i;
		item.iSelectedImage = item.iImage = m_lstIcons [i];
		item.iIndent = m_lstIndents [i];
		item.pszText = (LPTSTR)(LPCTSTR) strItem;
		item.cchTextMax = strItem.GetLength ();

		pWndComboEx->InsertItem (&item);
	}

	return TRUE;
}
//*******************************************************************************
BOOL CComboBoxExProp::AddOption (LPCTSTR lpszOption, int nIcon, int nIndent)
{
	if (!CBCGPProp::AddOption (lpszOption))
	{
		return FALSE;
	}

	m_lstIcons.Add (nIcon);
	m_lstIndents.Add (nIndent);

	return TRUE;
}
//*******************************************************************************
COwnerDrawDescrProp::COwnerDrawDescrProp (const CString& strName,
										  const _variant_t& varValue) :
	CBCGPProp (strName, varValue)
{
}
//*******************************************************************************
void COwnerDrawDescrProp::OnDrawDescription (CDC* pDC, CRect rect)
{
	CBCGPDrawManager dm (*pDC);
	dm.FillGradient2 (rect, RGB (102, 200, 238), RGB (0, 129, 185), 45);

	CFont* pOldFont = pDC->SelectObject (&globalData.fontBold);

	CString strText = _T("Custom Text");

	pDC->SetTextColor (RGB (0, 65, 117));
	pDC->DrawText (strText, rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	rect.OffsetRect (-2, -2);

	pDC->SetTextColor (RGB (155, 251, 255));
	pDC->DrawText (strText, rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	pDC->SelectObject (pOldFont);
}
//*******************************************************************************
CTwoButtonsProp::CTwoButtonsProp (const CString& strName,
								  const _variant_t& varValue) :
	CBCGPProp (strName, varValue)
{
	m_images.SetImageSize(CSize(14, 14));
	m_images.SetTransparentColor(RGB(255, 0, 255));
	//m_images.Load(IDB_BUTTONS);

	globalUtils.ScaleByDPI(m_images);

	m_iClickedButton = -1;
}
//*******************************************************************************
void CTwoButtonsProp::AdjustButtonRect ()
{
	CBCGPProp::AdjustButtonRect ();
	m_rectButton.left -= m_rectButton.Width ();
}
//*******************************************************************************
void CTwoButtonsProp::OnClickButton (CPoint point)
{
	BOOL bIsLeft = point.x < m_rectButton.CenterPoint ().x;

	m_iClickedButton = bIsLeft ? 0 : 1;
	RedrawButton();

	BCGPMessageBox (bIsLeft ? _T("Left button clicked") : _T("Right button clicked"));

	m_iClickedButton = -1;
	RedrawButton();
}
//*******************************************************************************
void CTwoButtonsProp::OnDrawButton (CDC* pDC, CRect rectButton)
{
	CSize sizeImage = m_images.GetImageSize();

	for (int i = 0; i < 2; i++)
	{
		CBCGPToolbarButton button;

		CRect rect = rectButton;

		if (i == 0)
		{
			rect.right = rect.left + rect.Width () / 2;
		}
		else
		{
			rect.left = rect.right - rect.Width () / 2;
		}

		CBCGPVisualManager::BCGBUTTON_STATE state = 
			m_iClickedButton == i ? CBCGPVisualManager::ButtonsIsHighlighted : CBCGPVisualManager::ButtonsIsRegular;

		CBCGPVisualManager::GetInstance ()->OnFillButtonInterior (pDC, &button, rect, state);
		m_images.DrawEx(pDC, rect, i, CBCGPToolBarImages::ImageAlignHorzCenter, CBCGPToolBarImages::ImageAlignVertCenter);
		CBCGPVisualManager::GetInstance ()->OnDrawButtonBorder (pDC, &button, rect, state);
	}
}
//*******************************************************************************
CCustomDlgProp::CCustomDlgProp (const CString& strName,
								const _variant_t& varValue) :
	CBCGPProp (strName, varValue)
{
}
//*******************************************************************************
void CCustomDlgProp::OnClickButton (CPoint /*point*/)
{
	BCGPMessageBox (_T("Show your dialog here..."));
	SetValue (_T("New value"));
}

////////////////////////////////////////////////////////////////////////////////
// CComboButtonProp class

CComboButtonProp::CComboButtonProp (const CString& strName,
				  const _variant_t& varValue) :
	CBCGPProp (strName, varValue)
{
}
//*******************************************************************************
#define PROP_HAS_LIST 0x0001

void CComboButtonProp::AdjustButtonRect ()
{
	CBCGPProp::AdjustButtonRect ();

	if (m_dwFlags & PROP_HAS_LIST)
	{
		m_rectButton.left -= m_rectButton.Width ();
	}
}
//*******************************************************************************
void CComboButtonProp::OnClickButton (CPoint point)
{
	BOOL bIsLeft = point.x < m_rectButton.CenterPoint ().x;
	
	if (bIsLeft)
	{
		// left - combobox
		CBCGPProp::OnClickButton (point);
	}
	else
	{
		// right - custom button

		// TODO: Add your custom code here....
		BCGPMessageBox (_T("Show your dialog here..."));
		SetValue (_T("New value"));
	}
}
//*******************************************************************************
void CComboButtonProp::OnDrawButton (CDC* pDC, CRect rectButton)
{
	int xCenter = rectButton.CenterPoint().x;

	for (int i = 0; i < 2; i++)
	{
		CRect rect = rectButton;

		if (i == 0)
		{
			if (m_dwFlags & PROP_HAS_LIST)
			{
				rect.right = xCenter - 1;
				
				// Draw combobox button at left
				CBCGPProp::OnDrawButton (pDC, rect);
			}
		}
		else
		{
			if (m_dwFlags & PROP_HAS_LIST)
			{
				rect.left = xCenter;
			}

			// Draw push button at right
			COLORREF clrText = CBCGPVisualManager::GetInstance()->OnDrawPropListPushButton(pDC, rect, this, m_pWndList->DrawControlBarColors(), m_bButtonIsFocused, m_bEnabled, m_bButtonIsDown, m_bButtonIsHighlighted);
			
			CString str = m_strButtonText;
			if (!str.IsEmpty())
			{
				COLORREF clrTextOld = pDC->SetTextColor (clrText);
				
				rect.DeflateRect(2, 2);
				
				pDC->DrawText (str, rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
				
				pDC->SetTextColor (clrTextOld);
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
// CRangeValidationProp

CRangeValidationProp::CRangeValidationProp(
					 const CString& strName,
					 long lValue,
					 LPCTSTR lpszDescr,
					 DWORD_PTR dwData) :
	CBCGPProp(strName, lValue, lpszDescr, dwData)
{
}
//*******************************************************************************
BOOL CRangeValidationProp::OnUpdateValue()
{
	ASSERT_VALID (this);
	ASSERT_VALID (m_pWndInPlace);
	ASSERT_VALID (m_pWndList);
	ASSERT (::IsWindow (m_pWndInPlace->GetSafeHwnd ()));
	
	CString strText;
	m_pWndInPlace->GetWindowText(strText);

	long lVal = _ttol(strText);
	if (lVal < 1 || lVal > 20)
	{
		SetState(_T("Incorrect value: the correct value should be between 1 and 20"));
		return FALSE;
	}
	
	CleanState();
	return CBCGPProp::OnUpdateValue();
}
//*******************************************************************************
BOOL CRangeValidationProp::OnEndEdit()
{
	CleanState();
	return CBCGPProp::OnEndEdit();
}

////////////////////////////////////////////////////////////////////////////////
// CCustomStateProp

CCustomStateProp::CCustomStateProp(
								   const CString& strName,
								   LPCTSTR lpszVal,
								   LPCTSTR lpszDescr,
								   DWORD_PTR dwData) :
	CBCGPProp(strName, lpszVal, lpszDescr, dwData)
{
	m_bAllowEdit = FALSE;
	m_nAnimationStep = 0;

	m_bStopped = FALSE;
	m_strButtonText = _T("II");
	
	m_imageListAnimation.SetImageSize(CSize(14, 14));
	//m_imageListAnimation.Load(IDB_PROP_LIST_ANIMATION);
}
//*******************************************************************************
void CCustomStateProp::OnDrawStateIndicator(CDC* pDC, CRect rect)
{
	if (!m_bStopped)
	{
		m_imageListAnimation.DrawEx(pDC, rect, m_nAnimationStep, 
			CBCGPToolBarImages::ImageAlignHorzCenter, CBCGPToolBarImages::ImageAlignVertCenter);
	}
}
//*******************************************************************************
void CCustomStateProp::AnimationStep()
{
	if (m_bStopped)
	{
		return;
	}

	m_nAnimationStep++;

	if (m_nAnimationStep >= m_imageListAnimation.GetCount())
	{
		m_nAnimationStep = 0;
	}

	RedrawState();
}
//*******************************************************************************
void CCustomStateProp::OnClickButton (CPoint /*point*/)
{
	m_bStopped = !m_bStopped;
	m_strButtonText = m_bStopped ? _T(">") : _T("II");

	Redraw();
}

////////////////////////////////////////////////////////////////////////////////
// CHexValueProp

CHexValueProp::CHexValueProp(
					 const CString& strName,
					 long lValue,
					 LPCTSTR lpszDescr,
					 DWORD_PTR dwData) :
	CBCGPProp(strName, lValue, lpszDescr, dwData, NULL, NULL, _T("01234567890ABCDEFabcdef"))
{
}
//*******************************************************************************
CString CHexValueProp::FormatProperty()
{
	ASSERT_VALID (this);

	CString str;
	str.Format (_T("%X"), (long)m_varValue);

	return str;
}
//*******************************************************************************
BOOL CHexValueProp::TextToVar(const CString& str)
{
	if (str.IsEmpty())
	{
		m_varValue = 0l;
		return TRUE;
	}

	long lVal;
	
#if _MSC_VER < 1400
	_stscanf (str, _T("%X"), &lVal);
#else
	_stscanf_s (str, _T("%X"), &lVal);
#endif
	
	m_varValue = lVal;
	return TRUE;
}
//**********************************************************************************
BOOL CHexValueProp::SerializeValue(CString& str)
{
	str = CBCGPProp::FormatProperty();
	return str != _T("*** error ***");
}
//**********************************************************************************
BOOL CHexValueProp::ParseValue(const CString& str)
{
	return CBCGPProp::TextToVar(str);
}

////////////////////////////////////////////////////////////////////////////////
// CCustomColorProp

CCustomColorProp::CCustomColorProp(const CString& strName, const COLORREF& color, LPCTSTR lpszDescr /*= NULL*/) :
	CBCGPColorProp(strName, color, NULL, lpszDescr)
{
	EnableOtherButton (_T("Other..."));
	EnableAutomaticButton (_T("Default"), ::GetSysColor (COLOR_3DFACE));
}
//**********************************************************************************	
BOOL CCustomColorProp::OnDrawPaletteColorBox(CDC* pDC, const CRect rectColor, COLORREF color, BOOL /*bIsAutomatic*/, BOOL /*bIsSelected*/, BOOL /*bOnPopupMenu*/)
{
	CBCGPDrawManager dm(*pDC);
	dm.DrawEllipse(rectColor, color, globalData.clrBarDkShadow);

	return TRUE; 
}
