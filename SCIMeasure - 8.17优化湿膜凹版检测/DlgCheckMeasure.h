#pragma once
#include "SCIMeasure.in.h"

// CDlgCheckMeasure �Ի���

class CDlgCheckMeasure : public CBCGPDialog
{
	DECLARE_DYNAMIC(CDlgCheckMeasure)

public:
	CDlgCheckMeasure(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgCheckMeasure();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_CHECK_MEASURE };
#endif

	void InitUI();

public:
	CMeasure*		m_pTemp;		//ģ�������
	CXmvImageWnd m_ImageWnd[30];
	MeasureInput	m_tInput;		//�������
	MeasureOutput	m_tOutput;		//������
	CFont			m_ftTitle;

	CBCGPDialog* m_pParent;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonCancel();
	afx_msg void OnBnClickedButtonOk();
};
