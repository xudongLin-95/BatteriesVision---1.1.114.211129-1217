#pragma once
#include "Blendent.h"
#include "afxdialogex.h"
#define BUTTON_CLOSE WM_USER+100
#define STATIC_TITLE WM_USER+101


// CDialogExt 对话框

class W_LIB CDialogExt : public CDialog
{
	DECLARE_DYNAMIC(CDialogExt)
public:
	CBlendent m_Blendent;
public:
	CDialogExt(UINT nIDD,CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDialogExt();

// 对话框数据
	//enum { IDD = IDD_DIALOGEXT };

	CStaticExt m_STC_Title;
	CString m_strTitle;
	CButtonExt m_BTN_close;
	void InitUI();
	CFont m_ftButton;

	void SetButton(int nID);
	void SetEdit(int nID);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedButtonClose();
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
