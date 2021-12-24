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
// CustomProperties.h
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CUSTOMPROPERTIES_H__1EB0FB92_8DCC_4A06_948E_2AB23F319482__INCLUDED_)
#define AFX_CUSTOMPROPERTIES_H__1EB0FB92_8DCC_4A06_948E_2AB23F319482__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMyDurationCtrl : public CBCGPDurationCtrl
{
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	DECLARE_MESSAGE_MAP()
};

class CDurationProp : public CBCGPProp
{
// Construction
public:
	CDurationProp(const CString& strName, const COleDateTimeSpan& duration, 
		LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0);

// Overrides
public:
	virtual void OnDrawValue (CDC* pDC, CRect rect);
	virtual CWnd* CreateInPlaceEdit (CRect rectEdit, BOOL& bDefaultFormat);
	virtual void OnPosSizeChanged (CRect rectOld);
	virtual BOOL OnUpdateValue ();
	virtual void AdjustInPlaceEditRect (CRect& rectEdit, CRect& rectSpin);

	virtual void OnSetSelection (CBCGPProp* pOldSel);
	virtual void OnKillSelection (CBCGPProp* pNewSel);

	virtual BOOL PushChar (UINT nChar);
	virtual void SetValue (const _variant_t& varValue);

	virtual BOOL DoPaste();

// Attributes
public:
	void SetDuration (COleDateTimeSpan duration);
	COleDateTimeSpan GetDuration () const
	{
		return (COleDateTimeSpan) (double) m_varValue;
	}

protected:
	CMyDurationCtrl	m_wndDuration;
};

class CPasswordProp : public CBCGPProp
{
public:
	CPasswordProp(
		const CString& strName,
		const CString& strPassword,
		LPCTSTR lpszDescr = NULL,
		DWORD_PTR dwData = 0
	);

protected:
	virtual CWnd* CreateInPlaceEdit (CRect rectEdit, BOOL& bDefaultFormat);
	virtual CString FormatProperty ();
};

class CSliderProp : public CBCGPProp
{
public:
	CSliderProp(
		const CString& strName,
		long nValue,
		LPCTSTR lpszDescr = NULL,
		DWORD_PTR dwData = 0
	);

	virtual BOOL OnUpdateValue ();

protected:
	virtual CWnd* CreateInPlaceEdit (CRect rectEdit, BOOL& bDefaultFormat);
	virtual BOOL OnSetCursor () const	{	return FALSE; /* Use default */	}
	virtual BOOL OnClickValue (UINT uiMsg, CPoint point);
};

/////////////////////////////////////////////////////////////////////////////
// CPropSliderCtrl window

class CPropSliderCtrl : public CBCGPSliderCtrl
{
// Construction
public:
	CPropSliderCtrl(CSliderProp* pProp, COLORREF clrBack);

// Attributes
protected:
	CBrush			m_brBackground;
	COLORREF		m_clrBack;
	CSliderProp*	m_pProp;

// Operations
public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropSliderCtrl)
	//}}AFX_VIRTUAL

	virtual void OnDrawThumb (CDC* pDC, 
		CRect rect, BOOL bIsHighlighted, BOOL bIsPressed, BOOL bIsDisabled,
		BOOL bVert, BOOL bLeftTop, BOOL bRightBottom,
		BOOL bDrawOnGlass);

// Implementation
public:
	virtual ~CPropSliderCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPropSliderCtrl)
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	//}}AFX_MSG
	afx_msg void HScroll (UINT nSBCode, UINT nPos);

	DECLARE_MESSAGE_MAP()
};

class CIconListProp : public CBCGPProp
{
public:
	CIconListProp(
		const CString& strName,
		CBCGPToolBarImages& imageListIcons,
		int nSelectedIcon,
		CStringList* plstIconNames = NULL,
		LPCTSTR lpszDescr = NULL,
		DWORD_PTR dwData = 0
	);

protected:
	virtual CWnd* CreateInPlaceEdit (CRect rectEdit, BOOL& bDefaultFormat);
	virtual CComboBox* CreateCombo (CWnd* pWndParent, CRect rect);
	virtual void OnDrawValue (CDC* pDC, CRect rect);

	CBCGPToolBarImages&	m_imageListIcons;
	CStringList			m_lstIconNames;
};

/////////////////////////////////////////////////////////////////////////////
// CIconCombo window

