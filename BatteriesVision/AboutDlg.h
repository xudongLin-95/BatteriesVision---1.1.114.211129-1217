#pragma once
#include "BCGCBProInc.h"

class CAboutDlg : public CBCGPDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	void InitUI();

	void RefreshModuleVersion();
	void RefreshVersionInfo();

	CString GetFileVersion(CString strFileName);
	CString GetModulePath(CString strModuleName);
private:
	CBCGPGridCtrl m_Grid_VersionList;
	CBCGPEditCtrl m_EDIT_UpdateInfo;
// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void PostNcDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};