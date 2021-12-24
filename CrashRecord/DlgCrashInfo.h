#pragma once


// CDlgCrashInfo 对话框

class CDlgCrashInfo : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgCrashInfo)

public:
	CDlgCrashInfo(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgCrashInfo();

// 对话框数据
	enum { IDD = IDD_DIALOG_INFO };

	void InitUI();

private:
	CString GetExceptionInfo(DWORD dwCode);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	CListBox m_LST_Info;
	afx_msg void OnBnClickedCancel();
};
