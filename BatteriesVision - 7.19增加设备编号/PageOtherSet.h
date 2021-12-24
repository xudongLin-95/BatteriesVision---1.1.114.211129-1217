#pragma once


// CPageOtherSet 对话框

class CPageOtherSet : public CBCGPDialog
{
	DECLARE_DYNAMIC(CPageOtherSet)

public:
	CPageOtherSet(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPageOtherSet();

// 对话框数据
	enum { IDD = IDD_PAGE_OTHER_SET };

	void ResetSize();
private:
	void InitUI();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void PostNcDestroy();
	afx_msg void OnBnClickedButtonRestart();
	afx_msg void OnBnClickedButtonVerifyInfo();
	afx_msg void OnBnClickedButtonAbout();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnBnClickedButtonUserManage();
	afx_msg void OnBnClickedButtonUserPower();
};
