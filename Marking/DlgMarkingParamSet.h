#pragma once

#include "MarkingCtr.h"
#include "resource.h"
#include "afxwin.h"
#include "BCGCBProInc.h"

// CDlgMarkingParamSet 对话框

class CDlgMarkingParamSet : public CBCGPDialog
{
	DECLARE_DYNAMIC(CDlgMarkingParamSet)

public:
	CDlgMarkingParamSet(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgMarkingParamSet();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_PLC_MARKINGSET };
#endif
	CMarkingCtr *m_pOwner;

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	void InitUI();
	void RefreshButton();

	DECLARE_MESSAGE_MAP()
public:
	int m_EDIT_nPort;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonSenddata();
	afx_msg void OnBnClickedButtonReceivedata();
	afx_msg void OnBnClickedButtonIsconnected();
	afx_msg void OnBnClickedButtonCancel();
	CString m_EDIT_Diff;           //存储最新图像数
	CString m_Edit_DivImageNum;    //每张图按照200mm一份划分的个数
	CString m_Edit_MarkingWidth;   //喷码器喷涂的宽度[以左侧铂材为基准，到右侧最后铂材结束] 
	CString m_Edit_MarkingHeight;  //喷码器喷涂的高度[默认200mm一份]
};
