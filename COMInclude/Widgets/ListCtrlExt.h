#pragma once
#include "HeaderCtrlExt.h"


class CEditCl : public CEdit
{
	DECLARE_DYNAMIC(CEditCl)

public:
	CEditCl();
	virtual ~CEditCl();
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnKillFocus(CWnd* pNewWnd);
};

// CListCtrlExt
struct stColor
{
	int nRow;
	int nCol;
	COLORREF rgb;
};


enum ListType
{
	ReadOnly,
	ProductInfo,
	IO,
};
class W_LIB CListCtrlExt : public CListCtrl
{
	DECLARE_DYNAMIC(CListCtrlExt)

public:
	CHeaderCtrlExt m_Header;
	CListCtrlExt();
	virtual ~CListCtrlExt();

protected:
	DECLARE_MESSAGE_MAP()
	virtual void PreSubclassWindow();
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
public:
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
public:
	ListType m_eType;
	// �и�
	int m_nRowHeight;
	int InsertColumn(int nCol, LPCTSTR lpszColumnHeading,
		int nFormat = LVCFMT_LEFT, int nWidth = -1, int nSubItem = -1);
public:
	// Gradient - ����ϵ�������屳����,���ý�����Ϊ0
	void SetHeaderBKColor(int R, int G, int B, int Gradient);
public:
	// ���ñ�ͷ�߶�
	void SetHeaderHeight(float Height);
	CPtrList m_ptrListCol;  //��������ɫ
	CPtrList m_ptrListItem; //����Item��ɫ��
	CPtrList m_colTextColor; //������������ɫ
	CPtrList m_ItemTextColor; //���浥Ԫ��������ɫ
	bool FindColColor(int col ,COLORREF &color); //��������ɫ
	bool FindItemColor(int col,int row,COLORREF &color);
	bool FindColTextColor(int col,COLORREF &color); //������������ɫ
	bool FindItemTextColor(int col,int row,COLORREF &color);
	void SetColColor(int col,COLORREF color);  //��������ɫ
	void SetItemColor(int col,int row,COLORREF color); //����Item��ɫ
	void SetColTextColor(int col,COLORREF color);   //�������ı���ɫ
	void SetItemTextColor(int col,int row,COLORREF color);
	void SetRowHeigt(int nHeight); //�����и�
	void SetHeaderFontHW(int nHeight,int nWith); //���ñ�ͷ�����С
	void SetHeaderTextColor(COLORREF color);
	COLORREF m_color;
	BOOL SetTextColor(COLORREF cr);
	void SetFontHW(int nHeight,int nWith);  //��������ĸߺͿ�

	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);//˫���ɱ༭
	afx_msg LRESULT OnEditEnd(WPARAM wParam, LPARAM lParam = FALSE);//�༭���
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
public:
	// ����߶�
	int m_fontHeight;
public:
	// ������
	int m_fontWith;
protected:
	CEditCl m_Edit;
	int m_nRow,m_nColumn;
};

