#pragma once


// CDlgSwitchBatch �Ի���

class CDlgSwitchBatch : public CBCGPDialog
{
	DECLARE_DYNAMIC(CDlgSwitchBatch)

public:
	CDlgSwitchBatch(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgSwitchBatch();

// �Ի�������
	enum { IDD = IDD_DIALOG_SWITCH_BATCH };

	void InitUI();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonGetbatch();
};
