#pragma once


// CDlgCrashInfo �Ի���

class CDlgCrashInfo : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgCrashInfo)

public:
	CDlgCrashInfo(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgCrashInfo();

// �Ի�������
	enum { IDD = IDD_DIALOG_INFO };

	void InitUI();

private:
	CString GetExceptionInfo(DWORD dwCode);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	CListBox m_LST_Info;
	afx_msg void OnBnClickedCancel();
};
