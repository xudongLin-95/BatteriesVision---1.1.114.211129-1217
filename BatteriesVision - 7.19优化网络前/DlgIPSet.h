#pragma once


// CDlgIPSet �Ի���

class CDlgIPSet : public CBCGPDialog
{
	DECLARE_DYNAMIC(CDlgIPSet)

public:
	CDlgIPSet(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgIPSet();

// �Ի�������
//#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_IP_SET };
//#endif

	void InitUI();

public:
	CBCGPGridCtrl m_Grid_Base;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};
