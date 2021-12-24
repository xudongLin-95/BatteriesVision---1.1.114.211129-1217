#pragma once
#include "afxwin.h"


// CDlgSimulateSet 对话框

class CDlgSimulateSet : public CBCGPDialog
{
	DECLARE_DYNAMIC(CDlgSimulateSet)

public:
	CDlgSimulateSet(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgSimulateSet();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum {IDD = IDD_DIALOG_SIMUATE_SET};
#endif

	void InitUI();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CBCGPEdit m_EDT_SimulatePath;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
