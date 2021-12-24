#pragma once
#include "LoggerManager.h"
#include "LogViewDlg.h"
#include "BCGCBProInc.h"

// CLoggerManagerDlg �Ի���

class CLoggerManagerDlg : public CBCGPDialog
{
	DECLARE_DYNAMIC(CLoggerManagerDlg)

public:
	CLoggerManagerDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CLoggerManagerDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LOGGERMANAGERDLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

private:
	CFont m_ftProp;
	CBCGPStatic m_Static_PropList;
	CBCGPGridCtrl m_GridList;
	CBCGPPropList m_PropList;
public:
	std::vector<CLogger*> m_vecPLogger;
public:
	virtual BOOL OnInitDialog();
	void InitUI();
	void SaveParam();
//	afx_msg void OnClose();
	afx_msg void OnDestroy();
	virtual void OnOK();
	afx_msg void OnBnClickedButtonViewlog();
	virtual void PostNcDestroy();
};
