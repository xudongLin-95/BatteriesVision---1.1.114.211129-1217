#pragma once


// DlgUserManage 对话框

class DlgUserManage : public CBCGPDialog
{
	DECLARE_DYNAMIC(DlgUserManage)

public:
	DlgUserManage(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~DlgUserManage();

// 对话框数据
	enum { IDD = IDD_DIALOG_USERMANAGE };
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnBnClickedButtonDelete();
	afx_msg void OnBnClickedButtonChange();
public:
	void initList();
	void loadList();
public:
	PowerLimitManage* m_pOwner;
	CBCGPGridCtrl m_listUser;
	CFont m_ListFont;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
