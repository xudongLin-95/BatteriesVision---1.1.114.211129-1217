#pragma once
#include "BetaRay.h"

// CDlgSet �Ի���

class CDlgSet : public CBCGPDialog
{
	DECLARE_DYNAMIC(CDlgSet)

public:
	CDlgSet(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgSet();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum {
		IDD = IDD_DIALOG_SET
};
#endif

	void InitUI();

	void RefreshData();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	DECLARE_MESSAGE_MAP()

public:
	CBetaRay *m_pOwner;
	CBCGPGridCtrl m_Grid_Data;
public:
	virtual BOOL OnInitDialog();
   
public:
	afx_msg void OnBnClickedButtonConnect();
	CBCGPComboBox m_Combo_BetaRayType;
	int m_EDIT_nPort;
	afx_msg void OnBnClickedButtonExit();
	CString m_EDIT_strIP;
	afx_msg void OnBnClickedButtonSaveExit();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	double m_EDIT_dWeightStd;
	double m_EDIT_dWeightTol;
};
