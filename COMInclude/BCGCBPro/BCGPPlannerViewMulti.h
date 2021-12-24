//*******************************************************************************
// COPYRIGHT NOTES
// ---------------
// This is a part of BCGControlBar Library Professional Edition
// Copyright (C) 1998-2018 BCGSoft Ltd.
// All rights reserved.
//
// This source code can be used, distributed or modified
// only under terms and conditions 
// of the accompanying license agreement.
//*******************************************************************************
//
// BCGPPlannerViewMulti.h: interface for the CBCGPPlannerViewMulti class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BCGPPLANNERVIEWMULTI_H__99CF9A8D_73D3_4D83_836D_0E35487383E9__INCLUDED_)
#define AFX_BCGPPLANNERVIEWMULTI_H__99CF9A8D_73D3_4D83_836D_0E35487383E9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BCGCBPro.h"

#ifndef BCGP_EXCLUDE_PLANNER

#include "BCGPPlannerViewDay.h"

class BCGCBPRODLLEXPORT CBCGPPlannerViewMulti : public CBCGPPlannerViewDay  
{
	friend class CBCGPPlannerManagerCtrl;

	DECLARE_DYNCREATE(CBCGPPlannerViewMulti)

public:
	virtual ~CBCGPPlannerViewMulti();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	virtual UINT GetCurrentResourceID () const;
	virtual BOOL SetCurrentResourceID (UINT nResourceID, BOOL bRedraw = TRUE, BOOL bNotify = FALSE);
	virtual BOOL OnUpdateStorage ();

	virtual CRect GetRectFromDate(const COleDateTime& date) const;	

protected:
	CBCGPPlannerViewMulti();

protected:
	virtual UINT GetResourceFromPoint (const CPoint& point) const;
	int FindResourceIndexByID (UINT nResourceID) const;

	virtual void AdjustLayout (CDC* pDC, const CRect& rectClient);
	virtual void AdjustRects ();
	virtual void AdjustAppointments ();

	virtual void AdjustCaptionFormat (CDC* pDC);

	virtual BCGP_PLANNER_HITTEST HitTestArea (const CPoint& point) const;

	virtual void OnPaint (CDC* pDC, const CRect& rectClient);
	virtual void OnDrawClient (CDC* pDC, const CRect& rect);
	virtual void OnDrawHeaderResource (CDC* pDC, const CRect& rectHeader);
	virtual void OnDrawHeaderAllDay (CDC* pDC, const CRect& rectHeader);
	virtual void OnDrawAppointmentsDuration (CDC* pDC);

	virtual void InitViewToolTipInfo ();
	virtual CString GetToolTipText (const CPoint& point);

	virtual CString GetAccName() const;

protected:
	UINT m_nResourceID;
	XResourceCollection	m_Resources;
};

#endif // BCGP_EXCLUDE_PLANNER

#endif // !defined(AFX_BCGPPLANNERVIEWMULTI_H__99CF9A8D_73D3_4D83_836D_0E35487383E9__INCLUDED_)