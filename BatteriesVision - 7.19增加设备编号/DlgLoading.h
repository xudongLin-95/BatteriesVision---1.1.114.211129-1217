#pragma once
#include "CxStatic.h"
#include "ProgressCtrlExt.h"
#include "afxwin.h"

// CDlgLoading �Ի���

class CDlgLoading : public CBCGPDialog
{
	DECLARE_DYNAMIC(CDlgLoading)

public:
	CDlgLoading(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgLoading();

// �Ի�������
	enum { IDD = IDD_DIALOG_LOADING };

	void InitUI();

	void SetLoadingInfo(const CString& strInfoCN,const CString& strInfoEN);
	void SetLoadingInfo(const CString& strInfoCN,const CString& strInfoEN,double dPercent);
	void InitProgressBar();
	void SetProgressPercent(double& dPercent);
	void ShowProgramVersion();
	CString GetFileVersion(CString strFileName);

	CFont m_ftTitle;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CxStatic m_STC_Title;
	CBCGPCircularProgressIndicatorCtrl m_ProgressIndicator;
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	CxStatic m_STC_LoadInfo;
	afx_msg LRESULT OnNcHitTest(CPoint point);
	virtual void PostNcDestroy();
	CProgressCtrlExt m_Progress_Loading;
	CxStatic m_STC_Version;
};