class CIconCombo : public CBCGPComboBox
{
// Construction
public:
	CIconCombo(CBCGPToolBarImages& imageListIcons, CStringList& lstIconNames);

// Attributes
protected:
	CBCGPToolBarImages&	m_imageListIcons;
	CStringList&		m_lstIconNames;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIconCombo)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CIconCombo();

	// Generated message map functions
protected:
	//{{AFX_MSG(CIconCombo)
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

class CComboBoxExProp : public CBCGPProp
{
public:
	CComboBoxExProp(
		const CString& strName,
		const CString& strValue,
		LPCTSTR lpszDescr = NULL,
		DWORD_PTR dwData = 0,
		CBCGPToolBarImages* pImageList = NULL
	);

	BOOL AddOption (LPCTSTR lpszOption, int nIcon = -1, int nIndent = 0);

protected:
	virtual CComboBox* CreateCombo (CWnd* pWndParent, CRect rect);
	virtual BOOL OnEdit (LPPOINT lptClick);

	CImageList			m_ImageList;
	CArray<int, int>	m_lstIcons;
	CArray<int, int>	m_lstIndents;
};

class COwnerDrawDescrProp : public CBCGPProp
{
public:
	COwnerDrawDescrProp(
		const CString& strName,
		const _variant_t& varValue
	);

protected:
	virtual void OnDrawDescription (CDC* pDC, CRect rect);
};

class CTwoButtonsProp : public CBCGPProp
{
public:
	CTwoButtonsProp(
		const CString& strName,
		const _variant_t& varValue
	);

protected:
	virtual BOOL HasButton () const	{	return TRUE;	}
	virtual void AdjustButtonRect ();
	virtual void OnClickButton (CPoint point);
	virtual void OnDrawButton (CDC* pDC, CRect rectButton);

	CBCGPToolBarImages	m_images;
	int					m_iClickedButton;
};

class CComboButtonProp : public CBCGPProp
{
public:
	CComboButtonProp(
		const CString& strName,
		const _variant_t& varValue
		);
	
protected:
	virtual BOOL HasButton () const	{	return TRUE;	}
	virtual void AdjustButtonRect ();
	virtual void OnClickButton (CPoint point);
	virtual void OnDrawButton (CDC* pDC, CRect rectButton);
};

class CCustomDlgProp : public CBCGPProp
{
public:
	CCustomDlgProp(
		const CString& strName,
		const _variant_t& varValue
	);

protected:
	virtual BOOL HasButton () const	{	return TRUE;	}
	virtual void OnClickButton (CPoint point);
};

class CRangeValidationProp : public CBCGPProp
{
public:
	CRangeValidationProp(
		const CString& strName,
		long lValue,
		LPCTSTR lpszDescr = NULL,
		DWORD_PTR dwData = 0);

protected:
	virtual BOOL OnUpdateValue();
	virtual BOOL OnEndEdit();
};

class CCustomStateProp : public CBCGPProp
{
public:
	CCustomStateProp(
		const CString& strName,
		LPCTSTR lpszVal,
		LPCTSTR lpszDescr = NULL,
		DWORD_PTR dwData = 0);

	void AnimationStep();
	
protected:
	virtual void OnDrawStateIndicator(CDC* pDC, CRect rect);

	virtual BOOL HasButton () const	{	return TRUE;	}
	virtual void OnClickButton (CPoint point);

	int					m_nAnimationStep;
	BOOL				m_bStopped;
	CBCGPToolBarImages	m_imageListAnimation;
};

class CHexValueProp : public CBCGPProp
{
public:
	CHexValueProp(
		const CString& strName,
		long lValue,
		LPCTSTR lpszDescr = NULL,
		DWORD_PTR dwData = 0);
	
protected:
	virtual CString FormatProperty();
	virtual BOOL TextToVar(const CString& strText);

	virtual BOOL SerializeValue(CString& str);
	virtual BOOL ParseValue(const CString& str);
};

class CCustomColorProp : public CBCGPColorProp
{
public:
	CCustomColorProp(const CString& strName, const COLORREF& color, LPCTSTR lpszDescr = NULL);
	
	virtual BOOL OnDrawPaletteColorBox(CDC* pDC, const CRect rectColor, COLORREF color, BOOL bIsAutomatic, BOOL bIsSelected, BOOL bOnPopupMenu);
};


#endif // !defined(AFX_CUSTOMPROPERTIES_H__1EB0FB92_8DCC_4A06_948E_2AB23F319482__INCLUDED_)

/////////////////////////////////////////////////////////////////////////////
