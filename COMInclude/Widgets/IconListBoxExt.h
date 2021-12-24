#pragma once
#include "afxwin.h"


// CIconListBoxExt

class W_LIB CIconListBoxExt : public CListBox
{
	DECLARE_DYNAMIC(CIconListBoxExt)

public:
	CIconListBoxExt();
	virtual ~CIconListBoxExt();

protected:
	DECLARE_MESSAGE_MAP()
	// 图片列表变量
	CImageList* m_pImageList;
public:
	int AddString(LPCTSTR lpszItem);
	int AddString(LPCTSTR lpszItem, int iImg);
	int InsertString(int iIndex, LPCTSTR lpszItem);
	int InsertString(int iIndex, LPCTSTR lpszItem, int iImg);
	void SetItemImage(int iIndex, int iImg);

	inline void SetImageList(CImageList* pImgList= NULL)
	{m_pImageList = pImgList;}


	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
};


