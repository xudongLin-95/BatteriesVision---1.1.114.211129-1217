#pragma once


// CStatusBarExt

class W_LIB CStatusBarExt : public CStatusBar
{
	DECLARE_DYNAMIC(CStatusBarExt)
public:
	COLORREF m_statusTextColor,m_statusBackColor;
	CFont    m_textFont;
public:
	CStatusBarExt();
	virtual ~CStatusBarExt();
    void DrawPaneContent(UINT pane,CDC* hdc,RECT rect);
	void SetBackgroundColor(COLORREF clr);
	void SetTextColor(COLORREF clr);
	COLORREF GetBackgroundColor();
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
public:
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
};


