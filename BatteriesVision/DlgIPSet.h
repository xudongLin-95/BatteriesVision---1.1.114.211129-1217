#pragma once


// CDlgIPSet 对话框

class CDlgIPSet : public CBCGPDialog
{
	DECLARE_DYNAMIC(CDlgIPSet)

public:
	CDlgIPSet(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgIPSet();

// 对话框数据
//#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_IP_SET };
//#endif

	void InitUI();

public:
	CBCGPGridCtrl m_Grid_Base;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};
