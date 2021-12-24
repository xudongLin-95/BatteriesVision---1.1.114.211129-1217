#pragma once


// CDlgSwitchBatch 对话框

class CDlgSwitchBatch : public CBCGPDialog
{
	DECLARE_DYNAMIC(CDlgSwitchBatch)

public:
	CDlgSwitchBatch(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgSwitchBatch();

// 对话框数据
	enum { IDD = IDD_DIALOG_SWITCH_BATCH };

	void InitUI();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonGetbatch();
};
