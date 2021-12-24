#pragma once


// DlgUploadAlarm 对话框

class DlgUploadAlarm : public CBCGPDialog
{
	DECLARE_DYNAMIC(DlgUploadAlarm)

public:
	DlgUploadAlarm(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~DlgUploadAlarm();

// 对话框数据
	enum { IDD = IDD_DIALOG_UPLOAD_ALARM };

	void ShowAlarm(CString strAlarmInfo);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();

	CListCtrl m_LIST_cUploadAlarm;
	virtual void PostNcDestroy();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
