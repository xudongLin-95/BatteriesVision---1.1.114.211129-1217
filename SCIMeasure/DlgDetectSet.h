#pragma once
#include "Detect.h"


// CDlgDetectSet 对话框

class CDlgDetectSet : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgDetectSet)

public:
	CDlgDetectSet(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgDetectSet();

// 对话框数据
	enum { IDD = IDD_DIALOG_DETECT_SET };

	CSeparate* m_pOwner;

private:
	void InitUI();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CReportCtrlExt m_LST_Coat;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonCancel();
	double m_dCoatShrink;
	afx_msg void OnBnClickedButtonOk();
};
