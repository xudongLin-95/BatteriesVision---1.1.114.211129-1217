#pragma once
#include "afxwin.h"


// CDlgSimulateSet �Ի���

class CDlgSimulateSet : public CBCGPDialog
{
	DECLARE_DYNAMIC(CDlgSimulateSet)

public:
	CDlgSimulateSet(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgSimulateSet();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum {IDD = IDD_DIALOG_SIMUATE_SET};
#endif

	void InitUI();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CBCGPEdit m_EDT_SimulatePath;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
